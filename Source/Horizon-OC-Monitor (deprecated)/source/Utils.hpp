#pragma once
#define ALWAYS_INLINE inline __attribute__((always_inline))
#include "SaltyNX.h"

#include "Battery.hpp"
#include "audsnoop.h"
#include "Misc.hpp"
#include "max17050.h"
#include "tmp451.h"
#include "pwm.h"
#include <numeric>
#include <tesla.hpp>
#include <sys/stat.h>

// rgltr_services.cpp (no changes needed here—just compile it once)
#include <switch.h>
#include "rgltr.h"
#include "rgltr_services.h"  // for extern Service g_rgltrSrv, etc.



#if defined(__cplusplus)
extern "C"
{
#endif

#include <hocclk/client/ipc.h>

#if defined(__cplusplus)
}
#endif

#define NVGPU_GPU_IOCTL_PMU_GET_GPU_LOAD 0x80044715
#define FieldDescriptor uint32_t
#define BASE_SNS_UOHM 5000

static bool fixHiding = false;
static bool fixForeground = false;

//Common
bool isMariko = false;
Thread t0;
Thread t1;
Thread t2;
Thread t3;
Thread t4;
Thread t5;
Thread t6;
Thread t7;
uint64_t systemtickfrequency = 19200000;

LEvent threadexit;
PwmChannelSession g_ICon;
const std::string folderpath = "sdmc:/switch/.overlays/";

// Use const string_view for paths to avoid string copying
constexpr const char* directoryPath = "sdmc:/config/status-monitor/";
constexpr const char* configIniPath = "sdmc:/config/status-monitor/config.ini";
constexpr const char* ultrahandConfigIniPath = "sdmc:/config/ultrahand/config.ini";
constexpr const char* teslaConfigIniPath = "sdmc:/config/tesla/config.ini";

std::string filename;
std::string filepath;
std::string keyCombo = "ZL+ZR+DDOWN"; // default Ultrahand Menu combo

//Misc2
MmuRequest nvdecRequest;
MmuRequest nvencRequest;
MmuRequest nvjpgRequest;

//Checks
Result nvCheck = 1;
Result i2cCheck = 1;
Result pwmCheck = 1;
Result tcCheck = 1;
Result Hinted = 1;
Result pmdmntCheck = 1;
Result psmCheck = 1;
Result audsnoopCheck = 1;
Result nvdecCheck = 1;
Result nvencCheck = 1;
Result nvjpgCheck = 1;
Result nifmCheck = 1;
Result hocclkCheck = 1;
Result pwmDutyCycleCheck = 1;

//Wi-Fi
NifmInternetConnectionType NifmConnectionType = (NifmInternetConnectionType)-1;
NifmInternetConnectionStatus NifmConnectionStatus = (NifmInternetConnectionStatus)-1;
bool Nifm_showpass = false;
Result Nifm_internet_rc = -1;
Result Nifm_profile_rc = -1;
NifmNetworkProfileData_new Nifm_profile = {0};

//Multimedia engines
uint32_t NVDEC_Hz = 0;
uint32_t NVENC_Hz = 0;
uint32_t NVJPG_Hz = 0;

//DSP
uint32_t DSP_Load_u = -1;

//Battery
Service* psmService = 0;
BatteryChargeInfoFields _batteryChargeInfoFields = {0};
float batCurrentAvg = 0;
float batVoltageAvg = 0;
float PowerConsumption = 0;
int16_t batTimeEstimate = -1;
float actualFullBatCapacity = 0;
float designedFullBatCapacity = 0;
bool batteryFiltered = false;
uint8_t batteryTimeLeftRefreshRate = 60;
int32_t BatteryTimeCache[120];

//Temperatures
float SOC_temperatureF = 0;
float PCB_temperatureF = 0;
int32_t skin_temperaturemiliC = 0;

//CPU Usage
//uint64_t idletick0 = systemtickfrequency;
//uint64_t idletick1 = systemtickfrequency;
//uint64_t idletick2 = systemtickfrequency;
//uint64_t idletick3 = systemtickfrequency;

std::atomic<uint64_t> idletick0{systemtickfrequency};
std::atomic<uint64_t> idletick1{systemtickfrequency};
std::atomic<uint64_t> idletick2{systemtickfrequency};
std::atomic<uint64_t> idletick3{systemtickfrequency};


//Frequency
uint32_t CPU_Hz = 0;
uint32_t GPU_Hz = 0;
uint32_t RAM_Hz = 0;

//RAM Size
uint64_t RAM_Total_all_u = 0;
uint64_t RAM_Total_application_u = 0;
uint64_t RAM_Total_applet_u = 0;
uint64_t RAM_Total_system_u = 0;
uint64_t RAM_Total_systemunsafe_u = 0;
uint64_t RAM_Used_all_u = 0;
uint64_t RAM_Used_application_u = 0;
uint64_t RAM_Used_applet_u = 0;
uint64_t RAM_Used_system_u = 0;
uint64_t RAM_Used_systemunsafe_u = 0;

//Fan
double Rotation_Duty = 0;

//GPU Usage
FieldDescriptor fd = 0;
uint32_t GPU_Load_u = 0;
bool GPULoadPerFrame = true;

//NX-FPS

struct resolutionCalls {
    uint16_t width;
    uint16_t height;
    uint16_t calls;
};

struct NxFpsSharedBlock {
    uint32_t MAGIC;
    uint8_t FPS;
    float FPSavg;
    bool pluginActive;
    uint8_t FPSlocked;
    uint8_t FPSmode;
    uint8_t ZeroSync;
    uint8_t patchApplied;
    uint8_t API;
    uint32_t FPSticks[10];
    uint8_t Buffers;
    uint8_t SetBuffers;
    uint8_t ActiveBuffers;
    uint8_t SetActiveBuffers;
    union {
        struct {
            bool handheld: 1;
            bool docked: 1;
            unsigned int reserved: 6;
        } NX_PACKED ds;
        uint8_t general;
    } displaySync;
    resolutionCalls renderCalls[8];
    resolutionCalls viewportCalls[8];
    bool forceOriginalRefreshRate;
    bool dontForce60InDocked;
    bool forceSuspend;
    uint8_t currentRefreshRate;
    float readSpeedPerSecond;
    uint8_t FPSlockedDocked;
    uint64_t frameNumber;
} NX_PACKED;

NxFpsSharedBlock* NxFps = 0;
std::atomic<bool> GameRunning{false};
std::atomic<bool> check{true};
std::atomic<bool> SaltySD{false};
std::atomic<bool> realVoltsPolling{false};
uintptr_t FPSaddress = 0;
uintptr_t FPSavgaddress = 0;
uint64_t PID = 0;
uint32_t FPS = 0xFE;
float FPSmin = 254;
float FPSmax = 0;
float FPSavg = 254;
float FPSavg_old = 254;
bool useOldFPSavg = false;
SharedMemory _sharedmemory = {};
std::atomic<bool> SharedMemoryUsed{false};
Handle remoteSharedMemory = 1;
uint64_t lastFrameNumber = 0;

//Read real freqs from sys-clk sysmodule
uint32_t realCPU_Hz = 0;
uint32_t realGPU_Hz = 0;
uint32_t realRAM_Hz = 0;
uint32_t partLoad[HocClkPartLoad_EnumMax];
uint32_t realCPU_mV = 0;
uint32_t realGPU_mV = 0;
uint32_t realVDD2_mV = 0;
uint32_t realVDDQ_mV = 0;
uint32_t realSOC_mV = 0;
uint8_t refreshRate = 0;

//Read real temps from sys-clk sysmodule
int32_t realCPU_Temp = 0;
int32_t realGPU_Temp = 0;
u32     realPLLX_Temp = 0;
int32_t realRAM_Temp = 0;

int compare (const void* elem1, const void* elem2) {
    if ((((resolutionCalls*)(elem1))->calls) > (((resolutionCalls*)(elem2))->calls)) return -1;
    else return 1;
}

void LoadSharedMemoryAndRefreshRate() {
    if (SaltySD_Connect())
        return;

    SaltySD_GetSharedMemoryHandle(&remoteSharedMemory);
    SaltySD_GetDisplayRefreshRate(&refreshRate);
    SaltySD_Term();

    shmemLoadRemote(&_sharedmemory, remoteSharedMemory, 0x1000, Perm_Rw);
    if (!shmemMap(&_sharedmemory))
        SharedMemoryUsed = true;
    else FPS = 1234;
}

void LoadSharedMemory() {
    if (SaltySD_Connect())
        return;

    SaltySD_GetSharedMemoryHandle(&remoteSharedMemory);
    SaltySD_Term();

    shmemLoadRemote(&_sharedmemory, remoteSharedMemory, 0x1000, Perm_Rw);
    if (!shmemMap(&_sharedmemory))
        SharedMemoryUsed = true;
    else FPS = 1234;
}

void searchSharedMemoryBlock(uintptr_t base) {
    if (!base || !SharedMemoryUsed) {
        NxFps = 0;
        return;
    }

    ptrdiff_t search_offset = 0;
    const uintptr_t memory_end = base + 0x1000;

    while (search_offset < 0x1000) {
        const uintptr_t current_addr = base + search_offset;

        // Ensure we don't read past the end of shared memory
        if (current_addr + sizeof(NxFpsSharedBlock) > memory_end) {
            break;
        }

        NxFps = (NxFpsSharedBlock*)current_addr;

        // Add bounds checking and magic validation
        if (NxFps && current_addr >= base && NxFps->MAGIC == 0x465053) {
            return;
        }
        search_offset += 4;
    }
    NxFps = 0;
}

//Check if SaltyNX is working
bool CheckPort() {
    Handle saltysd;

    // Try up to 67 times with exponential backoff for better responsiveness
    for (int i = 0; i < 50; i++) {
        if (R_SUCCEEDED(svcConnectToNamedPort(&saltysd, "InjectServ"))) {
            svcCloseHandle(saltysd);
            return true;
        }

        // Progressive sleep - start fast, then slow down
        //if (i < 10) {
        //    svcSleepThread(100'000);    // 0.1ms for first 10 attempts
        //} else if (i < 30) {
        //    svcSleepThread(500'000);    // 0.5ms for next 20 attempts
        //} else {
        //    svcSleepThread(1'000'000);  // 1ms for remaining attempts
        //}
    }

    return false;
}

