/*
 * bench.c — CPU bandwidth + latency benchmarks and system info.
 * Refactored from Membench-NX/main.c into result-returning functions with no
 * console I/O, so a GUI (borealis) can drive them from a worker thread.
 *
 * Original bandwidth/latency methodology:
 *   Copyright (c) 2011 Siarhei Siamashka, (c) 20xx KazushiMe, (c) 2025 Souldbminer
 */

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <switch.h>

#include "bench.h"
#include "gpu_bw.h"
#include <sys/time.h>

#define SIZE (32 * 1024 * 1024)
#define MAXREPEATS 10
#define LATBENCH_COUNT 10000000
#define ALIGN_PADDING 0x100000
#define CACHE_LINE_SIZE 128

#define BW_MAX_THREADS 4

typedef void (*bench_kernel_fn)(int64_t *dst, int64_t *src, long size);

struct bw_pool;

typedef struct {
    Thread thread;
    Semaphore start;
    struct bw_pool *pool;
    bench_kernel_fn func;
    int64_t *dst;
    int64_t *src;
    long size;
    int core;
    bool created;
} bw_worker_t;

typedef struct bw_pool {
    bw_worker_t workers[BW_MAX_THREADS];
    Semaphore done;
    int threads;
    bool ok;
} bw_pool_t;

static void bw_worker_entry(void *arg) {
    bw_worker_t *w = (bw_worker_t *)arg;
    bw_pool_t *p = w->pool;
    /* Best-effort pin to own core. */
    svcSetThreadCoreMask(CUR_THREAD_HANDLE, w->core, (u64)1 << w->core);
    for (;;) {
        semaphoreWait(&w->start);
        bench_kernel_fn f = w->func;
        if (!f)
            break;
        f(w->dst, w->src, w->size);
        semaphoreSignal(&p->done);
    }
}

static bool bw_pool_init(bw_pool_t *p, int threads) {
    memset(p, 0, sizeof(*p));
    if (threads > BW_MAX_THREADS)
        threads = BW_MAX_THREADS;
    p->threads = threads;
    semaphoreInit(&p->done, 0);
    s32 prio = 0x2C;
    svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
    for (int i = 0; i < threads; i++) {
        bw_worker_t *w = &p->workers[i];
        w->core = i;
        w->func = NULL;
        w->pool = p;
        semaphoreInit(&w->start, 0);
        if (R_FAILED(threadCreate(&w->thread, bw_worker_entry, w, NULL, 0x8000, prio, i)))
            return false;
        w->created = true;
        if (R_FAILED(threadStart(&w->thread)))
            return false;
    }
    p->ok = true;
    return true;
}

static void bw_pool_free(bw_pool_t *p) {
    for (int i = 0; i < p->threads; i++) {
        bw_worker_t *w = &p->workers[i];
        if (w->created) {
            w->func = NULL; /* shutdown */
            semaphoreSignal(&w->start);
        }
    }
    for (int i = 0; i < p->threads; i++) {
        bw_worker_t *w = &p->workers[i];
        if (w->created) {
            threadWaitForExit(&w->thread);
            threadClose(&w->thread);
            w->created = false;
        }
    }
    p->ok = false;
}

static double bw_run_once(bw_pool_t *p, bench_kernel_fn f, int64_t *dstbuf, int64_t *srcbuf, long size) {
    int threads = p->threads;
    for (int i = 0; i < threads; i++) {
        bw_worker_t *w = &p->workers[i];
        w->func = f;
        w->dst = dstbuf + (size * i) / (long)sizeof(int64_t);
        w->src = srcbuf + (size * i) / (long)sizeof(int64_t);
        w->size = size;
    }
    uint64_t t0 = armGetSystemTick();
    for (int i = 0; i < threads; i++)
        semaphoreSignal(&p->workers[i].start);
    for (int i = 0; i < threads; i++)
        semaphoreWait(&p->done);
    uint64_t t1 = armGetSystemTick();
    return (double)armTicksToNs(t1 - t0) / 1000000000.0;
}

#define BW_GPR_CLOBBERS \
    "x3", "x4", "x5", "x6", "x7", "x8", "x9", "x10", "x11", "x12", "x13", "x14", "x15", "x16", "x17", "x18"

