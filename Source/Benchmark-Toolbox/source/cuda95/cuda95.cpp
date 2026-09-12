#include "cuda95.h"

#include <switch.h>
#include <deko3d.h>

#include <atomic>
#include <cstdarg>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>

#ifndef DK_UNIFORM_BUF_ALIGNMENT
#define DK_UNIFORM_BUF_ALIGNMENT 0x100
#endif

#define ALIGN_UP(x, a) (((x) + ((a) - 1)) & ~((uint32_t)(a) - 1))

#define WG_SIZE_X 16
#define WG_SIZE_Y 16
// groups are dispatched in a 4x4 grid, for 2048 threads total
// 2048 per SM = 4096 on TX1, so we fully saturate the GPU
#define NUM_GROUPS_X 4
#define NUM_GROUPS_Y 4

#define N_THREADS ((NUM_GROUPS_X * WG_SIZE_X) * (NUM_GROUPS_Y * WG_SIZE_Y))  // 4096
#define N_ROWS 64

// Thread buffers = N_ROWS * N_THREADS, must be power of 2
#define N_VEC ((uint64_t)N_ROWS * N_THREADS)   // uvec4 elements, 2^18
#define N_ELEMS (N_VEC * 4)                      // uints, 2^20

static_assert((N_VEC & (N_VEC - 1)) == 0,
               "N_ROWS * N_THREADS must be a power of two for the scatter mask");

// Gather size, 8192 * 16B = 128KB per SM. Fits into L2 perfectly
#define L2_WINDOW_VEC 8192u
#define DISPATCHES_PER_LIST 32
#define NUM_INFLIGHT 3
#define MAX_RECORDS 64          // fault records per batch, per slot

#define FENCE_TIMEOUT_NS (5ULL * 1000000000ULL)
#define DATA_CHECK_INTERVAL 512

#define CODEMEM_SIZE (128 * 1024)
#define CMDMEM_SIZE (512 * 1024)

#define DKSH_MAGIC UINT32_C(0x48534B44)

// TOOLBOX: console drawing removed, log kept as a ring buffer.
#define SCR_COLS 79
#define LOG_LINES 14

typedef struct {
    uint32_t magic;
    uint32_t header_sz;
    uint32_t control_sz;
    uint32_t code_sz;
    uint32_t programs_off;
    uint32_t num_programs;
} DkshHeaderLocal;

// Shader parameters
typedef struct {
    uint32_t n_rows;
    uint32_t n_repeats;
    uint32_t alu_iters;
    uint32_t gather_iters;
    uint32_t max_records;
    uint32_t mode;          // 0 = capture golden, 1 = compare, 2 = ignore
    uint32_t scatter_mask;  // Gather window - 1, used for errors.
    uint32_t chain_bias;
} Params;

// Result buffer
typedef struct {
    uint32_t mismatches;
    uint32_t digest;
    uint32_t selfFail;
    uint32_t errCount;
} GpuResult;

// Errorlog
typedef struct {
    uint32_t id;            // thread index
    uint32_t stage;         // 1 = ALU self-check, 2 = per-thread digest
    uint32_t got;
    uint32_t want;
} GpuRecord;


// Presets for calibration
typedef struct {
    const char* name;
    uint32_t    alu;
    uint32_t    gather;
    uint32_t    window;     // 0 = whole buffer
} Profile;

// Testing modes
// Balanced tried to hit both
// Core forces 2x chains and not as much memory access, although in practice it strains it enough.
// Dram scatters reads accross allocated size (4MB, lol, lmao even)
// L2 changes this to 256KB.
static const Profile kProfiles[] = {
    { "balanced", 64, 32, 0 },
    { "core", 512, 4, 0 },
    { "dram", 8, 256, 0 },
    { "l2", 8, 256, L2_WINDOW_VEC },
};
#define NUM_PROFILES ((int)(sizeof(kProfiles) / sizeof(kProfiles[0])))

static uint32_t scatterMaskFor(int profile)
{
    uint32_t win = kProfiles[profile].window;
    if (win == 0u) win = (uint32_t)N_VEC;
    return win - 1u;
}

// Error storage
typedef struct {
    uint64_t batches;
    uint64_t detected;      // batches where digest=/count
    uint64_t transient;     // corrected by the re-run vote
    uint64_t persistent;    // reproduced on re-run
    uint64_t selfFail;      // ALU DUP/COMP Fails
    uint64_t dataBad;       // corrupt inputs (how did this happen)
    uint64_t warnings;
    uint64_t timeouts;
} Tally;