Mutex mutex_Misc = {0};

void CheckIfGameRunning(void*) {
    do {
        mutexLock(&mutex_Misc);
        if (!check && R_FAILED(pmdmntGetApplicationProcessId(&PID))) {
            GameRunning = false;
            check = true;
        }
        else if (!GameRunning && SharedMemoryUsed) {
            const uintptr_t base = (uintptr_t)shmemGetAddr(&_sharedmemory);
            searchSharedMemoryBlock(base);
            if (NxFps) {
                (NxFps->pluginActive) = false;
                mutexUnlock(&mutex_Misc);  // ← Fix: Unlock before return
                if (leventWait(&threadexit, 100'000'000)) {
                    return;
                }
                mutexLock(&mutex_Misc);
                if ((NxFps->pluginActive)) {
                    GameRunning = true;
                    check = false;
                }
            }
        }
        mutexUnlock(&mutex_Misc);
    } while (!leventWait(&threadexit, 1'000'000'000));
}

// Utils.hpp or your relevant header
static constexpr size_t CACHE_ELEMENTS = sizeof(BatteryTimeCache) / sizeof(BatteryTimeCache[0]);

Mutex mutex_BatteryChecker = {0};
void BatteryChecker(void*) {
    if (R_FAILED(psmCheck) || R_FAILED(i2cCheck)){
        return;
    }
    uint16_t data = 0;
    float tempV = 0.0;
    float tempA = 0.0;
    size_t ArraySize = 10;
    if (batteryFiltered) {
        ArraySize = 1;
    }
    float* readingsAmp = new float[ArraySize];
    float* readingsVolt = new float[ArraySize];

    Max17050ReadReg(MAX17050_AvgCurrent, &data);
    tempA = (1.5625 / (max17050SenseResistor * max17050CGain)) * (s16)data;
    for (size_t i = 0; i < ArraySize; i++) {
        readingsAmp[i] = tempA;
    }
    Max17050ReadReg(MAX17050_AvgVCELL, &data);
    tempV = 0.625 * (data >> 3);
    for (size_t i = 0; i < ArraySize; i++) {
        readingsVolt[i] = tempV;
    }
    if (!actualFullBatCapacity) {
        Max17050ReadReg(MAX17050_FullCAP, &data);
        actualFullBatCapacity = data * (BASE_SNS_UOHM / MAX17050_BOARD_SNS_RESISTOR_UOHM) / MAX17050_BOARD_CGAIN;
    }
    if (!designedFullBatCapacity) {
        Max17050ReadReg(MAX17050_DesignCap, &data);
        designedFullBatCapacity = data * (BASE_SNS_UOHM / MAX17050_BOARD_SNS_RESISTOR_UOHM) / MAX17050_BOARD_CGAIN;
    }
    if (readingsAmp[0] >= 0) {
        batTimeEstimate = -1;
    }
    else {
        Max17050ReadReg(MAX17050_TTE, &data);
        float batteryTimeEstimateInMinutes = (5.625 * data) / 60;
        if (batteryTimeEstimateInMinutes > (99.0*60.0)+59.0) {
            batTimeEstimate = (99*60)+59;
        }
        else batTimeEstimate = (int16_t)batteryTimeEstimateInMinutes;
    }

    size_t counter = 0;
    uint64_t tick_TTE = svcGetSystemTick();
    uint64_t nanoseconds = 1000;
    do {
        mutexLock(&mutex_BatteryChecker);
        const uint64_t startTick = svcGetSystemTick();

        psmGetBatteryChargeInfoFields(psmService, &_batteryChargeInfoFields);

        // Calculation is based on Hekate's max17050.c
        // Source: https://github.com/CTCaer/hekate/blob/master/bdk/power/max17050.c

        if (!batteryFiltered) {
            Max17050ReadReg(MAX17050_Current, &data);
            tempA = (1.5625 / (max17050SenseResistor * max17050CGain)) * (s16)data;
            Max17050ReadReg(MAX17050_VCELL, &data);
            tempV = 0.625 * (data >> 3);
        } else {
            Max17050ReadReg(MAX17050_AvgCurrent, &data);
            tempA = (1.5625 / (max17050SenseResistor * max17050CGain)) * (s16)data;
            Max17050ReadReg(MAX17050_AvgVCELL, &data);
            tempV = 0.625 * (data >> 3);
        }

        if (tempA && tempV) {
            readingsAmp[counter % ArraySize] = tempA;
            readingsVolt[counter % ArraySize] = tempV;
            counter++;
        }

        float batCurrent = 0.0;
        float batVoltage = 0.0;
        float batPowerAvg = 0.0;
        for (size_t x = 0; x < ArraySize; x++) {
            batCurrent += readingsAmp[x];
            batVoltage += readingsVolt[x];
            batPowerAvg += (readingsAmp[x] * readingsVolt[x]) / 1'000;
        }
        batCurrent /= ArraySize;
        batVoltage /= ArraySize;
        batCurrentAvg = batCurrent;
        batVoltageAvg = batVoltage;
        batPowerAvg /= ArraySize * 1000;
        PowerConsumption = batPowerAvg;

        if (batCurrentAvg >= 0) {
            batTimeEstimate = -1;
        }
        else {
            static float batteryTimeEstimateInMinutes = 0;
            Max17050ReadReg(MAX17050_TTE, &data);
            batteryTimeEstimateInMinutes = (5.625 * data) / 60;
            if (batteryTimeEstimateInMinutes > (99.0*60.0)+59.0) {
                batteryTimeEstimateInMinutes = (99.0*60.0)+59.0;
            }
            static int itr = 0;
            const int cacheElements = (sizeof(BatteryTimeCache) / sizeof(BatteryTimeCache[0]));
            BatteryTimeCache[itr++ % cacheElements] = (int32_t)batteryTimeEstimateInMinutes;
            const uint64_t new_tick_TTE = svcGetSystemTick();
            if (armTicksToNs(new_tick_TTE - tick_TTE) / 1'000'000'000 >= batteryTimeLeftRefreshRate) {
                const size_t to_divide = itr < cacheElements ? itr : cacheElements;
                batTimeEstimate = (int16_t)(std::accumulate(&BatteryTimeCache[0], &BatteryTimeCache[to_divide], 0) / to_divide);
                tick_TTE = new_tick_TTE;
            }
        }

        mutexUnlock(&mutex_BatteryChecker);
        nanoseconds = armTicksToNs(svcGetSystemTick() - startTick);
        if (nanoseconds < 1'000'000'000 / 2) {
            nanoseconds = (1'000'000'000 / 2) - nanoseconds;
        } else {
            nanoseconds = 1000;
        }
    } while(!leventWait(&threadexit, nanoseconds));

    batTimeEstimate = -1;
    _batteryChargeInfoFields = {0};
    memset(BatteryTimeCache, 0, sizeof(BatteryTimeCache));
    delete[] readingsAmp;
    delete[] readingsVolt;
}


void StartBatteryThread() {
    //if (!skip) {
    //    threadWaitForExit(&t7);
    //    threadClose(&t7);
    //    leventClear(&threadexit);
    //}
    leventClear(&threadexit);
    threadCreate(&t7, BatteryChecker, NULL, NULL, 0x2000, 0x3F, 3);
    threadStart(&t7);
}

void CloseBatteryThread() {
    leventSignal(&threadexit);
    threadWaitForExit(&t7);
    threadClose(&t7);
}



void gpuLoadThread(void*) {
    #define gpu_samples_average 8
    uint32_t gpu_load_array[gpu_samples_average] = {0};
    size_t i = 0;
    if (!GPULoadPerFrame && R_SUCCEEDED(nvCheck)) do {
        u32 temp;
        if (R_SUCCEEDED(nvIoctl(fd, NVGPU_GPU_IOCTL_PMU_GET_GPU_LOAD, &temp))) {
            gpu_load_array[i++ % gpu_samples_average] = temp;
            GPU_Load_u = std::accumulate(&gpu_load_array[0], &gpu_load_array[gpu_samples_average], 0) / gpu_samples_average;
        }
    } while(!leventWait(&threadexit, 16'666'000));
}

std::string getVersionString() {
    char buf[0x100] = "";  // 256 bytes — safe for any expected version string
    Result rc = hocclkIpcGetVersionString(buf, sizeof(buf));
    if (R_FAILED(rc) || buf[0] == '\0') {
        return "unknown";
    }
    return std::string(buf);
}


bool usingEOS() {
    const std::string versionString = getVersionString();
    return versionString.find("eos") != std::string::npos;
}

// === ULTRA-FAST VOLTAGE READING ===
static constexpr PowerDomainId domains[] = {
    PcvPowerDomainId_Max77621_Cpu,    // [0] CPU
    PcvPowerDomainId_Max77621_Gpu,    // [1] GPU
    PcvPowerDomainId_Max77812_Dram,   // [2] VDD2 (EMC/DRAM)
    PcvPowerDomainId_Max77620_Sd0,    // [3] SOC
    PcvPowerDomainId_Max77620_Sd1     // [4] VDDQ
};


// Stuff that doesn't need multithreading
void Misc(void*) {
    const uint64_t timeout_ns = TeslaFPS < 10 ? (1'000'000'000 / TeslaFPS) : 100'000'000;
    const bool isUsingEOS = usingEOS();

    // Initialize voltage reading if needed
    bool canReadVoltages = false;
    if (!isUsingEOS && realVoltsPolling) {
        canReadVoltages = R_SUCCEEDED(rgltrInitialize());
        if (!canReadVoltages) {
            realVoltsPolling = false;
        }
    }

    do {
        mutexLock(&mutex_Misc);

        // Get sys-clk data
        if (R_SUCCEEDED(hocclkCheck)) {
            HocClkContext hocclkCTX;
            if (R_SUCCEEDED(hocclkIpcGetCurrentContext(&hocclkCTX))) {
                realCPU_Hz = hocclkCTX.realFreqs[HocClkModule_CPU];
                realGPU_Hz = hocclkCTX.realFreqs[HocClkModule_GPU];
                realRAM_Hz = hocclkCTX.realFreqs[HocClkModule_MEM];
                partLoad[HocClkPartLoad_EMC]       = hocclkCTX.partLoad[HocClkPartLoad_EMC];
                partLoad[HocClkPartLoad_EMCCpu]    = hocclkCTX.partLoad[HocClkPartLoad_EMCCpu];
                partLoad[HocClkPartLoad_RamBWAll]  = hocclkCTX.partLoad[HocClkPartLoad_RamBWAll];
                partLoad[HocClkPartLoad_RamBWCpu]  = hocclkCTX.partLoad[HocClkPartLoad_RamBWCpu];
                partLoad[HocClkPartLoad_RamBWGpu]  = hocclkCTX.partLoad[HocClkPartLoad_RamBWGpu];
                partLoad[HocClkPartLoad_RamBWPeak] = hocclkCTX.partLoad[HocClkPartLoad_RamBWPeak];
				realCPU_Temp = hocclkCTX.temps[HocClkThermalSensor_CPU];
				realGPU_Temp = hocclkCTX.temps[HocClkThermalSensor_GPU];
				realPLLX_Temp = hocclkCTX.temps[HocClkThermalSensor_PLLX];
				realRAM_Temp = hocclkCTX.temps[HocClkThermalSensor_MEM];

                realCPU_mV = hocclkCTX.voltages[HocClkVoltage_CPU];
                realGPU_mV = hocclkCTX.voltages[HocClkVoltage_GPU];
                realSOC_mV = hocclkCTX.voltages[HocClkVoltage_SOC];
                realVDD2_mV = hocclkCTX.voltages[HocClkVoltage_EMCVDD2];
                realVDDQ_mV = hocclkCTX.voltages[HocClkVoltage_EMCVDDQ];
                CPU_Hz = hocclkCTX.freqs[HocClkModule_CPU];
                GPU_Hz = hocclkCTX.freqs[HocClkModule_GPU];
                RAM_Hz = hocclkCTX.freqs[HocClkModule_MEM];
            }
        }


        // Temperatures
        if (R_SUCCEEDED(i2cCheck)) {
            Tmp451GetSocTemp(&SOC_temperatureF);
            Tmp451GetPcbTemp(&PCB_temperatureF);
        }
        if (R_SUCCEEDED(tcCheck)) {
            tcGetSkinTemperatureMilliC(&skin_temperaturemiliC);
        }

        // RAM Memory Used
        if (R_SUCCEEDED(Hinted)) {
            svcGetSystemInfo(&RAM_Total_application_u, 0, INVALID_HANDLE, 0);
            svcGetSystemInfo(&RAM_Total_applet_u, 0, INVALID_HANDLE, 1);
            svcGetSystemInfo(&RAM_Total_system_u, 0, INVALID_HANDLE, 2);
            svcGetSystemInfo(&RAM_Total_systemunsafe_u, 0, INVALID_HANDLE, 3);
            svcGetSystemInfo(&RAM_Used_application_u, 1, INVALID_HANDLE, 0);
            svcGetSystemInfo(&RAM_Used_applet_u, 1, INVALID_HANDLE, 1);
            svcGetSystemInfo(&RAM_Used_system_u, 1, INVALID_HANDLE, 2);
            svcGetSystemInfo(&RAM_Used_systemunsafe_u, 1, INVALID_HANDLE, 3);
        }

        // Fan
        if (R_SUCCEEDED(pwmCheck)) {
            double temp = 0;
            if (R_SUCCEEDED(pwmChannelSessionGetDutyCycle(&g_ICon, &temp))) {
                temp *= 10;
                temp = trunc(temp);
                temp /= 10;
                Rotation_Duty = 100.0 - temp;
                if (Rotation_Duty <= 0) {
                    Rotation_Duty = 0.0000001;
                }
            }
        }

        // GPU Load
        if (R_SUCCEEDED(nvCheck) && GPULoadPerFrame) {
            nvIoctl(fd, NVGPU_GPU_IOCTL_PMU_GET_GPU_LOAD, &GPU_Load_u);
        }

        // FPS - with proper null checks
        if (GameRunning) {
            if (NxFps && SharedMemoryUsed) {
                FPS = NxFps->FPS;
                const size_t element_count = sizeof(NxFps->FPSticks) / sizeof(NxFps->FPSticks[0]);
                FPSavg_old = static_cast<float>(systemtickfrequency) /
                            (std::accumulate(&NxFps->FPSticks[0], &NxFps->FPSticks[element_count], 0.0f) / element_count);

                const float FPS_in = static_cast<float>(FPS);
                if (FPSavg_old >= (FPS_in - 0.25f) && FPSavg_old <= (FPS_in + 0.25f)) {
                    FPSavg = FPS_in;
                } else {
                    FPSavg = FPSavg_old;
                }

                lastFrameNumber = NxFps->frameNumber;

                if (FPSavg > FPSmax) FPSmax = FPSavg;
                if (FPSavg < FPSmin) FPSmin = FPSavg;
            }
        } else {
            FPSavg = 254;
            FPSmin = 254;
            FPSmax = 0;
        }

        mutexUnlock(&mutex_Misc);

    } while (!leventWait(&threadexit, timeout_ns));

    // Cleanup voltage reading if initialized
    if (canReadVoltages) {
        rgltrExit();
    }
}

void Misc2(void*) {
    u32 dummy = 0;
    do {
        //DSP
        if (R_SUCCEEDED(audsnoopCheck)) audsnoopGetDspUsage(&DSP_Load_u);

        //Multimedia clock rates
        if (R_SUCCEEDED(nvdecCheck)) mmuRequestGet(&nvdecRequest, &NVDEC_Hz);
        if (R_SUCCEEDED(nvencCheck)) mmuRequestGet(&nvencRequest, &NVENC_Hz);
        if (R_SUCCEEDED(nvjpgCheck)) mmuRequestGet(&nvjpgRequest, &NVJPG_Hz);

        if (R_SUCCEEDED(nifmCheck)) {
            //u32 dummy = 0;
            Nifm_internet_rc = nifmGetInternetConnectionStatus(&NifmConnectionType, &dummy, &NifmConnectionStatus);
            if (!Nifm_internet_rc && (NifmConnectionType == NifmInternetConnectionType_WiFi))
                Nifm_profile_rc = nifmGetCurrentNetworkProfile((NifmNetworkProfileData*)&Nifm_profile);
        }
    } while (!leventWait(&threadexit, 100'000'000));
}

void Misc3(void*) {
    const bool isUsingEOS = usingEOS();

    // Initialize voltage reading if needed
    bool canReadVoltages = false;
    if (!isUsingEOS && realVoltsPolling) {
        canReadVoltages = R_SUCCEEDED(rgltrInitialize());
        if (!canReadVoltages) {
            realVoltsPolling = false;
        }
    }

    do {
        mutexLock(&mutex_Misc);

        // Get sys-clk data
        if (R_SUCCEEDED(hocclkCheck)) {
            HocClkContext hocclkCTX;
            if (R_SUCCEEDED(hocclkIpcGetCurrentContext(&hocclkCTX))) {
                partLoad[HocClkPartLoad_EMC]       = hocclkCTX.partLoad[HocClkPartLoad_EMC];
                partLoad[HocClkPartLoad_EMCCpu]    = hocclkCTX.partLoad[HocClkPartLoad_EMCCpu];
                partLoad[HocClkPartLoad_RamBWAll]  = hocclkCTX.partLoad[HocClkPartLoad_RamBWAll];
                partLoad[HocClkPartLoad_RamBWCpu]  = hocclkCTX.partLoad[HocClkPartLoad_RamBWCpu];
                partLoad[HocClkPartLoad_RamBWGpu]  = hocclkCTX.partLoad[HocClkPartLoad_RamBWGpu];
                partLoad[HocClkPartLoad_RamBWPeak] = hocclkCTX.partLoad[HocClkPartLoad_RamBWPeak];

				realCPU_Temp = hocclkCTX.temps[HocClkThermalSensor_CPU];
				realGPU_Temp = hocclkCTX.temps[HocClkThermalSensor_GPU];
				realPLLX_Temp = hocclkCTX.temps[HocClkThermalSensor_PLLX];
				realRAM_Temp = hocclkCTX.temps[HocClkThermalSensor_MEM];

                realCPU_mV = hocclkCTX.voltages[HocClkVoltage_CPU];
                realGPU_mV = hocclkCTX.voltages[HocClkVoltage_GPU];
                realSOC_mV = hocclkCTX.voltages[HocClkVoltage_SOC];
                realVDD2_mV = hocclkCTX.voltages[HocClkVoltage_EMCVDD2];
                realVDDQ_mV = hocclkCTX.voltages[HocClkVoltage_EMCVDDQ];

            }
        }


        // Temperatures
        if (R_SUCCEEDED(i2cCheck)) {
            Tmp451GetSocTemp(&SOC_temperatureF);
            Tmp451GetPcbTemp(&PCB_temperatureF);
        }
        if (R_SUCCEEDED(tcCheck)) {
            tcGetSkinTemperatureMilliC(&skin_temperaturemiliC);
        }

        // Fan
        if (R_SUCCEEDED(pwmCheck)) {
            double temp = 0;
            if (R_SUCCEEDED(pwmChannelSessionGetDutyCycle(&g_ICon, &temp))) {
                temp *= 10;
                temp = trunc(temp);
                temp /= 10;
                Rotation_Duty = 100.0 - temp;
                if (Rotation_Duty <= 0) {
                    Rotation_Duty = 0.0000001;
                }
            }
        }

        // GPU Load
        if (R_SUCCEEDED(nvCheck)) {
            nvIoctl(fd, NVGPU_GPU_IOCTL_PMU_GET_GPU_LOAD, &GPU_Load_u);
        }

        mutexUnlock(&mutex_Misc);

    } while (!leventWait(&threadexit, 1'000'000'000)); // 1 second timeout

    // Cleanup voltage reading if initialized
    if (canReadVoltages) {
        rgltrExit();
    }
}

//Check each core for idled ticks in intervals, they cannot read info about other core than they are assigned
//In case of getting more than systemtickfrequency in idle, make it equal to systemtickfrequency to get 0% as output and nothing less
//This is because making each loop also takes time, which is not considered because this will take also additional time

//Check each core for idled ticks in intervals, they cannot read info about other core than they are assigned
//In case of getting more than systemtickfrequency in idle, make it equal to systemtickfrequency to get 0% as output and nothing less
//This is because making each loop also takes time, which is not considered because this will take also additional time
//void CheckCore0(void*) {
//    uint64_t timeout_ns = 1'000'000'000 / TeslaFPS;
//    while(true) {
//        uint64_t idletick_a0 = 0;
//        uint64_t idletick_b0 = 0;
//        svcGetInfo(&idletick_b0, InfoType_IdleTickCount, INVALID_HANDLE, 0);
//        if (leventWait(&threadexit, timeout_ns))
//            return;
//        svcGetInfo(&idletick_a0, InfoType_IdleTickCount, INVALID_HANDLE, 0);
//        idletick0 = idletick_a0 - idletick_b0;
//    }
//}
//
//void CheckCore1(void*) {
//    uint64_t timeout_ns = 1'000'000'000 / TeslaFPS;
//    while(true) {
//        uint64_t idletick_a1 = 0;
//        uint64_t idletick_b1 = 0;
//        svcGetInfo(&idletick_b1, InfoType_IdleTickCount, INVALID_HANDLE, 1);
//        if (leventWait(&threadexit, timeout_ns))
//            return;
//        svcGetInfo(&idletick_a1, InfoType_IdleTickCount, INVALID_HANDLE, 1);
//        idletick1 = idletick_a1 - idletick_b1;
//    }
//}
//
//void CheckCore2(void*) {
//    uint64_t timeout_ns = 1'000'000'000 / TeslaFPS;
//    while(true) {
//        uint64_t idletick_a2 = 0;
//        uint64_t idletick_b2 = 0;
//        svcGetInfo(&idletick_b2, InfoType_IdleTickCount, INVALID_HANDLE, 2);
//        if (leventWait(&threadexit, timeout_ns))
//            return;
//        svcGetInfo(&idletick_a2, InfoType_IdleTickCount, INVALID_HANDLE, 2);
//        idletick2 = idletick_a2 - idletick_b2;
//    }
//}
//
//void CheckCore3(void*) {
//    uint64_t timeout_ns = 1'000'000'000 / TeslaFPS;
//    while(true) {
//        uint64_t idletick_a3 = 0;
//        uint64_t idletick_b3 = 0;
//        svcGetInfo(&idletick_b3, InfoType_IdleTickCount, INVALID_HANDLE, 3);
//        if (leventWait(&threadexit, timeout_ns))
//            return;
//        svcGetInfo(&idletick_a3, InfoType_IdleTickCount, INVALID_HANDLE, 3);
//        idletick3 = idletick_a3 - idletick_b3;
//    }
//}

void CheckCore(void* idletick_ptr) {
    const uint64_t timeout_ns = 1'000'000'000ULL / TeslaFPS;
    std::atomic<uint64_t>* idletick = (std::atomic<uint64_t>*)idletick_ptr;
    while (true) {
        uint64_t idletick_a;
        uint64_t idletick_b;
        svcGetInfo(&idletick_b, InfoType_IdleTickCount, INVALID_HANDLE, -1);
        Result rc_break = leventWait(&threadexit, timeout_ns);
        svcGetInfo(&idletick_a, InfoType_IdleTickCount, INVALID_HANDLE, -1);
        if (rc_break) return;
        idletick->store(idletick_a - idletick_b, std::memory_order_release);
    }
}


//Start reading all stats
void StartThreads() {
    // Clear the thread exit event for new threads
    leventClear(&threadexit);

    threadCreate(&t0, CheckCore, &idletick0, NULL, 0x1000, 0x10, 0);
    threadCreate(&t1, CheckCore, &idletick1, NULL, 0x1000, 0x10, 1);
    threadCreate(&t2, CheckCore, &idletick2, NULL, 0x1000, 0x10, 2);
    threadCreate(&t3, CheckCore, &idletick3, NULL, 0x1000, 0x10, 3);

    //threadCreate(&t0, CheckCore, &coreIds[0], NULL, 0x1000, 0x10, 0);
    //threadCreate(&t1, CheckCore, &coreIds[1], NULL, 0x1000, 0x10, 1);
    //threadCreate(&t2, CheckCore, &coreIds[2], NULL, 0x1000, 0x10, 2);
    //threadCreate(&t3, CheckCore, &coreIds[3], NULL, 0x1000, 0x10, 3);
    threadCreate(&t4, Misc, NULL, NULL, 0x4000, 0x3F, -2);
    threadCreate(&t5, gpuLoadThread, NULL, NULL, 0x1000, 0x3F, -2);
    threadCreate(&t7, BatteryChecker, NULL, NULL, 0x4000, 0x3F, -2);

    threadStart(&t0);
    threadStart(&t1);
    threadStart(&t2);
    threadStart(&t3);
    threadStart(&t4);
    threadStart(&t5);
    threadStart(&t7);

    if (SaltySD) {
        //Assign NX-FPS to default core
        threadCreate(&t6, CheckIfGameRunning, NULL, NULL, 0x1000, 0x38, -2);
        threadStart(&t6);
    }
}

//End reading all stats
void CloseThreads() {
    leventSignal(&threadexit);
    threadWaitForExit(&t0);
    threadWaitForExit(&t1);
    threadWaitForExit(&t2);
    threadWaitForExit(&t3);
    threadWaitForExit(&t4);
    threadWaitForExit(&t5);
    threadWaitForExit(&t6);
    threadWaitForExit(&t7);
    threadClose(&t0);
    threadClose(&t1);
    threadClose(&t2);
    threadClose(&t3);
    threadClose(&t4);
    threadClose(&t5);
    threadClose(&t6);
    threadClose(&t7);
}

//Separate functions dedicated to "FPS Counter" mode
void FPSCounter(void*) {
    const uint64_t timeout_ns = 1'000'000'000 / TeslaFPS;
    do {
        if (GameRunning) {
            if (SharedMemoryUsed && NxFps) {
                FPS = (NxFps -> FPS);
                const size_t element_count = sizeof(NxFps -> FPSticks) / sizeof(NxFps -> FPSticks[0]);
                FPSavg_old = (float)systemtickfrequency / (std::accumulate<uint32_t*, float>(&NxFps->FPSticks[0], &NxFps->FPSticks[element_count], 0) / element_count);
                const float FPS_in = (float)FPS;
                if (FPSavg_old >= (FPS_in-0.25) && FPSavg_old <= (FPS_in+0.25))
                    FPSavg = FPS_in;
                else FPSavg = FPSavg_old;
                lastFrameNumber = NxFps -> frameNumber;
            }
        }
        else FPSavg = 254;
    } while (!leventWait(&threadexit, timeout_ns));
}

void StartFPSCounterThread() {
    leventClear(&threadexit);

    threadCreate(&t6, CheckIfGameRunning, NULL, NULL, 0x1000, 0x38, -2);
    threadStart(&t6);

    threadCreate(&t4, FPSCounter, NULL, NULL, 0x1000, 0x3F, 3);
    threadStart(&t4);
}

void EndFPSCounterThread() {
    leventSignal(&threadexit);
    threadWaitForExit(&t6);
    threadWaitForExit(&t4);
    threadClose(&t6);
    threadClose(&t4);
}

void StartInfoThread() {
    // Clear the thread exit event for new threads
    leventClear(&threadexit);

    threadCreate(&t0, CheckCore, &idletick0, NULL, 0x1000, 0x10, 0);
    threadCreate(&t1, CheckCore, &idletick1, NULL, 0x1000, 0x10, 1);
    threadCreate(&t2, CheckCore, &idletick2, NULL, 0x1000, 0x10, 2);
    threadCreate(&t3, CheckCore, &idletick3, NULL, 0x1000, 0x10, 3);

    //threadCreate(&t1, CheckCore, &coreIds[0], NULL, 0x1000, 0x10, 0);
    //threadCreate(&t2, CheckCore, &coreIds[1], NULL, 0x1000, 0x10, 1);
    //threadCreate(&t3, CheckCore, &coreIds[2], NULL, 0x1000, 0x10, 2);
    //threadCreate(&t4, CheckCore, &coreIds[3], NULL, 0x1000, 0x10, 3);
    threadCreate(&t7, Misc3, NULL, NULL, 0x1000, 0x3F, -2);

    threadStart(&t0);
    threadStart(&t1);
    threadStart(&t2);
    threadStart(&t3);
    threadStart(&t7);
}

void EndInfoThread() {
    // Signal the thread exit event
    leventSignal(&threadexit);

    // Wait for all threads to exit
    threadWaitForExit(&t0);
    threadWaitForExit(&t1);
    threadWaitForExit(&t2);
    threadWaitForExit(&t3);
    threadWaitForExit(&t7);

    // Close thread handles
    threadClose(&t0);
    threadClose(&t1);
    threadClose(&t2);
    threadClose(&t3);
    threadClose(&t7);
}

// String formatting functions
void removeSpaces(std::string& str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
}

void convertToUpper(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::toupper);
}

void convertToLower(std::string& str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

std::map<std::string, std::string> replaces{
    {"A", "\uE0E0"},
    {"B", "\uE0E1"},
    {"X", "\uE0E2"},
    {"Y", "\uE0E3"},
    {"L", "\uE0E4"},
    {"R", "\uE0E5"},
    {"ZL", "\uE0E6"},
    {"ZR", "\uE0E7"},
    {"SL", "\uE0E8"},
    {"SR", "\uE0E9"},
    {"DUP", "\uE0EB"},
    {"DDOWN", "\uE0EC"},
    {"DLEFT", "\uE0ED"},
    {"DRIGHT", "\uE0EE"},
    {"PLUS", "\uE0EF"},
    {"MINUS", "\uE0F0"},
    {"LSTICK", "\uE104"},
    {"RSTICK", "\uE105"},
    {"RS", "\uE105"},
    {"LS", "\uE104"}
};

void formatButtonCombination(std::string& line) {
    // Remove all spaces from the line
    line.erase(std::remove(line.begin(), line.end(), ' '), line.end());

    // Replace '+' with ' + '
    size_t pos = 0;
    size_t max_pluses = 3;
    while ((pos = line.find('+', pos)) != std::string::npos) {
        if (!max_pluses) {
            line = line.substr(0, pos);
            return;
        }
        if (pos > 0 && pos < line.size() - 1) {
            if (std::isalnum(line[pos - 1]) && std::isalnum(line[pos + 1])) {
                line.replace(pos, 1, " + ");
                pos += 3;
            }
        }
        ++pos;
        max_pluses--;
    }
    pos = 0;
    size_t old_pos = 0;

    static std::string button;
    while ((pos = line.find(" + ", pos)) != std::string::npos) {

        button = line.substr(old_pos, pos - old_pos);
        if (replaces.find(button) != replaces.end()) {
            line.replace(old_pos, button.length(), replaces[button]);
            pos = 0;
            old_pos = 0;
        }
        else pos += 3;
        old_pos = pos;
    }
    button = line.substr(old_pos);
    if (replaces.find(button) != replaces.end()) {
        line.replace(old_pos, button.length(), replaces[button]);
    }
}

//uint64_t comboBitmask = 0;
//
//constexpr uint64_t MapButtons(const std::string& buttonCombo) {
//    static std::map<std::string, uint64_t> buttonMap = {
//        {"A", HidNpadButton_A},
//        {"B", HidNpadButton_B},
//        {"X", HidNpadButton_X},
//        {"Y", HidNpadButton_Y},
//        {"L", HidNpadButton_L},
//        {"R", HidNpadButton_R},
//        {"ZL", HidNpadButton_ZL},
//        {"ZR", HidNpadButton_ZR},
//        {"PLUS", HidNpadButton_Plus},
//        {"MINUS", HidNpadButton_Minus},
//        {"DUP", HidNpadButton_Up},
//        {"DDOWN", HidNpadButton_Down},
//        {"DLEFT", HidNpadButton_Left},
//        {"DRIGHT", HidNpadButton_Right},
//        {"SL", HidNpadButton_AnySL},
//        {"SR", HidNpadButton_AnySR},
//        {"LSTICK", HidNpadButton_StickL},
//        {"RSTICK", HidNpadButton_StickR},
//        {"LS", HidNpadButton_StickL},
//        {"RS", HidNpadButton_StickR},
//        {"UP", HidNpadButton_AnyUp},
//        {"DOWN", HidNpadButton_AnyDown},
//        {"LEFT", HidNpadButton_AnyLeft},
//        {"RIGHT", HidNpadButton_AnyRight}
//    };
//
//
//    std::string comboCopy = buttonCombo;  // Make a copy of buttonCombo
//
//    static const std::string delimiter = "+";
//    size_t pos = 0;
//    static std::string button;
//    size_t max_delimiters = 4;
//    while ((pos = comboCopy.find(delimiter)) != std::string::npos) {
//        button = comboCopy.substr(0, pos);
//        if (buttonMap.find(button) != buttonMap.end()) {
//            comboBitmask |= buttonMap[button];
//        }
//        comboCopy.erase(0, pos + delimiter.length());
//        if (!--max_delimiters) {
//            return comboBitmask;
//        }
//    }
//    if (buttonMap.find(comboCopy) != buttonMap.end()) {
//        comboBitmask |= buttonMap[comboCopy];
//    }
//    return comboBitmask;
//}

ALWAYS_INLINE bool isKeyComboPressed(uint64_t keysHeld, uint64_t keysDown) {
    // Check if any of the combo buttons are pressed down this frame
    // while the rest of the combo buttons are being held

    const uint64_t comboButtonsDown = keysDown & tsl::cfg::launchCombo;
    const uint64_t comboButtonsHeld = keysHeld & tsl::cfg::launchCombo;

    // If any combo buttons are pressed down this frame
    if (comboButtonsDown != 0) {
        // Check if the remaining combo buttons are being held
        // (the full combo should be active when combining held + down)
        const uint64_t totalComboActive = comboButtonsHeld | comboButtonsDown;

        if (totalComboActive == tsl::cfg::launchCombo) {
            fixHiding = true; // for fixing hiding when returning
            //triggerRumbleDoubleClick.store(true, std::memory_order_release);
            //triggerExitSound.store(true, std::memory_order_release);
            return true;
        }
    }

    return false;
}

inline int safeFanDuty(int raw) {
    if (raw < 0)   return 0;
    if (raw > 100) return 100;
    return raw;
}

// Helper function to check if comboBitmask is satisfied with at least one key in keysDown and the rest in keysHeld
bool isKeyComboPressed2(uint64_t keysDown, uint64_t keysHeld) {
    uint64_t requiredKeys = tsl::cfg::launchCombo;
    bool hasKeyDown = false; // Tracks if at least one key is in keysDown

    static uint64_t keyBit;
    // Iterate over each bit in the comboBitmask
    while (requiredKeys) {
        keyBit = requiredKeys & ~(requiredKeys - 1); // Get the lowest bit set in requiredKeys

        // Check if the key is in keysDown or keysHeld
        if (keysDown & keyBit) {
            hasKeyDown = true; // Found at least one key in keysDown
        } else if (!(keysHeld & keyBit)) {
            return false; // If the key is neither in keysDown nor keysHeld, the combo is incomplete
        }

        // Remove the lowest bit and continue to check other keys
        requiredKeys &= ~keyBit;
    }

    // Ensure that at least one key was in keysDown and the rest were in keysHeld
    return hasKeyDown;
}


// Custom utility function for parsing an ini file
void ParseIniFile() {
    // Check and create directory if needed
    //struct stat st;
    //if (stat(directoryPath, &st) != 0) {
    //    mkdir(directoryPath, 0777);
    //}
    ult::createSingleDirectory(directoryPath);

    // Load main config INI once
    auto configData = ult::getParsedDataFromIniFile(configIniPath);
    auto statusIt = configData.find("status-monitor");

    if (statusIt != configData.end()) {
        const auto& statusSection = statusIt->second;
        std::string key;

        // Process all settings with direct lookups
        auto batteryFilterIt = statusSection.find("battery_avg_iir_filter");
        if (batteryFilterIt != statusSection.end()) {
            key = batteryFilterIt->second;
            convertToUpper(key);
            batteryFiltered = (key == "TRUE");
        }

        auto refreshRateIt = statusSection.find("battery_time_left_refreshrate");
        if (refreshRateIt != statusSection.end()) {
            batteryTimeLeftRefreshRate = std::clamp(atol(refreshRateIt->second.c_str()), 1L, 60L);
        }

        auto gpuLoadIt = statusSection.find("average_gpu_load");
        if (gpuLoadIt != statusSection.end()) {
            key = gpuLoadIt->second;
            convertToUpper(key);
            GPULoadPerFrame = (key != "TRUE");
        }

        auto fpsAvgIt = statusSection.find("use_old_fps_average");
        if (fpsAvgIt != statusSection.end()) {
            key = fpsAvgIt->second;
            convertToUpper(key);
            useOldFPSavg = (key == "TRUE");
        }
    }

    // Handle external combo - load each file once
    const struct { const char* path; const char* section; } externalConfigs[] = {
        {ultrahandConfigIniPath, "ultrahand"},
        {teslaConfigIniPath, "tesla"}
    };

    for (const auto& config : externalConfigs) {
        auto extConfigData = ult::getParsedDataFromIniFile(config.path);
        auto sectionIt = extConfigData.find(config.section);

        if (sectionIt != extConfigData.end()) {
            auto keyComboIt = sectionIt->second.find("key_combo");
            if (keyComboIt != sectionIt->second.end() && !keyComboIt->second.empty()) {
                keyCombo = keyComboIt->second;
                removeSpaces(keyCombo);
                convertToUpper(keyCombo);
                break;
            }
        }
    }

    //comboBitmask = MapButtons(keyCombo);
}

ALWAYS_INLINE bool isValidRGBA4Color(const std::string& hexColor) {
    const char* data = hexColor.data();
    const size_t size = hexColor.size();

    static unsigned char c;
    for (size_t i = 0; i < size; ++i) {
        c = data[i];
        // Branchless hex digit check: (c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f')
        if (!((c - '0') <= 9 || (c - 'A') <= 5 || (c - 'a') <= 5)) {
            return false;
        }
    }

    return true;
}

bool convertStrToRGBA4444(std::string hexColor, uint16_t* returnValue) {
    // Check if # is present
    if (hexColor.size() != 5 || hexColor[0] != '#')
        return false;

    hexColor = hexColor.substr(1);

    if (isValidRGBA4Color(hexColor)) {
        *returnValue = std::stoi(std::string(hexColor.rbegin(), hexColor.rend()), nullptr, 16);
        return true;
    }
    return false;
}

struct FullSettings {
    uint8_t refreshRate;
    bool setPosRight;
    bool showRealFreqs;
    bool realVolts;
	bool realTemps;
    bool showDeltas;
    bool showTargetFreqs;
    bool showFPS;
    bool showRES;
    bool showRDSD;
    std::string ramInfoMode;
    bool useDynamicColors;
    bool disableScreenshots;
    uint16_t separatorColor;
    uint16_t catColor1;
    uint16_t catColor2;
    uint16_t textColor;
};

struct MiniSettings {
    uint8_t refreshRate;
    bool realFrequencies;
    bool realVolts;
	bool realTemps;
    bool showFullCPU;
    bool showFullResolution;
    bool showFanPercentage;
    bool showSOCVoltage;
    bool useDynamicColors;
    bool showVDDQ;
    bool showVDD2;
    bool decimalVDD2;
    bool showDTC;
    bool useDTCSymbol;
    std::string dtcFormat;
    size_t handheldFontSize;
    size_t dockedFontSize;
    size_t spacing;
    uint16_t backgroundColor;
    uint16_t focusBackgroundColor;
    uint16_t separatorColor;
    uint16_t catColor;
    uint16_t textColor;
    std::string show;
    bool showpartLoad;
    bool showpartLoadCPUGPU;
    std::string ramInfoMode;
    bool invertBatteryDisplay;
    bool disableScreenshots;
    bool sleepExit;
    //int setPos;
    int frameOffsetX;
    int frameOffsetY;
    size_t framePadding;
};

struct MicroSettings {
    uint8_t refreshRate;
    bool realFrequencies;
    bool realVolts;
	bool realTemps;
    bool showFullCPU;
    bool showFullResolution;
    bool showSOCVoltage;
    bool useDynamicColors;
    bool showVDDQ;
    bool showVDD2;
    bool decimalVDD2;
    bool showDTC;
    bool useDTCSymbol;
    std::string dtcFormat;
    bool invertBatteryDisplay;
    size_t handheldFontSize;
    size_t dockedFontSize;
    uint8_t alignTo;
    uint16_t backgroundColor;
    uint16_t separatorColor;
    uint16_t catColor;
    uint16_t textColor;
    std::string show;
    bool showpartLoad;
    std::string ramInfoMode;
    bool setPosBottom;
    bool disableScreenshots;
    bool sleepExit;
};

struct FpsCounterSettings {
    uint8_t refreshRate;
    size_t handheldFontSize;
    size_t dockedFontSize;
    uint16_t backgroundColor;
    uint16_t focusBackgroundColor;
    uint16_t textColor;
    //int setPos;
    bool useIntegerCounter;
    bool disableScreenshots;
    int frameOffsetX;
    int frameOffsetY;
    size_t framePadding;
};

struct FpsGraphSettings {
    bool showInfo;
	bool realTemps;
    uint8_t refreshRate;
    uint16_t backgroundColor;
    uint16_t focusBackgroundColor;
    uint16_t fpsColor;
    uint16_t mainLineColor;
    uint16_t roundedLineColor;
    uint16_t perfectLineColor;
    uint16_t dashedLineColor;
    uint16_t borderColor;
    uint16_t maxFPSTextColor;
    uint16_t minFPSTextColor;
    uint16_t textColor;
    uint16_t catColor;
    //int setPos;
    bool useDynamicColors;
    bool disableScreenshots;
    int frameOffsetX;
    int frameOffsetY;
    size_t framePadding;
};

struct ResolutionSettings {
    uint8_t refreshRate;
    uint16_t backgroundColor;
    uint16_t focusBackgroundColor;
    uint16_t catColor;
    //uint16_t catColor2;
    uint16_t textColor;
    //int setPos;
    bool disableScreenshots;

    int frameOffsetX;
    int frameOffsetY;
    size_t framePadding;
};

ALWAYS_INLINE void GetConfigSettings(MiniSettings* settings) {
    // Initialize defaults
    settings->realFrequencies = true;
    settings->realVolts = true;
	settings->realTemps = true;
    settings->showFullCPU = false;
    settings->showFullResolution = true;
    settings->showFanPercentage = true;
    settings->useDynamicColors = true;
    settings->showFullCPU = false;
    settings->showSOCVoltage = false;
    settings->showVDDQ = false;
    settings->showVDD2 = true;
    settings->decimalVDD2 = false;
    settings->showDTC = true;
    settings->useDTCSymbol = true;
    settings->dtcFormat = "%m-%d-%Y%H:%M:%S";//"%Y-%m-%d %I:%M:%S %p";
    settings->handheldFontSize = 15;
    settings->dockedFontSize = 15;
    settings->spacing = 8;
    convertStrToRGBA4444("#0009", &(settings->backgroundColor));
    convertStrToRGBA4444("#000F", &(settings->focusBackgroundColor));
    convertStrToRGBA4444("#888F", &(settings->separatorColor));
    convertStrToRGBA4444("#2DFF", &(settings->catColor));
    convertStrToRGBA4444("#FFFF", &(settings->textColor));
    settings->show = "DTC+BAT+CPU+GPU+RAM+TMP+FPS+RES";
    settings->showpartLoad = true;
    settings->showpartLoadCPUGPU = false;
    settings->ramInfoMode = "LOAD";
    settings->invertBatteryDisplay = true;
    settings->refreshRate = 1;
    settings->disableScreenshots = false;
    settings->sleepExit = false;
    //settings->setPos = 0;
    settings->frameOffsetX = 10;
    settings->frameOffsetY = 10;
    settings->framePadding = 10;

    // Open and read file efficiently
    FILE* configFile = fopen(configIniPath, "r");
    if (!configFile) return;

    fseek(configFile, 0, SEEK_END);
    const long fileSize = ftell(configFile);
    fseek(configFile, 0, SEEK_SET);

    std::string fileData;
    fileData.resize(fileSize);
    fread(fileData.data(), 1, fileSize, configFile);
    fclose(configFile);

    auto parsedData = ult::parseIni(fileData);

    // Cache section lookup
    auto sectionIt = parsedData.find("mini");
    if (sectionIt == parsedData.end()) return;

    std::string key;
    uint16_t temp;

    const auto& section = sectionIt->second;

    // Process refresh_rate
    auto it = section.find("refresh_rate");
    if (it != section.end()) {
        settings->refreshRate = std::clamp(atol(it->second.c_str()), 1L, 60L);
    }

    // Process boolean flags
    it = section.find("real_freqs");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->realFrequencies = (key == "TRUE");
    }

    it = section.find("real_volts");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->realVolts = (key == "TRUE");
    }

    it = section.find("real_temps");
	if (it != section.end()) {
		key = it->second;
		convertToUpper(key);
		settings->realTemps = (key == "TRUE");
	}

    // Process font sizes with shared bounds
    static constexpr long minFontSize = 8;
    static constexpr long maxFontSize = 22;

    it = section.find("handheld_font_size");
    if (it != section.end()) {
        settings->handheldFontSize = std::clamp(atol(it->second.c_str()), minFontSize, maxFontSize);
    }

    it = section.find("docked_font_size");
    if (it != section.end()) {
        settings->dockedFontSize = std::clamp(atol(it->second.c_str()), minFontSize, maxFontSize);
    }

    it = section.find("spacing");
    if (it != section.end()) {
        settings->spacing = atol(it->second.c_str());
    }

    // Process colors
    it = section.find("background_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->backgroundColor = temp;
    }
    it = section.find("focus_background_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->focusBackgroundColor = temp;
    }

    it = section.find("separator_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->separatorColor = temp;
    }

    it = section.find("cat_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->catColor = temp;
    }

    it = section.find("text_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->textColor = temp;
    }

    // Process RAM load flag
    it = section.find("show_full_cpu");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showFullCPU = !(key == "FALSE");
    }

    it = section.find("show_full_res");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showFullResolution = !(key == "FALSE");
    }

    it = section.find("show_soc_voltage");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showSOCVoltage = !(key == "FALSE");
    }

    it = section.find("use_dynamic_colors");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->useDynamicColors = (key == "TRUE");
    }

    it = section.find("show_vddq");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showVDDQ = !(key == "FALSE");
    }

    it = section.find("show_vdd2");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showVDD2 = !(key == "FALSE");
    }

    it = section.find("decimal_vdd2");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->decimalVDD2 = !(key == "FALSE");
    }

    it = section.find("show_dtc");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showDTC = !(key == "FALSE");
    }

    it = section.find("use_dtc_symbol");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->useDTCSymbol = !(key == "FALSE");
    }

    it = section.find("dtc_format");
    if (it != section.end()) {
        key = it->second;
        settings->dtcFormat = std::move(key);
    }

    // Process show string
    it = section.find("show");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->show = std::move(key);
    }

    // Process RAM load flag
    it = section.find("replace_MB_with_RAM_load");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showpartLoad = (key != "FALSE");
    }

    // Process CPU/GPU RAM load flag
    it = section.find("show_RAM_load_CPU_GPU");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showpartLoadCPUGPU = (key != "FALSE");
    }

    // Process RAM info mode
    it = section.find("ram_info_mode");
    if (it != section.end()) {
        settings->ramInfoMode = it->second;
    }

    // Invert the battery display value
    it = section.find("invert_battery_display");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->invertBatteryDisplay = (key != "FALSE");
    }

    // Process disable screenshots
    it = section.find("disable_screenshots");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->disableScreenshots = (key != "FALSE");
    }

    // Process exit on sleep
    it = section.find("sleep_exit");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->sleepExit = (key != "FALSE");
    }

    // Process alignment settings
    //it = section.find("layer_width_align");
    //if (it != section.end()) {
    //    key = it->second;
    //    convertToUpper(key);
    //    if (key == "CENTER") {
    //        settings->setPos = 1;
    //    } else if (key == "RIGHT") {
    //        settings->setPos = 2;
    //    }
    //}
    //
    //it = section.find("layer_height_align");
    //if (it != section.end()) {
    //    key = it->second;
    //    convertToUpper(key);
    //    if (key == "CENTER") {
    //        settings->setPos += 3;
    //    } else if (key == "BOTTOM") {
    //        settings->setPos += 6;
    //    }
    //}

    it = section.find("frame_offset_x");
    if (it != section.end()) {
        settings->frameOffsetX = atol(it->second.c_str());
    }

    it = section.find("frame_offset_y");
    if (it != section.end()) {
        settings->frameOffsetY = atol(it->second.c_str());
    }

    it = section.find("frame_padding");
    if (it != section.end()) {
        settings->framePadding = atol(it->second.c_str());
    }
}