static void aligned_block_copy(int64_t *dst, int64_t *src, long size) {
    __asm__ __volatile__(
        "1:\n"
        "ldp x3, x4,   [%[s], #0x00]\n"
        "ldp x5, x6,   [%[s], #0x10]\n"
        "ldp x7, x8,   [%[s], #0x20]\n"
        "ldp x9, x10,  [%[s], #0x30]\n"
        "ldp x11, x12, [%[s], #0x40]\n"
        "ldp x13, x14, [%[s], #0x50]\n"
        "ldp x15, x16, [%[s], #0x60]\n"
        "ldp x17, x18, [%[s], #0x70]\n"
        "add %[s], %[s], #0x80\n"
        "stp x3, x4,   [%[d], #0x00]\n"
        "stp x5, x6,   [%[d], #0x10]\n"
        "stp x7, x8,   [%[d], #0x20]\n"
        "stp x9, x10,  [%[d], #0x30]\n"
        "stp x11, x12, [%[d], #0x40]\n"
        "stp x13, x14, [%[d], #0x50]\n"
        "stp x15, x16, [%[d], #0x60]\n"
        "stp x17, x18, [%[d], #0x70]\n"
        "add %[d], %[d], #0x80\n"
        "subs %[n], %[n], #0x80\n"
        "b.gt 1b\n"
        : [s] "+r"(src), [d] "+r"(dst), [n] "+r"(size)
        :
        : BW_GPR_CLOBBERS, "cc", "memory");
}
static void aligned_block_fetch(int64_t *dst, int64_t *src, long size) {
    (void)src;
    __asm__ __volatile__(
        "1:\n"
        "ldp x3, x4,   [%[p], #0x00]\n"
        "ldp x5, x6,   [%[p], #0x10]\n"
        "ldp x7, x8,   [%[p], #0x20]\n"
        "ldp x9, x10,  [%[p], #0x30]\n"
        "ldp x11, x12, [%[p], #0x40]\n"
        "ldp x13, x14, [%[p], #0x50]\n"
        "ldp x15, x16, [%[p], #0x60]\n"
        "ldp x17, x18, [%[p], #0x70]\n"
        "add %[p], %[p], #0x80\n"
        "subs %[n], %[n], #0x80\n"
        "b.gt 1b\n"
        : [p] "+r"(dst), [n] "+r"(size)
        :
        : BW_GPR_CLOBBERS, "cc", "memory");
}
static void aligned_block_fill(int64_t *dst, int64_t *src, long size) {
    (void)src;
    __asm__ __volatile__(
        "1:\n"
        "stp x3, x4,   [%[d], #0x00]\n"
        "stp x5, x6,   [%[d], #0x10]\n"
        "stp x7, x8,   [%[d], #0x20]\n"
        "stp x9, x10,  [%[d], #0x30]\n"
        "stp x11, x12, [%[d], #0x40]\n"
        "stp x13, x14, [%[d], #0x50]\n"
        "stp x15, x16, [%[d], #0x60]\n"
        "stp x17, x18, [%[d], #0x70]\n"
        "add %[d], %[d], #0x80\n"
        "subs %[n], %[n], #0x80\n"
        "b.gt 1b\n"
        : [d] "+r"(dst), [n] "+r"(size)
        :
        : BW_GPR_CLOBBERS, "cc", "memory");
}

static double gettime(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)((int64_t)tv.tv_sec * 1000000 + tv.tv_usec) / 1000000.;
}

static double bandwidth_bench_helper(bw_pool_t *pool, int threads, int64_t *dstbuf, int64_t *srcbuf, long size, bench_kernel_fn f) {
    int i, loopcount, innerloopcount, n;
    double t, speed, maxspeed, s, s0, s1, s2;

    if (!pool || !pool->ok)
        return 0.;

    s = s0 = s1 = s2 = 0.;
    maxspeed = 0.;
    for (n = 0; n < MAXREPEATS; n++) {
        loopcount = 0;
        innerloopcount = 1;
        t = 0.;
        do {
            loopcount += innerloopcount;
            for (i = 0; i < innerloopcount; i++)
                t += bw_run_once(pool, f, dstbuf, srcbuf, size);
            innerloopcount *= 2;
        } while (t < 0.5);

        speed = (double)size * threads * loopcount / t / 1000000.;
        s0 += 1.;
        s1 += speed;
        s2 += speed * speed;
        if (speed > maxspeed)
            maxspeed = speed;
        if (s0 > 2.) {
            s = sqrt((s0 * s2 - s1 * s1) / (s0 * (s0 - 1)));
            if (s < maxspeed / 1000.)
                break;
        }
    }
    return maxspeed;
}