static Tally g_tally;
static uint64_t g_tStart;

static char g_log[LOG_LINES][SCR_COLS + 1];
static uint32_t g_logCount;

// TOOLBOX: shared state
static std::thread g_thread;
static std::mutex g_lock;
static std::atomic<bool> g_stop{ false };
static std::atomic<bool> g_running{ false };
static cuda95_status_t g_pub{};

static int s_profile;
static unsigned s_repeats;
static uint32_t s_goldDigest;
static uint32_t s_goldMismatch;
static double s_dps;
static double s_gbps;
static int s_cal;
static int s_error;
static char s_state[96];

static int s_cfgProfile;
static unsigned s_cfgRepeats;
static int s_wantProfile;
static unsigned s_wantRepeats;
static bool s_wantDirty;

static uint64_t elapsedSec(void)
{
    return armTicksToNs(armGetSystemTick() - g_tStart) / 1000000000ULL;
}

static void note(const char* fmt, ...)
{
    char body[SCR_COLS + 1];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(body, sizeof(body), fmt, ap);
    va_end(ap);

    // TOOLBOX: console branch removed
    uint32_t idx = g_logCount % LOG_LINES;
    int n = snprintf(g_log[idx], sizeof(g_log[idx]), "[%5llus] ",
                     (unsigned long long)elapsedSec());
    if (n < 0) n = 0;
    size_t off = (size_t)n;
    if (off > sizeof(g_log[idx]) - 1) off = sizeof(g_log[idx]) - 1;
    size_t room = sizeof(g_log[idx]) - 1 - off;
    size_t len = strlen(body);
    if (len > room) len = room;
    memcpy(g_log[idx] + off, body, len);
    g_log[idx][off + len] = 0;
    g_logCount++;
}

// Generate seeds
static uint32_t hash32(uint32_t x)
{
    x ^= x >> 16;
    x *= 0x7FEB352Du;
    x ^= x >> 15;
    x *= 0x846CA68Bu;
    x ^= x >> 16;
    return x;
}


// RNG, clamped so nothing fucking explodes too hard
static uint32_t dataBits(uint64_t index)
{
    uint32_t bits = hash32((uint32_t)index);
    uint32_t exp  = (bits >> 23) & 0xFFu;
    if (exp < 0x30u) exp = 0x30u + (exp & 0x0Fu);
    if (exp > 0xC0u) exp = 0xC0u - (exp & 0x0Fu);
    return (bits & 0x807FFFFFu) | (exp << 23);
}


typedef struct {
    DkMemBlock block;
    uint32_t offset;
    uint32_t size;
} BumpAlloc;

static uint32_t bumpAlloc(BumpAlloc* a, uint32_t size, uint32_t align)
{
    uint32_t off = ALIGN_UP(a->offset, align);
    if (off + size > a->size) {
        note("out of memory (need %u, have %u)", off + size, a->size);
        return UINT32_MAX;
    }
    a->offset = off + size;
    return off;
}

static void* cpuPtr(BumpAlloc* a, uint32_t off)
{
    return (uint8_t*)dkMemBlockGetCpuAddr(a->block) + off;
}

static DkGpuAddr gpuPtr(BumpAlloc* a, uint32_t off)
{
    return dkMemBlockGetGpuAddr(a->block) + off;
}

// TOOLBOX: copy worker state for the UI thread.
static void publish(void)
{
    cuda95_status_t s{};
    s.batches = g_tally.batches;
    s.detected = g_tally.detected;
    s.transient = g_tally.transient;
    s.persistent = g_tally.persistent;
    s.selfFail = g_tally.selfFail;
    s.dataBad = g_tally.dataBad;
    s.warnings = g_tally.warnings;
    s.timeouts = g_tally.timeouts;
    s.dispPerSec = s_dps;
    s.gbps = s_gbps;
    s.goldDigest = s_goldDigest;
    s.goldMismatch = s_goldMismatch;
    s.elapsedSec = elapsedSec();
    s.profile = s_profile;
    s.repeats = s_repeats;
    s.calibrating = s_cal;
    s.running = g_running.load() ? 1 : 0;
    s.error = s_error;
    snprintf(s.status, sizeof(s.status), "%s", s_state);

    uint32_t total = g_logCount < LOG_LINES ? g_logCount : LOG_LINES;
    uint32_t keep = total < CUDA95_LOG_LINES ? total : CUDA95_LOG_LINES;
    s.logLines = keep;
    for (unsigned i = 0; i < keep; i++) {
        uint32_t idx = (g_logCount - keep + i) % LOG_LINES;
        snprintf(s.log[i], sizeof(s.log[i]), "%s", g_log[idx]);
    }

    std::lock_guard<std::mutex> lk(g_lock);
    g_pub = s;
}