ALWAYS_INLINE void GetConfigSettings(MicroSettings* settings) {
    // Initialize defaults
    settings->realFrequencies = true;
    settings->realVolts = true;
	settings->realTemps = true;
    settings->showFullCPU = false;
    settings->showFullResolution = false;
    settings->showSOCVoltage = true;
    settings->useDynamicColors = true;
    settings->showVDDQ = false;
    settings->showVDD2 = true;
    settings->decimalVDD2 = false;
    settings->showDTC = true;
    settings->useDTCSymbol = true;
    settings->dtcFormat = "%H:%M:%S";//"%Y-%m-%d %I:%M:%S %p";
    settings->invertBatteryDisplay = false;
    settings->handheldFontSize = 14;
    settings->dockedFontSize = 14;
    settings->alignTo = 1; // CENTER
    convertStrToRGBA4444("#0009", &(settings->backgroundColor));
    convertStrToRGBA4444("#888F", &(settings->separatorColor));
    convertStrToRGBA4444("#2DFF", &(settings->catColor));
    convertStrToRGBA4444("#FFFF", &(settings->textColor));
    settings->show = "FPS+CPU+GPU+RAM+SOC+BAT+DTC";
    settings->showpartLoad = true;
    settings->ramInfoMode = "LOAD";
    settings->setPosBottom = false;
    settings->disableScreenshots = false;
    settings->sleepExit = false;
    settings->refreshRate = 1;

    // Open and read file efficiently
    FILE* configFile = fopen(configIniPath, "r");
    if (!configFile) return;

    fseek(configFile, 0, SEEK_END);
    const long fileSize = ftell(configFile);
    fseek(configFile, 0, SEEK_SET);

    std::string fileData;
    fileData.resize(fileSize);
    fread(fileData.data(), 1, fileSize, configFile);
    fclose(configFile);

    auto parsedData = ult::parseIni(fileData);

    // Cache section lookup
    auto sectionIt = parsedData.find("micro");
    if (sectionIt == parsedData.end()) return;

    std::string key;
    uint16_t temp;

    const auto& section = sectionIt->second;

    // Process refresh_rate
    auto it = section.find("refresh_rate");
    if (it != section.end()) {
        settings->refreshRate = std::clamp(atol(it->second.c_str()), 1L, 60L);
    }

    // Process boolean flags
    it = section.find("real_freqs");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->realFrequencies = (key == "TRUE");
    }

    it = section.find("real_volts");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->realVolts = (key == "TRUE");
    }

	it = section.find("real_temps");
	if (it != section.end()) {
		key = it->second;
		convertToUpper(key);
		settings->realTemps = (key == "TRUE");
    }

    it = section.find("show_full_cpu");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showFullCPU = (key == "TRUE");
    }

    it = section.find("show_full_res");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showFullResolution = (key == "TRUE");
    }

    it = section.find("show_soc_voltage");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showSOCVoltage = !(key == "FALSE");
    }

    it = section.find("use_dynamic_colors");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->useDynamicColors = (key == "TRUE");
    }

    it = section.find("show_vddq");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showVDDQ = !(key == "FALSE");
    }

    it = section.find("show_vdd2");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showVDD2 = !(key == "FALSE");
    }

    it = section.find("decimal_vdd2");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->decimalVDD2 = !(key == "FALSE");
    }

    it = section.find("show_dtc");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showDTC = !(key == "FALSE");
    }

    it = section.find("use_dtc_symbol");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->useDTCSymbol = !(key == "FALSE");
    }

    it = section.find("dtc_format");
    if (it != section.end()) {
        key = it->second;
        settings->dtcFormat = std::move(key);
    }

    // Invert the battery display value
    it = section.find("invert_battery_display");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->invertBatteryDisplay = (key != "FALSE");
    }

    // Process font sizes with shared bounds
    static constexpr long minFontSize = 8;
    static constexpr long maxFontSize = 18;

    it = section.find("handheld_font_size");
    if (it != section.end()) {
        settings->handheldFontSize = std::clamp(atol(it->second.c_str()), minFontSize, maxFontSize);
    }

    it = section.find("docked_font_size");
    if (it != section.end()) {
        settings->dockedFontSize = std::clamp(atol(it->second.c_str()), minFontSize, maxFontSize);
    }

    // Process colors
    it = section.find("background_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->backgroundColor = temp;
    }

    it = section.find("separator_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->separatorColor = temp;
    }

    it = section.find("cat_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->catColor = temp;
    }

    it = section.find("text_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->textColor = temp;
    }

    // Process text alignment
    it = section.find("text_align");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        if (key == "LEFT") {
            settings->alignTo = 0;
        } else if (key == "CENTER") {
            settings->alignTo = 1;
        } else if (key == "RIGHT") {
            settings->alignTo = 2;
        }
    }

    // Process RAM load flag
    it = section.find("replace_GB_with_RAM_load");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showpartLoad = (key != "FALSE");
    }

    // Process RAM info mode
    it = section.find("ram_info_mode");
    if (it != section.end()) {
        settings->ramInfoMode = it->second;
    }

    // Process show string
    it = section.find("show");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->show = std::move(key);
    }

    // Process layer height alignment
    it = section.find("layer_height_align");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->setPosBottom = (key == "BOTTOM");
    }

    // Process disable screenshots
    it = section.find("disable_screenshots");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->disableScreenshots = (key != "FALSE");
    }

    // Process exit on sleep
    it = section.find("sleep_exit");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->sleepExit = (key != "FALSE");
    }

}