static char *align_up(char *ptr, int align) {
    return (char *)(((uintptr_t)ptr + align - 1) & ~(uintptr_t)(align - 1));
}

static void *alloc_nonaliased_buffers(void **buf1_, int size1, void **buf2_, int size2, void **buf3_, int size3) {
    char **buf1 = (char **)buf1_, **buf2 = (char **)buf2_, **buf3 = (char **)buf3_;
    int mask = (ALIGN_PADDING - 1) & ~(CACHE_LINE_SIZE - 1);
    char *buf = malloc(size1 + size2 + size3 + 9 * ALIGN_PADDING);
    char *ptr = buf;
    memset(buf, 0xCC, size1 + size2 + size3 + 9 * ALIGN_PADDING);
    ptr = align_up(ptr, ALIGN_PADDING);
    if (buf1) {
        *buf1 = ptr + (0xAAAAAAAA & mask);
        ptr = align_up(*buf1 + size1, ALIGN_PADDING);
    }
    if (buf2) {
        *buf2 = ptr + (0x55555555 & mask);
        ptr = align_up(*buf2 + size2, ALIGN_PADDING);
    }
    if (buf3) {
        *buf3 = ptr + (0xCCCCCCCC & mask);
    }
    return buf;
}

#pragma GCC diagnostic push
static void __attribute__((noinline)) random_read_test(char *buf, int count, int nbits) {
    uint32_t seed = 0;
    uintptr_t mask = (1 << nbits) - 1;
    uint32_t v;
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
    static volatile uint32_t dummy;
#define RMA()                         \
    seed = seed * 1103515245 + 12345; \
    v = (seed >> 16) & 0xFF;          \
    seed = seed * 1103515245 + 12345; \
    v |= (seed >> 8) & 0xFF00;        \
    seed = seed * 1103515245 + 12345; \
    v |= seed & 0x7FFF0000;           \
    seed |= buf[v & mask];
    while (count >= 16) {
        RMA() RMA() RMA() RMA() RMA() RMA() RMA() RMA() RMA() RMA() RMA() RMA() RMA() RMA() RMA() RMA() count -= 16;
    }
    dummy = seed;
#undef RMA
}
#pragma GCC diagnostic pop

static double latency_measure(char *buf, int nbits, int count) {
    double t_noaccess = 0, t_before, t_after, t, xs1 = 0, xs2 = 0, min_t = 0;
    double xs;
    int n;

    for (n = 1; n <= MAXREPEATS; n++) {
        t_before = gettime();
        random_read_test(buf, count, 1);
        t_after = gettime();
        if (n == 1 || t_after - t_before < t_noaccess)
            t_noaccess = t_after - t_before;
    }
    for (n = 1; n <= MAXREPEATS; n++) {
        t_before = gettime();
        random_read_test(buf, count, nbits);
        t_after = gettime();
        t = t_after - t_before - t_noaccess;
        if (t < 0)
            t = 0;
        xs1 += t;
        xs2 += t * t;
        if (n == 1 || t < min_t)
            min_t = t;
        if (n > 2) {
            xs = sqrt((xs2 * n - xs1 * xs1) / (n * (n - 1)));
            if (xs < min_t / 1000.)
                break;
        }
    }
    return min_t * 1000000000.0 / count;
}

static void latency_bench(double *l2_out, double *ram_out) {
    char *buf_alloc = malloc(0x2001000);
    char *buf = (char *)(((uintptr_t)buf_alloc + 4095) & ~(uintptr_t)4095);
    memset(buf, 0, 0x2000000);
    *l2_out = latency_measure(buf, 20, LATBENCH_COUNT);
    *ram_out = latency_measure(buf, 25, LATBENCH_COUNT);
    free(buf_alloc);
}