static void debugCallback(void* userData, const char* context,
                          DkResult result, const char* message)
{
    (void)userData;
    if (result == DkResult_Success) {
        g_tally.warnings++;
        note("deko3d warn [%s]: %s", context, message);
        publish();
        return;
    }
    // TOOLBOX: flag the error instead.
    note("deko3d ERROR [%s]: %s", context, message);
    s_error = 1;
    snprintf(s_state, sizeof(s_state), "GPU error");
    publish();
    g_stop.store(true);
}

static bool loadShader(DkShader* shader, BumpAlloc* codeMem, const char* path)
{
    FILE* f = fopen(path, "rb");
    if (!f) { note("cannot open %s", path); return false; }

    DkshHeaderLocal hdr;
    if (fread(&hdr, sizeof(hdr), 1, f) != 1 || hdr.magic != DKSH_MAGIC) {
        note("%s is not a DKSH file", path);
        fclose(f);
        return false;
    }

    void* control = malloc(hdr.control_sz);
    if (!control) { fclose(f); return false; }

    rewind(f);
    if (fread(control, hdr.control_sz, 1, f) != 1) {
        free(control); fclose(f); return false;
    }

    uint32_t codeOff = bumpAlloc(codeMem, hdr.code_sz, DK_SHADER_CODE_ALIGNMENT);
    if (codeOff == UINT32_MAX) { free(control); fclose(f); return false; }

    fseek(f, hdr.control_sz, SEEK_SET);
    if (fread(cpuPtr(codeMem, codeOff), hdr.code_sz, 1, f) != 1) {
        free(control); fclose(f); return false;
    }
    fclose(f);

    DkShaderMaker maker;
    dkShaderMakerDefaults(&maker, codeMem->block, codeOff);
    maker.control = control;
    dkShaderInitialize(shader, &maker);

    free(control);
    return dkShaderIsValid(shader);
}

// Usual Deko3D memory allocation
typedef struct {
    DkQueue queue;
    DkCmdBuf cmdbuf;
    DkShader shader;

    DkGpuAddr uboAddr;
    uint32_t uboSize;

    //3x slots, two are constantly worked on while a third is checked
    DkGpuAddr resultAddr[NUM_INFLIGHT];
    GpuResult* resultCpu[NUM_INFLIGHT];
    DkGpuAddr recAddr[NUM_INFLIGHT];
    GpuRecord* recCpu[NUM_INFLIGHT];
    DkCmdList batch[NUM_INFLIGHT];

    DkGpuAddr dataAddr;
    uint32_t dataSize;
    uint32_t* dataCpu;

    // Golden digests
    DkGpuAddr expAddr;
    uint32_t expSize;
} Ctx;

static void recordBatches(Ctx* c, Params params, uint32_t dispatches)
{
    dkCmdBufClear(c->cmdbuf);

    for (uint32_t s = 0; s < NUM_INFLIGHT; s++) {
        const DkShader* shaders[] = { &c->shader };
        dkCmdBufBindShaders(c->cmdbuf, DkStageFlag_Compute, shaders, 1);

        DkBufExtents ubo = { c->uboAddr, c->uboSize };
        dkCmdBufBindUniformBuffers(c->cmdbuf, DkStage_Compute, 0, &ubo, 1);
        dkCmdBufPushConstants(c->cmdbuf, c->uboAddr, c->uboSize,
                              0, sizeof(params), &params);

        DkBufExtents ssbos[4];
        ssbos[0].addr = c->dataAddr;      ssbos[0].size = c->dataSize;
        ssbos[1].addr = c->resultAddr[s]; ssbos[1].size = sizeof(GpuResult);
        ssbos[2].addr = c->recAddr[s];    ssbos[2].size = MAX_RECORDS * sizeof(GpuRecord);
        ssbos[3].addr = c->expAddr;       ssbos[3].size = c->expSize;
        dkCmdBufBindStorageBuffers(c->cmdbuf, DkStage_Compute, 0, ssbos, 4);

        for (uint32_t i = 0; i < dispatches; i++)
            dkCmdBufDispatchCompute(c->cmdbuf, NUM_GROUPS_X, NUM_GROUPS_Y, 1);

        c->batch[s] = dkCmdBufFinishList(c->cmdbuf);
    }
}