ALWAYS_INLINE void GetConfigSettings(FpsCounterSettings* settings) {
    // Initialize defaults
    settings->handheldFontSize = 40;
    settings->dockedFontSize = 40;
    convertStrToRGBA4444("#0009", &(settings->backgroundColor));
    convertStrToRGBA4444("#000F", &(settings->focusBackgroundColor));
    convertStrToRGBA4444("#8CFF", &(settings->textColor));
    //settings->setPos = 0;
    settings->refreshRate = 30;
    settings->useIntegerCounter = false;
    settings->disableScreenshots = false;

    settings->frameOffsetX = 10;
    settings->frameOffsetY = 10;
    settings->framePadding = 10;

    // Open and read file efficiently
    FILE* configFile = fopen(configIniPath, "r");
    if (!configFile) return;

    fseek(configFile, 0, SEEK_END);
    const long fileSize = ftell(configFile);
    fseek(configFile, 0, SEEK_SET);

    std::string fileData;
    fileData.resize(fileSize);
    fread(fileData.data(), 1, fileSize, configFile);
    fclose(configFile);

    auto parsedData = ult::parseIni(fileData);

    // Cache section lookup
    auto sectionIt = parsedData.find("fps-counter");
    if (sectionIt == parsedData.end()) return;


    std::string key;
    uint16_t temp;

    const auto& section = sectionIt->second;

    // Process font sizes with shared bounds
    static constexpr long minFontSize = 8;
    static constexpr long maxFontSize = 150;

    auto it = section.find("handheld_font_size");
    if (it != section.end()) {
        settings->handheldFontSize = std::clamp(atol(it->second.c_str()), minFontSize, maxFontSize);
    }

    it = section.find("docked_font_size");
    if (it != section.end()) {
        settings->dockedFontSize = std::clamp(atol(it->second.c_str()), minFontSize, maxFontSize);
    }

    // Process colors
    it = section.find("background_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->backgroundColor = temp;
    }

    it = section.find("focus_background_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->focusBackgroundColor = temp;
    }


    it = section.find("text_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->textColor = temp;
    }

    // Process alignment settings
    //it = section.find("layer_width_align");
    //if (it != section.end()) {
    //    key = it->second;
    //    convertToUpper(key);
    //    if (key == "CENTER") {
    //        settings->setPos = 1;
    //    } else if (key == "RIGHT") {
    //        settings->setPos = 2;
    //    }
    //}

    //it = section.find("layer_height_align");
    //if (it != section.end()) {
    //    key = it->second;
    //    convertToUpper(key);
    //    if (key == "CENTER") {
    //        settings->setPos += 3;
    //    } else if (key == "BOTTOM") {
    //        settings->setPos += 6;
    //    }
    //}

    it = section.find("use_integer_counter");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->useIntegerCounter = (key != "FALSE");
    }

    // Process disable screenshots
    it = section.find("disable_screenshots");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->disableScreenshots = (key != "FALSE");
    }

    it = section.find("frame_offset_x");
    if (it != section.end()) {
        settings->frameOffsetX = atol(it->second.c_str());
    }

    it = section.find("frame_offset_y");
    if (it != section.end()) {
        settings->frameOffsetY = atol(it->second.c_str());
    }

    it = section.find("frame_padding");
    if (it != section.end()) {
        settings->framePadding = atol(it->second.c_str());
    }
}