void bench_get_sysinfo(sysinfo_t *out) {
    memset(out, 0, sizeof(*out));
    out->threads = 3;
    out->is_4gb = (appletGetAppletType() == AppletType_Application);
    if (R_SUCCEEDED(clkrstInitialize())) {
        ClkrstSession s;
        clkrstOpenSession(&s, PcvModuleId_CpuBus, 3);
        clkrstGetClockRate(&s, &out->cpu_hz);
        clkrstCloseSession(&s);
        clkrstOpenSession(&s, PcvModuleId_GPU, 3);
        clkrstGetClockRate(&s, &out->gpu_hz);
        clkrstCloseSession(&s);
        clkrstOpenSession(&s, PcvModuleId_EMC, 3);
        clkrstGetClockRate(&s, &out->mem_hz);
        clkrstCloseSession(&s);
        clkrstExit();
    }
}

void bench_run_full(bench_results_t *out, bench_progress_fn progress, void *user) {
    const int threads = 3;
    const int size = SIZE;
    bool is_4gb = (appletGetAppletType() == AppletType_Application);
    memset(out, 0, sizeof(*out));

#define STEP(label, frac)                    \
    do {                                     \
        if (progress)                        \
            progress((label), (frac), user); \
    } while (0)

    STEP("GPU bandwidth", 0.05f);
    gpu_bw_run(is_4gb, &out->gpu_copy, &out->gpu_read, &out->gpu_write);

    int64_t *srcbuf, *dstbuf;
    void *poolbuf = alloc_nonaliased_buffers((void **)&srcbuf, size * threads, (void **)&dstbuf, size * threads, NULL, 0);
    bw_pool_t bw;
    bw_pool_init(&bw, threads);

    STEP("CPU copy", 0.40f);
    out->cpu_copy = bandwidth_bench_helper(&bw, threads, dstbuf, srcbuf, size, aligned_block_copy);
    STEP("CPU read", 0.55f);
    out->cpu_read = bandwidth_bench_helper(&bw, threads, dstbuf, srcbuf, size, aligned_block_fetch);
    STEP("CPU write", 0.70f);
    out->cpu_write = bandwidth_bench_helper(&bw, threads, dstbuf, srcbuf, size, aligned_block_fill);
    bw_pool_free(&bw);
    free(poolbuf);

    STEP("Latency", 0.85f);
    latency_bench(&out->l2_ns, &out->ram_ns);

    STEP("Done", 1.0f);
#undef STEP
}

struct bench_ctx {
    int phase;
    bool is_4gb;
    void *buf;
    int64_t *src;
    int64_t *dst;
    bw_pool_t bw;
};

bench_ctx *bench_begin(void) {
    bench_ctx *c = (bench_ctx *)calloc(1, sizeof(bench_ctx));
    if (!c)
        return NULL;
    c->is_4gb = (appletGetAppletType() == AppletType_Application);
    c->buf = alloc_nonaliased_buffers((void **)&c->src, SIZE * 3, (void **)&c->dst, SIZE * 3, NULL, 0);
    bw_pool_init(&c->bw, 3);
    return c;
}

bool bench_step(bench_ctx *c, bench_results_t *out, const char **label, float *frac) {
    const int threads = 3;
    const int size = SIZE;
    switch (c->phase) {
        case 0:
            gpu_bw_run(c->is_4gb, &out->gpu_copy, &out->gpu_read, &out->gpu_write);
            *label = "GPU bandwidth";
            *frac = 0.25f;
            break;
        case 1:
            out->cpu_copy = bandwidth_bench_helper(&c->bw, threads, c->dst, c->src, size, aligned_block_copy);
            *label = "CPU copy";
            *frac = 0.45f;
            break;
        case 2:
            out->cpu_read = bandwidth_bench_helper(&c->bw, threads, c->dst, c->src, size, aligned_block_fetch);
            *label = "CPU read";
            *frac = 0.60f;
            break;
        case 3:
            out->cpu_write = bandwidth_bench_helper(&c->bw, threads, c->dst, c->src, size, aligned_block_fill);
            *label = "CPU write";
            *frac = 0.75f;
            break;
        case 4:
            bw_pool_free(&c->bw);
            if (c->buf) {
                free(c->buf);
                c->buf = NULL;
            }
            latency_bench(&out->l2_ns, &out->ram_ns);
            *label = "Latency";
            *frac = 0.95f;
            break;
        default:
            *label = "Done";
            *frac = 1.0f;
            return false;
    }
    c->phase++;
    return true;
}

void bench_end(bench_ctx *c) {
    if (!c)
        return;
    bw_pool_free(&c->bw);
    if (c->buf)
        free(c->buf);
    free(c);
}