typedef enum { RUN_OK = 0, RUN_TIMEOUT, RUN_QUEUE_ERROR } RunStatus;

static RunStatus runSync(Ctx* c, uint32_t slot, GpuResult* out, uint64_t* outNs)
{
    memset(c->resultCpu[slot], 0, sizeof(GpuResult));

    DkFence fence;
    memset(&fence, 0, sizeof(fence));

    uint64_t t0 = armGetSystemTick();
    dkQueueSubmitCommands(c->queue, c->batch[slot]);
    // CPU politely asks for the data
    dkQueueSignalFence(c->queue, &fence, true);
    dkQueueFlush(c->queue);

    DkResult r = dkFenceWait(&fence, (int64_t)FENCE_TIMEOUT_NS);
    uint64_t t1 = armGetSystemTick();
    if (outNs) *outNs = armTicksToNs(t1 - t0);

    // FAHHHHHHH
    if (r != DkResult_Success)
        return dkQueueIsInErrorState(c->queue) ? RUN_QUEUE_ERROR : RUN_TIMEOUT;
    if (dkQueueIsInErrorState(c->queue))
        return RUN_QUEUE_ERROR;

    *out = *c->resultCpu[slot];
    return RUN_OK;
}

// Gets reports from GPU results, mainly fault records
static void reportRecords(Ctx* c, uint32_t slot, uint32_t errCount)
{
    uint32_t shown = errCount < MAX_RECORDS ? errCount : MAX_RECORDS;
    if (shown > 3) shown = 3;

    for (uint32_t i = 0; i < shown; i++) {
        GpuRecord r = c->recCpu[slot][i];
        note("  rec thread %u %s got %08x want %08x", r.id,
             r.stage == 1u ? "ALU" : "digest", r.got, r.want);
    }
    if (errCount > shown)
        note("  (%u more records)", errCount - shown);
}


// Seperated because this way we can distinguish between source corruption and the GPU on a bender.
static uint32_t checkDataIntegrity(Ctx* c)
{
    const uint32_t stride = 997;
    uint32_t bad = 0;
    for (uint64_t i = 0; i < N_ELEMS; i += stride)
        if (c->dataCpu[i] != dataBits(i)) bad++;
    return bad;
}


// Capture golden values to confirm them as valid.
// Runs which cannot reproduce their own digests as valid 3 times are assumed as always unstable.
// Used to refuse baking in bad data and instead re compute.
static bool calibrate(Ctx* c, Params base, uint32_t* goldDigest, uint32_t* goldMismatch)
{
    GpuResult r;
    Params p = base;

    // Capture data
    p.mode = 0;
    recordBatches(c, p, 1);
    if (runSync(c, 0, &r, NULL) != RUN_OK) {
        note("calibration: capture run failed");
        return false;
    }

    // Compare against what we have
    p.mode = 1;
    recordBatches(c, p, 1);

    uint32_t d[3], m[3];
    for (int i = 0; i < 3; i++) {
        // TOOLBOX: abort calibration when stopped.
        if (g_stop.load()) return false;
        if (runSync(c, 0, &r, NULL) != RUN_OK) {
            note("calibration: confirm run %d failed", i + 1);
            return false;
        }
        d[i] = r.digest;
        m[i] = r.mismatches;
        if (r.selfFail || r.errCount) {
            g_tally.selfFail += r.selfFail;
            note("calibration: run %d reported %u selfFail, %u records",
                 i + 1, r.selfFail, r.errCount);
            reportRecords(c, 0, r.errCount);
        }
    }

    if (d[0] == d[1] && d[1] == d[2]) {
        *goldDigest = d[0];
        *goldMismatch = m[0];
        return true;
    }

    // Weigh the results, if nothing appeared twice, then flag these values.
    g_tally.detected++;
    if (d[0] == d[1] || d[0] == d[2])      { *goldDigest = d[0]; *goldMismatch = m[0]; }
    else if (d[1] == d[2])                 { *goldDigest = d[1]; *goldMismatch = m[1]; }
    else                                   { *goldDigest = d[0]; *goldMismatch = m[0]; }
    note("CALIBRATION UNSTABLE: %08x %08x %08x -- reference may be wrong",
         d[0], d[1], d[2]);
    return true;
}