ALWAYS_INLINE void GetConfigSettings(FpsGraphSettings* settings) {
    // Initialize defaults
    settings->showInfo = true;
	settings->realTemps = false;
    //settings->setPos = 0;
    convertStrToRGBA4444("#0009", &(settings->backgroundColor));
    convertStrToRGBA4444("#000F", &(settings->focusBackgroundColor));
    convertStrToRGBA4444("#888C", &(settings->fpsColor));
    convertStrToRGBA4444("#2DFF", &(settings->borderColor));
    convertStrToRGBA4444("#8888", &(settings->dashedLineColor));
    convertStrToRGBA4444("#FFFF", &(settings->maxFPSTextColor));
    convertStrToRGBA4444("#FFFF", &(settings->minFPSTextColor));
    convertStrToRGBA4444("#FFFF", &(settings->mainLineColor));
    convertStrToRGBA4444("#F0FF", &(settings->roundedLineColor));
    convertStrToRGBA4444("#0C0F", &(settings->perfectLineColor));

    convertStrToRGBA4444("#FFFF", &(settings->textColor));
    convertStrToRGBA4444("#0F0F", &(settings->catColor));

    settings->refreshRate = 30;
    settings->useDynamicColors = true;
    settings->disableScreenshots = false;

    settings->frameOffsetX = 10;
    settings->frameOffsetY = 10;
    settings->framePadding = 10;


    // Open and read file efficiently
    FILE* configFile = fopen(configIniPath, "r");
    if (!configFile) return;

    fseek(configFile, 0, SEEK_END);
    const long fileSize = ftell(configFile);
    fseek(configFile, 0, SEEK_SET);

    std::string fileData;
    fileData.resize(fileSize);
    fread(fileData.data(), 1, fileSize, configFile);
    fclose(configFile);

    auto parsedData = ult::parseIni(fileData);

    // Cache section lookup
    auto sectionIt = parsedData.find("fps-graph");
    if (sectionIt == parsedData.end()) return;

    std::string key;
    uint16_t temp;

    const auto& section = sectionIt->second;

    // Process alignment settings
    //auto it = section.find("layer_width_align");
    //if (it != section.end()) {
    //    key = it->second;
    //    convertToUpper(key);
    //    if (key == "CENTER") {
    //        settings->setPos = 1;
    //    } else if (key == "RIGHT") {
    //        settings->setPos = 2;
    //    }
    //}
    //
    //it = section.find("layer_height_align");
    //if (it != section.end()) {
    //    key = it->second;
    //    convertToUpper(key);
    //    if (key == "CENTER") {
    //        settings->setPos += 3;
    //    } else if (key == "BOTTOM") {
    //        settings->setPos += 6;
    //    }
    //}

    // Process show_info boolean
    auto it = section.find("show_info");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showInfo = (key == "TRUE");
    }

	    it = section.find("real_temps");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->realTemps = (key == "TRUE");
    }

    it = section.find("use_dynamic_colors");

    it = section.find("use_dynamic_colors");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->useDynamicColors = (key == "TRUE");
    }

    // Process disable screenshots
    it = section.find("disable_screenshots");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->disableScreenshots = (key != "FALSE");
    }

    it = section.find("frame_offset_x");
    if (it != section.end()) {
        settings->frameOffsetX = atol(it->second.c_str());
    }

    it = section.find("frame_offset_y");
    if (it != section.end()) {
        settings->frameOffsetY = atol(it->second.c_str());
    }

    it = section.find("frame_padding");
    if (it != section.end()) {
        settings->framePadding = atol(it->second.c_str());
    }


    // Process colors - using a struct for cleaner code
    struct ColorMapping {
        const char* key;
        uint16_t* target;
    };

    const ColorMapping colorMappings[] = {
        {"min_fps_text_color", &settings->minFPSTextColor},
        {"max_fps_text_color", &settings->maxFPSTextColor},
        {"background_color", &settings->backgroundColor},
        {"focus_background_color", &settings->focusBackgroundColor},
        {"fps_counter_color", &settings->fpsColor},
        {"border_color", &settings->borderColor},
        {"dashed_line_color", &settings->dashedLineColor},
        {"main_line_color", &settings->mainLineColor},
        {"rounded_line_color", &settings->roundedLineColor},
        {"perfect_line_color", &settings->perfectLineColor},
        {"text_color", &settings->textColor},
        {"cat_color", &settings->catColor}
    };

    for (const auto& mapping : colorMappings) {
        it = section.find(mapping.key);
        if (it != section.end()) {
            temp = 0;
            if (convertStrToRGBA4444(it->second, &temp))
                *(mapping.target) = temp;
        }
    }
}

