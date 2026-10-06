/*
 * Copyright (c) Souldbminer, Lightos_ and Horizon OC Contributors
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

/* --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include <algorithm>
#include <battery.h>
#include <hocclk.h>
#include <i2c.h>
#include <math.h>
#include <max17050.h>
#include <minIni.h>
#include <numeric>
#include <switch.h>
#include <t210.h>
#include <tmp451.h>

#include "../hos/apm_ext.h"
#include "board.hpp"
#include "board_misc.hpp"
#include <ipc_server.h>
#include <lockable_mutex.h>


namespace board {

    Thread gpuThread;
    LEvent threadexit;
    Thread cpuCore0Thread;
    Thread cpuCore1Thread;
    Thread cpuCore2Thread;

    u32 gpuLoad;
    u32 _fd, _fd2;
    Result nvCheckSched = 0;
    Result nvCheck_load = 0;
    u64 idletick0 = 0;
    u64 idletick1 = 0;
    u64 idletick2 = 0;

    constexpr u64 CpuTimeOutNs = 500'000'000;
    constexpr double Systemtickfrequency = 19200000.0 * (static_cast<double>(CpuTimeOutNs) / 1'000'000'000.0);

    void GpuLoadThread(void *ptr) {
#define gpu_samples_average 8
#define NVGPU_GPU_IOCTL_PMU_GET_GPU_LOAD 0x80044715
        uint32_t gpu_load_array[gpu_samples_average] = { 0 };
        size_t i = 0;
        if (R_SUCCEEDED(nvCheck_load))
            do {
                u32 temp;
                if (R_SUCCEEDED(nvIoctl(_fd, NVGPU_GPU_IOCTL_PMU_GET_GPU_LOAD, &temp))) {
                    gpu_load_array[i++ % gpu_samples_average] = temp;
                    gpuLoad = std::accumulate(&gpu_load_array[0], &gpu_load_array[gpu_samples_average], 0) / gpu_samples_average;
                }
                svcSleepThread(16'666'000);  // wait a bit (this is the perfect amount of time to keep the reading accurate)
            } while (true);
    }

    void CheckCore(void *idletickPtr) {
        u64 *idletick = static_cast<u64 *>(idletickPtr);
        while (true) {
            u64 idletickA;
            u64 idletickB;
            svcGetInfo(&idletickB, InfoType_IdleTickCount, INVALID_HANDLE, -1);
            svcWaitForAddress(&threadexit, ArbitrationType_WaitIfEqual, 0, CpuTimeOutNs);
            svcGetInfo(&idletickA, InfoType_IdleTickCount, INVALID_HANDLE, -1);
            *idletick = idletickA - idletickB;
        }
    }

    void StartLoad(Result nvCheck, u32 fd) {
        _fd = fd;
        nvCheck_load = nvCheck;
        threadCreate(&gpuThread, GpuLoadThread, &nvCheck, NULL, 0x1000, 0x3F, -2);
        threadStart(&gpuThread);

        leventClear(&threadexit);
        threadCreate(&cpuCore0Thread, CheckCore, &idletick0, NULL, 0x1000, 0x10, 0);
        threadCreate(&cpuCore1Thread, CheckCore, &idletick1, NULL, 0x1000, 0x10, 1);
        threadCreate(&cpuCore2Thread, CheckCore, &idletick2, NULL, 0x1000, 0x10, 2);

        threadStart(&cpuCore0Thread);
        threadStart(&cpuCore1Thread);
        threadStart(&cpuCore2Thread);
    }

    u32 GetMaxCpuLoad() {
        float cpuUsage0 = std::clamp(((Systemtickfrequency - idletick0) / static_cast<double>(Systemtickfrequency)) * 1000.0, 0.0, 1000.0);
        float cpuUsage1 = std::clamp(((Systemtickfrequency - idletick1) / static_cast<double>(Systemtickfrequency)) * 1000.0, 0.0, 1000.0);
        float cpuUsage2 = std::clamp(((Systemtickfrequency - idletick2) / static_cast<double>(Systemtickfrequency)) * 1000.0, 0.0, 1000.0);
        return std::round(std::max({ cpuUsage0, cpuUsage1, cpuUsage2 }));
    }

    u32 GetPartLoad(HocClkPartLoad loadSource) {
        switch (loadSource) {
            case HocClkPartLoad_EMC:
                return t210EmcLoadAll();
            case HocClkPartLoad_EMCCpu:
                return t210EmcLoadCpu();
            case HocClkPartLoad_GPU:
                return gpuLoad;
            case HocClkPartLoad_CPUMax:
                return GetMaxCpuLoad();
            case HocClkPartLoad_BAT:
                BatteryChargeInfo info;
                batteryInfoGetChargeInfo(&info);
                return info.RawBatteryCharge;
            case HocClkPartLoad_FAN:
                return GetFanLevel();
            case HocClkPartLoad_RamBWAll:
                return t210EmcBwAll();
            case HocClkPartLoad_RamBWCpu:
                return t210EmcBwCpu();
            case HocClkPartLoad_RamBWGpu:
                return t210EmcBwGpu();
            case HocClkPartLoad_RamBWPeak:
                return t210EmcBwPeak();
            default:
                ASSERT_ENUM_VALID(HocClkPartLoad, loadSource);
        }
        return 0;
    }

    void ExitLoad() {
        threadClose(&gpuThread);
        threadClose(&cpuCore0Thread);
        threadClose(&cpuCore1Thread);
        threadClose(&cpuCore2Thread);
    }

    namespace {
        constexpr u32 NVschedCtrlEnable = 0x00000601;
        constexpr u32 NVschedCtrlDisable = 0x00000602;
    }  // namespace

    void SetGpuSchedulingMode(GpuSchedulingMode mode, GpuSchedulingOverrideMethod method) {
        if (R_FAILED(nvCheckSched) && method == GpuSchedulingOverrideMethod_NvService) {
            return;
        }

        u32 temp;
        bool enabled = false;
        switch (mode) {
            case GpuSchedulingMode_DoNotOverride:
                break;
            case GpuSchedulingMode_Disabled:
                if (method == GpuSchedulingOverrideMethod_NvService) {
                    nvIoctl(_fd2, NVschedCtrlDisable, &temp);
                } else {
                    enabled = false;
                }
                break;
            case GpuSchedulingMode_Enabled:
                if (method == GpuSchedulingOverrideMethod_NvService) {
                    nvIoctl(_fd2, NVschedCtrlEnable, &temp);
                } else {
                    enabled = true;
                }
                break;
            default:
                ASSERT_ENUM_VALID(GpuSchedulingMode, mode);
        }

        if (method == GpuSchedulingOverrideMethod_Ini) {
            constexpr const char *IniPath = "sdmc:/atmosphere/config/system_settings.ini";
            constexpr const char *Section = "am.gpu";
            constexpr const char *Key = "gpu_scheduling_enabled";

            const char *value = enabled ? "u8!0x1" : "u8!0x0";

            ini_puts(Section, Key, value, IniPath);
        }
    }

    void SchedSetFD2(u32 fd2) {
        _fd2 = fd2;
    }

    void NvSchedSucceed(Result nvSched) {
        nvCheckSched = nvSched;
    }

}  // namespace board