// TOOLBOX: worker thread, replace runDemo() loop.
static void workerMain(void)
{
    g_running.store(true);
    appletSetAutoSleepDisabled(true);

    {
        std::lock_guard<std::mutex> lk(g_lock);
        s_profile = s_cfgProfile;
        s_repeats = s_cfgRepeats;
    }
    int profile = s_profile;
    uint32_t repeats = s_repeats;

    memset(&g_tally, 0, sizeof(g_tally));
    g_logCount = 0;
    g_tStart = armGetSystemTick();
    s_goldDigest = 0;
    s_goldMismatch = 0;
    s_dps = 0.0;
    s_gbps = 0.0;
    s_cal = 0;
    s_error = 0;
    snprintf(s_state, sizeof(s_state), "starting");

    Ctx c;
    memset(&c, 0, sizeof(c));

    note("CUDA95 %d rows x %d threads", N_ROWS, N_THREADS);

    DkDeviceMaker devMaker;
    dkDeviceMakerDefaults(&devMaker);
    devMaker.cbDebug = debugCallback;
    DkDevice device = dkDeviceCreate(&devMaker);

    // Setup queues
    DkQueueMaker queueMaker;
    dkQueueMakerDefaults(&queueMaker, device);
    queueMaker.flags = DkQueueFlags_Compute
                     | DkQueueFlags_MediumPrio
                     | DkQueueFlags_DisableZcull;
    queueMaker.commandMemorySize = 512 * 1024;
    queueMaker.flushThreshold = 128 * 1024;
    DkQueue queue = dkQueueCreate(&queueMaker);
    c.queue = queue;

    // Allocate and create memory regions
    DkMemBlockMaker mbMaker;
    dkMemBlockMakerDefaults(&mbMaker, device, CODEMEM_SIZE);
    mbMaker.flags = DkMemBlockFlags_CpuUncached
                  | DkMemBlockFlags_GpuCached
                  | DkMemBlockFlags_Code;
    BumpAlloc codeMem = { dkMemBlockCreate(&mbMaker), 0, CODEMEM_SIZE };

    uint32_t dataSize = (uint32_t)(N_ELEMS * sizeof(uint32_t));   // 4 MiB
    uint32_t expSize = N_THREADS * sizeof(uint32_t);             // 16 KiB
    uint32_t recSize = MAX_RECORDS * sizeof(GpuRecord);          // 1 KiB
    uint32_t dataMemSize = ALIGN_UP(CMDMEM_SIZE + dataSize + expSize
                                    + NUM_INFLIGHT * (recSize + 0x100) + 0x4000,
                                    DK_MEMBLOCK_ALIGNMENT);

    dkMemBlockMakerDefaults(&mbMaker, device, dataMemSize);
    mbMaker.flags = DkMemBlockFlags_CpuUncached | DkMemBlockFlags_GpuCached;
    BumpAlloc dataMem = { dkMemBlockCreate(&mbMaker), 0, dataMemSize };

    DkCmdBufMaker cbMaker;
    dkCmdBufMakerDefaults(&cbMaker, device);
    c.cmdbuf = dkCmdBufCreate(&cbMaker);

    uint32_t cmdOff = bumpAlloc(&dataMem, CMDMEM_SIZE, DK_CMDMEM_ALIGNMENT);
    dkCmdBufAddMemory(c.cmdbuf, dataMem.block, cmdOff, CMDMEM_SIZE);

    c.uboSize = ALIGN_UP(sizeof(Params), DK_UNIFORM_BUF_ALIGNMENT);
    uint32_t uboOff = bumpAlloc(&dataMem, c.uboSize, DK_UNIFORM_BUF_ALIGNMENT);
    c.uboAddr = gpuPtr(&dataMem, uboOff);

    // Allocate the result slots so we do not stall
    for (uint32_t s = 0; s < NUM_INFLIGHT; s++) {
        uint32_t ro = bumpAlloc(&dataMem, sizeof(GpuResult), 0x100);
        c.resultAddr[s] = gpuPtr(&dataMem, ro);
        c.resultCpu[s] = (GpuResult*)cpuPtr(&dataMem, ro);

        uint32_t eo = bumpAlloc(&dataMem, recSize, 0x100);
        c.recAddr[s] = gpuPtr(&dataMem, eo);
        c.recCpu[s] = (GpuRecord*)cpuPtr(&dataMem, eo);
    }

    uint32_t expOff = bumpAlloc(&dataMem, expSize, 0x100);
    c.expAddr = gpuPtr(&dataMem, expOff);
    c.expSize = expSize;

    uint32_t dataOff = bumpAlloc(&dataMem, dataSize, 0x100);
    c.dataAddr = gpuPtr(&dataMem, dataOff);
    c.dataSize = dataSize;
    c.dataCpu = (uint32_t*)cpuPtr(&dataMem, dataOff);

    // Fill memory
    note("filling %u KiB of test data...", dataSize / 1024);
    for (uint64_t i = 0; i < N_ELEMS; i++) {
        if ((i & 0xFFFFF) == 0 && g_stop.load()) break;
        c.dataCpu[i] = dataBits(i);
    }

    uint32_t goldDigest = 0, goldMismatch = 0;
    uint32_t expDigest = 0, expMismatch = 0;
    bool ready = false;

    // TOOLBOX: host app owns romfs
    if (!g_stop.load() && loadShader(&c.shader, &codeMem, "romfs:/shaders/stress_csh.dksh")) {
        Params base = { N_ROWS, repeats,
                        kProfiles[profile].alu, kProfiles[profile].gather,
                        MAX_RECORDS, 1, scatterMaskFor(profile), 0u };

        s_cal = 1;
        snprintf(s_state, sizeof(s_state), "calibrating");
        note("calibrating (%s, repeats %u)...", kProfiles[profile].name, repeats);
        publish();
        if (!g_stop.load() && calibrate(&c, base, &goldDigest, &goldMismatch)) {
            expDigest = goldDigest * DISPATCHES_PER_LIST;
            expMismatch = goldMismatch * DISPATCHES_PER_LIST;
            s_goldDigest = goldDigest;
            s_goldMismatch = goldMismatch;
            note("golden digest %08x, mismatches %u", goldDigest, goldMismatch);

            Params runParams = base;
            runParams.mode = 1;
            recordBatches(&c, runParams, DISPATCHES_PER_LIST);
            ready = true;
        } else if (!g_stop.load()) {
            s_error = 1;
            snprintf(s_state, sizeof(s_state), "calibration failed");
            note("initial calibration failed, aborting");
        }
        s_cal = 0;

        if (ready) {
            // Dispatches are idential and use atomicAdd, so batches must be DISPATCHES_PER_lIST * 1 dispatch.
            DkFence fences[NUM_INFLIGHT];
            memset(fences, 0, sizeof(fences));

            uint64_t submitted = 0;
            uint64_t windowDisp = 0;
            uint64_t tWindow = armGetSystemTick();
            double dps = 0.0;
            bool dirty = false;
            bool fatal = false;
            snprintf(s_state, sizeof(s_state), "running");
            publish();

            while (!g_stop.load() && !fatal) {
                // TOOLBOX: settings from the UI
                {
                    std::lock_guard<std::mutex> lk(g_lock);
                    if (s_wantDirty) {
                        s_wantDirty = false;
                        profile = s_wantProfile;
                        repeats = s_wantRepeats;
                        s_profile = profile;
                        s_repeats = repeats;
                        dirty = true;
                    }
                }

                if (dirty) {
                    // If we get an error, recapture golden digest for comparisons to actually work.
                    dkQueueWaitIdle(queue);

                    base.n_repeats = repeats;
                    base.alu_iters = kProfiles[profile].alu;
                    base.gather_iters = kProfiles[profile].gather;
                    base.scatter_mask = scatterMaskFor(profile);
                    base.mode = 1;

                    s_cal = 1;
                    snprintf(s_state, sizeof(s_state), "calibrating");
                    note("recalibrating (%s, repeats %u)...", kProfiles[profile].name, repeats);
                    publish();

                    if (g_stop.load()) break;
                    if (!calibrate(&c, base, &goldDigest, &goldMismatch)) {
                        note("recalibration failed");
                        s_error = 1;
                        snprintf(s_state, sizeof(s_state), "recalibration failed");
                        fatal = true;
                        s_cal = 0;
                        publish();
                        break;
                    }
                    expDigest = goldDigest * DISPATCHES_PER_LIST;
                    expMismatch = goldMismatch * DISPATCHES_PER_LIST;
                    s_goldDigest = goldDigest;
                    s_goldMismatch = goldMismatch;
                    note("calibrated %s repeats %u -> digest %08x",
                         kProfiles[profile].name, repeats, goldDigest);

                    Params runParams = base;
                    runParams.mode = 1;
                    recordBatches(&c, runParams, DISPATCHES_PER_LIST);

                    memset(fences, 0, sizeof(fences));
                    submitted = 0;
                    dirty = false;
                    s_cal = 0;
                    snprintf(s_state, sizeof(s_state), "running");
                    publish();
                }

                uint32_t slot = (uint32_t)(submitted % NUM_INFLIGHT);

                if (submitted >= NUM_INFLIGHT) {
                    DkResult r = dkFenceWait(&fences[slot], (int64_t)FENCE_TIMEOUT_NS);

                    if (dkQueueIsInErrorState(queue)) {
                        note("FATAL: queue entered error state");
                        s_error = 1;
                        snprintf(s_state, sizeof(s_state), "GPU queue error");
                        fatal = true;
                        publish();
                        break;
                    }
                    if (r != DkResult_Success) {
                        g_tally.timeouts++;
                        note("FATAL: batch exceeded %llu s deadline",
                             (unsigned long long)(FENCE_TIMEOUT_NS / 1000000000ULL));
                        s_error = 1;
                        snprintf(s_state, sizeof(s_state), "batch timeout");
                        fatal = true;
                        publish();
                        break;
                    }

                    g_tally.batches++;
                    GpuResult res = *c.resultCpu[slot];

                    bool bad = (res.digest != expDigest) || (res.mismatches != expMismatch);

                    // Shader COMP/DUP still occurs even if digests are bad.
                    // Allows to distinguish what died.
                    if (res.selfFail) {
                        g_tally.selfFail += res.selfFail;
                        note("batch %llu: %u ALU self-check failures",
                             (unsigned long long)g_tally.batches, res.selfFail);
                        bad = true;
                    }
                    if (res.errCount) {
                        reportRecords(&c, slot, res.errCount);
                        bad = true;
                    }

                    // We outa stability
                    if (bad) {
                        if (res.digest != expDigest)
                            note("batch %llu: digest %08x want %08x",
                                 (unsigned long long)g_tally.batches, res.digest, expDigest);

                        g_tally.detected++;

                        // Re run batches twice and average, mainly for transience.
                        dkQueueWaitIdle(queue);
                        GpuResult a, b;
                        RunStatus s1 = runSync(&c, slot, &a, NULL);
                        RunStatus s2 = runSync(&c, slot, &b, NULL);
                        if (s1 != RUN_OK || s2 != RUN_OK) {
                            note("FATAL: re-run failed during vote");
                            s_error = 1;
                            snprintf(s_state, sizeof(s_state), "re-run failed");
                            fatal = true;
                            publish();
                            break;
                        }

                        // If we rerun, replay the same list so batches are compared to what they should result in, not the golden value for single dispatches
                        bool aOk = (a.digest == expDigest) && !a.selfFail && !a.errCount;
                        bool bOk = (b.digest == expDigest) && !b.selfFail && !b.errCount;

                        // If we detect transience, then we sorta ball, if its persistent, fucking overvolt.
                        if (aOk && bOk) {
                            g_tally.transient++;
                            note("  -> transient, corrected");
                        } else {
                            g_tally.persistent++;
                            note("  -> PERSISTENT (re-runs %08x, %08x)", a.digest, b.digest);
                        }

                        // How did this happen
                        // A long time ago
                        uint32_t badElems = checkDataIntegrity(&c);
                        if (badElems) {
                            g_tally.dataBad += badElems;
                            note("  -> input buffer: %u sampled elements wrong", badElems);
                        }

                        publish();
                        memset(fences, 0, sizeof(fences));
                        submitted = 0;
                        continue;
                    }

                    // que
                    if ((g_tally.batches % DATA_CHECK_INTERVAL) == 0) {
                        uint32_t badElems = checkDataIntegrity(&c);
                        if (badElems) {
                            g_tally.dataBad += badElems;
                            note("input buffer: %u sampled elements wrong", badElems);
                        }
                        publish();
                    }
                }

                memset(c.resultCpu[slot], 0, sizeof(GpuResult));
                dkQueueSubmitCommands(queue, c.batch[slot]);
                dkQueueSignalFence(queue, &fences[slot], true);
                dkQueueFlush(queue);

                submitted++;
                windowDisp += DISPATCHES_PER_LIST;

                uint64_t now = armGetSystemTick();
                uint64_t windowNs = armTicksToNs(now - tWindow);
                if (windowNs >= 500000000ULL) {
                    double secs = windowNs / 1.0e9;
                    dps = windowDisp / secs;

                    double bytesPer = (double)N_THREADS * 16.0 * repeats
                                    * ((double)N_ROWS + (double)kProfiles[profile].gather);

                    s_dps = dps;
                    s_gbps = (dps * bytesPer) / 1.0e9;
                    publish();

                    windowDisp = 0;
                    tWindow = now;
                }
            }
        }
    } else if (!g_stop.load()) {
        s_error = 1;
        snprintf(s_state, sizeof(s_state), "shader load failed");
        note("could not load stress_csh.dksh, aborting");
        publish();
    }

    if (!dkQueueIsInErrorState(queue))
        dkQueueWaitIdle(queue);
    dkCmdBufDestroy(c.cmdbuf);
    dkMemBlockDestroy(dataMem.block);
    dkMemBlockDestroy(codeMem.block);
    dkQueueDestroy(queue);
    dkDeviceDestroy(device);

    if (!s_error && g_stop.load())
        snprintf(s_state, sizeof(s_state), "stopped");
    publish();

    appletSetAutoSleepDisabled(false);
    g_running.store(false);
    publish();
}