ALWAYS_INLINE void GetConfigSettings(FullSettings* settings) {
    // Initialize defaults
    settings->setPosRight = false;
    settings->refreshRate = 1;
    settings->showRealFreqs = true;
	settings->realTemps = false;
    settings->showDeltas = true;
    settings->showTargetFreqs = true;
    settings->showFPS = true;
    settings->showRES = true;
    settings->showRDSD = true;
    settings->ramInfoMode = "LOAD";
    settings->useDynamicColors = true;
    settings->disableScreenshots = false;
    convertStrToRGBA4444("#888F", &(settings->separatorColor));
    convertStrToRGBA4444("#8FFF", &(settings->catColor1));
    convertStrToRGBA4444("#8CFF", &(settings->catColor2));
    convertStrToRGBA4444("#FFFF", &(settings->textColor));

    // Open and read file efficiently
    FILE* configFile = fopen(configIniPath, "r");
    if (!configFile) return;

    fseek(configFile, 0, SEEK_END);
    const long fileSize = ftell(configFile);
    fseek(configFile, 0, SEEK_SET);

    std::string fileData;
    fileData.resize(fileSize);
    fread(fileData.data(), 1, fileSize, configFile);
    fclose(configFile);

    auto parsedData = ult::parseIni(fileData);

    // Cache section lookup
    auto sectionIt = parsedData.find("full");
    if (sectionIt == parsedData.end()) return;

    std::string key;
    uint16_t temp;

    const auto& section = sectionIt->second;

    // Process refresh_rate
    auto it = section.find("refresh_rate");
    if (it != section.end()) {
        settings->refreshRate = std::clamp(atol(it->second.c_str()), 1L, 60L);
    }

    // Process layer position
    it = section.find("layer_width_align");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->setPosRight = (key == "RIGHT");
    }

    // Process boolean flags
    it = section.find("show_real_freqs");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showRealFreqs = !(key == "FALSE");
    }

	it = section.find("real_temps");
	if (it != section.end()) {
		key = it->second;
		convertToUpper(key);
		settings->realTemps = (key == "TRUE");
	}

    it = section.find("show_deltas");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showDeltas = !(key == "FALSE");
    }

    it = section.find("show_target_freqs");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showTargetFreqs = !(key == "FALSE");
    }

    it = section.find("show_fps");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showFPS = !(key == "FALSE");
    }

    it = section.find("show_res");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showRES = !(key == "FALSE");
    }

    it = section.find("show_read_speed");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->showRDSD = !(key == "FALSE");
    }

    it = section.find("ram_info_mode");
    if (it != section.end()) {
        settings->ramInfoMode = it->second;
    }

    it = section.find("use_dynamic_colors");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->useDynamicColors = (key == "TRUE");
    }

    // Process disable screenshots
    it = section.find("disable_screenshots");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->disableScreenshots = (key != "FALSE");
    }

    it = section.find("separator_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->separatorColor = temp;
    }

    it = section.find("cat_color_1");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->catColor1 = temp;
    }

    it = section.find("cat_color_2");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->catColor2 = temp;
    }

    it = section.find("text_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->textColor = temp;
    }
}

ALWAYS_INLINE void GetConfigSettings(ResolutionSettings* settings) {
    // Initialize defaults
    convertStrToRGBA4444("#0009", &(settings->backgroundColor));
    convertStrToRGBA4444("#000F", &(settings->focusBackgroundColor));
    convertStrToRGBA4444("#8FFF", &(settings->catColor));
    //convertStrToRGBA4444("#8CFF", &(settings->catColor2));
    convertStrToRGBA4444("#FFFF", &(settings->textColor));
    settings->refreshRate = 10;
    //ettings->setPos = 0;
    settings->disableScreenshots = false;

    settings->frameOffsetX = 10;
    settings->frameOffsetY = 10;
    settings->framePadding = 10;


    // Open and read file efficiently
    FILE* configFile = fopen(configIniPath, "r");
    if (!configFile) return;

    fseek(configFile, 0, SEEK_END);
    const long fileSize = ftell(configFile);
    fseek(configFile, 0, SEEK_SET);

    std::string fileData;
    fileData.resize(fileSize);
    fread(fileData.data(), 1, fileSize, configFile);
    fclose(configFile);

    auto parsedData = ult::parseIni(fileData);

    // Cache section lookup
    auto sectionIt = parsedData.find("game_resolutions");
    if (sectionIt == parsedData.end()) return;

    std::string key;

    const auto& section = sectionIt->second;

    // Process refresh_rate
    auto it = section.find("refresh_rate");
    if (it != section.end()) {
        settings->refreshRate = std::clamp(atol(it->second.c_str()), 1L, 60L);
    }

    uint16_t temp;

    // Process colors
    it = section.find("background_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->backgroundColor = temp;
    }

    it = section.find("focus_background_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->focusBackgroundColor = temp;
    }

    it = section.find("cat_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->catColor = temp;
    }

    //it = section.find("cat_color_2");
    //if (it != section.end()) {
    //    temp = 0;
    //    if (convertStrToRGBA4444(it->second, &temp))
    //        settings->catColor2 = temp;
    //}


    it = section.find("text_color");
    if (it != section.end()) {
        temp = 0;
        if (convertStrToRGBA4444(it->second, &temp))
            settings->textColor = temp;
    }

    it = section.find("frame_offset_x");
    if (it != section.end()) {
        settings->frameOffsetX = atol(it->second.c_str());
    }

    it = section.find("frame_offset_y");
    if (it != section.end()) {
        settings->frameOffsetY = atol(it->second.c_str());
    }

    it = section.find("frame_padding");
    if (it != section.end()) {
        settings->framePadding = atol(it->second.c_str());
    }

    // Process alignment settings
   //it = section.find("layer_width_align");
   //if (it != section.end()) {
   //    key = it->second;
   //    convertToUpper(key);
   //    if (key == "CENTER") {
   //        settings->setPos = 1;
   //    } else if (key == "RIGHT") {
   //        settings->setPos = 2;
   //    }
   //}
   //
   //it = section.find("layer_height_align");
   //if (it != section.end()) {
   //    key = it->second;
   //    convertToUpper(key);
   //    if (key == "CENTER") {
   //        settings->setPos += 3;
   //    } else if (key == "BOTTOM") {
   //        settings->setPos += 6;
   //    }
   //}

    // Process disable screenshots
    it = section.find("disable_screenshots");
    if (it != section.end()) {
        key = it->second;
        convertToUpper(key);
        settings->disableScreenshots = (key != "FALSE");
    }
}