static unsigned normRepeats(unsigned r)
{
    if (r < CUDA95_REPEATS_MIN) r = CUDA95_REPEATS_MIN;
    if (r > CUDA95_REPEATS_MAX) r = CUDA95_REPEATS_MAX;
    unsigned p = 1;
    while (p * 2u <= r) p *= 2u;
    return p;
}

static int normProfile(int p)
{
    if (p < 0) return 0;
    if (p >= NUM_PROFILES) return NUM_PROFILES - 1;
    return p;
}

extern "C" {

void cuda95_start(int profile, unsigned repeats)
{
    cuda95_stop();

    {
        std::lock_guard<std::mutex> lk(g_lock);
        s_cfgProfile = normProfile(profile);
        s_cfgRepeats = normRepeats(repeats);
        s_wantProfile = s_cfgProfile;
        s_wantRepeats = s_cfgRepeats;
        s_wantDirty = false;
        memset(&g_pub, 0, sizeof(g_pub));
        snprintf(g_pub.status, sizeof(g_pub.status), "starting");
    }

    g_stop.store(false);
    g_thread = std::thread(workerMain);
}

void cuda95_stop(void)
{
    g_stop.store(true);
    if (g_thread.joinable())
        g_thread.join();
}

int cuda95_running(void)
{
    return g_running.load() ? 1 : 0;
}

void cuda95_get(cuda95_status_t *out)
{
    if (!out) return;
    std::lock_guard<std::mutex> lk(g_lock);
    *out = g_pub;
}

void cuda95_set_profile(int profile)
{
    std::lock_guard<std::mutex> lk(g_lock);
    s_cfgProfile = normProfile(profile);
    if (g_running.load()) {
        s_wantProfile = s_cfgProfile;
        s_wantDirty = true;
    }
}

void cuda95_set_repeats(unsigned repeats)
{
    std::lock_guard<std::mutex> lk(g_lock);
    s_cfgRepeats = normRepeats(repeats);
    if (g_running.load()) {
        s_wantRepeats = s_cfgRepeats;
        s_wantDirty = true;
    }
}

const char *cuda95_profile_name(int profile)
{
    profile = normProfile(profile);
    return kProfiles[profile].name;
}

unsigned cuda95_profile_alu(int profile)
{
    profile = normProfile(profile);
    return kProfiles[profile].alu;
}

unsigned cuda95_profile_gather(int profile)
{
    profile = normProfile(profile);
    return kProfiles[profile].gather;
}

} // extern C

