/*
 * Copyright (c) Souldbminer, Lightos and Horizon OC Contributors
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

#include "../hos/process_management.hpp"
#include "governor.hpp"
#include <hocclk/clock_manager.h>

namespace governor {

#define POLL_NS 5'000'000   // 5 ms  – governor poll rate
#define DOWN_HOLD_TICKS 10  // 50 ms – how long to in POLL_NS to hold while ramping down
#define STEP_UTIL 900       // multiplier for step calculations

    bool isGpuGovernorEnabled = false;
    bool isCpuGovernorEnabled = false;
    bool lastGpuGovernorState = false;
    bool lastCpuGovernorState = false;
    bool lastVrrGovernorState = false;
    bool hasChanged = true;
    bool isCpuGovernorInBoostMode = false;
    bool isVRREnabled = false;

    Thread governorTHREAD;

    void HandleGovernor(uint32_t targetHz) {
        u32 tempTargetHz = clockManager::gContext.overrideFreqs[HocClkModule_Governor];
        if (!tempTargetHz) {
            tempTargetHz = config::GetAutoClockHz(clockManager::gContext.applicationId, HocClkModule_Governor, clockManager::gContext.profile, true);
            if (!tempTargetHz)
                tempTargetHz = config::GetAutoClockHz(HOCCLK_GLOBAL_PROFILE_TID, HocClkModule_Governor, clockManager::gContext.profile, true);
        }

        auto resolve = [](u8 app, u8 temp) -> u8 {
            if (temp == ComponentGovernor_Disabled)
                return ComponentGovernor_Disabled;
            if (temp != ComponentGovernor_DoNotOverride)
                return temp;
            return app;
        };

        u8 effectiveCpu = resolve(GovernorStateCpu(targetHz), GovernorStateCpu(tempTargetHz));
        u8 effectiveGpu = resolve(GovernorStateGpu(targetHz), GovernorStateGpu(tempTargetHz));
        u8 effectiveVrr = resolve(GovernorStateVrr(targetHz), GovernorStateVrr(tempTargetHz));

        bool newCpuGovernorState = (effectiveCpu == ComponentGovernor_Enabled);
        bool newGpuGovernorState = (effectiveGpu == ComponentGovernor_Enabled);
        bool newVrrGovernorState = (effectiveVrr == ComponentGovernor_Enabled);

        isCpuGovernorEnabled = newCpuGovernorState;
        isGpuGovernorEnabled = newGpuGovernorState;
        isVRREnabled = newVrrGovernorState;

        if (newCpuGovernorState == false && lastCpuGovernorState == true)
            board::ResetToStockCpu();
        if (newGpuGovernorState == false && lastGpuGovernorState == true)
            board::ResetToStockGpu();
        if (newVrrGovernorState == false && lastVrrGovernorState == true)
            board::ResetToStockDisplay();

        if (newCpuGovernorState != lastCpuGovernorState || newGpuGovernorState != lastGpuGovernorState ||
            newVrrGovernorState != lastVrrGovernorState) {
            fileUtils::LogLine("[mgr] Governor state changed: CPU %s, GPU %s, VRR %s", newCpuGovernorState ? "enabled" : "disabled",
                               newGpuGovernorState ? "enabled" : "disabled", newVrrGovernorState ? "enabled" : "disabled");
            lastCpuGovernorState = newCpuGovernorState;
            lastGpuGovernorState = newGpuGovernorState;
            lastVrrGovernorState = newVrrGovernorState;
        }
    }

    u32 SchedutilTargetHz(u32 util, u32 tableMaxHz) {
        u64 hz = (u64)tableMaxHz * util / STEP_UTIL;
        return (u32)(std::min(hz, static_cast<u64>(tableMaxHz)));
    }

    u32 TableIndexForHz(const clockManager::FreqTable &table, u32 targetHz) {
        for (u32 i = 0; i < table.count; i++)
            if (table.list[i] >= targetHz)
                return i;
        return table.count - 1;
    }

    u32 ResolveTargetHz(HocClkModule module) {
        u32 hz = clockManager::gContext.overrideFreqs[module];
        if (!hz)
            hz = config::GetAutoClockHz(clockManager::gContext.applicationId, module, clockManager::gContext.profile, false);
        if (!hz)
            hz = config::GetAutoClockHz(HOCCLK_GLOBAL_PROFILE_TID, module, clockManager::gContext.profile, false);
        return hz;
    }

    void GovernorThread(void *arg) {
        (void)arg;

        u32 cpuDownHoldRemaining = 0;
        u32 cpuLastHz = 0;
        u32 gpuDownHoldRemaining = 0;
        u32 gpuLastHz = 0;
        u32 minHz = 612;
        u32 cpuTick = 0;
        u8 vrrTick = 0;
        u8 vrrFocusTick = 0;

        for (;;) {

            if (!clockManager::gRunning) {
                cpuDownHoldRemaining = 0;
                cpuLastHz = 0;
                gpuDownHoldRemaining = 0;
                gpuLastHz = 0;
                svcSleepThread(POLL_NS);
                continue;
            }

            if (isCpuGovernorEnabled) {
                u32 mode = 0;
                Result rc = apmExtGetCurrentPerformanceConfiguration(&mode);

                if (R_SUCCEEDED(rc) && apmExtIsBoostMode(mode)) {
                    isCpuGovernorInBoostMode = true;
                    cpuDownHoldRemaining = 0;
                    cpuLastHz = 0;
                } else {
                    isCpuGovernorInBoostMode = false;

                    auto &table = clockManager::gFreqTable[HocClkModule_CPU];
                    std::scoped_lock lock{ clockManager::gContextMutex };

                    u32 cpuLoad = board::GetPartLoad(HocClkPartLoad_CPUMax);
                    u32 tableMaxHz = table.list[table.count - 1];
                    u32 desiredHz = SchedutilTargetHz(cpuLoad, tableMaxHz);
                    u32 targetHz = ResolveTargetHz(HocClkModule_CPU);
                    u32 maxHz = clockManager::GetMaxAllowedHz(HocClkModule_CPU, clockManager::gContext.profile);

                    if (targetHz && desiredHz > targetHz)
                        desiredHz = targetHz;
                    if (maxHz && desiredHz > maxHz)
                        desiredHz = maxHz;

                    u32 newHz = table.list[TableIndexForHz(table, desiredHz)];
                    bool goingDown = (cpuLastHz != 0) && (newHz < cpuLastHz);

                    if (!goingDown)
                        cpuDownHoldRemaining = 0;
                    else if (cpuDownHoldRemaining == 0)
                        cpuDownHoldRemaining = DOWN_HOLD_TICKS;

                    if (cpuDownHoldRemaining > 0)
                        cpuDownHoldRemaining--;

                    if (++cpuTick > 50) {
                        minHz = config::GetConfigValue(HocClkConfigValue_CpuGovernorMinimumFreq);
                        if (config::GetConfigValue(HocClkConfigValue_AutoRAMCPUOverclock)) {
                            u32 ramHz = clockManager::gContext.freqs[HocClkModule_MEM];
                            u32 threshold = (u32)config::GetConfigValue(HocClkConfigValue_AutoRamCpuRamOCThreshold) * 1000;
                            if (ramHz >= threshold) {
                                u32 overrideHz = (u32)config::GetConfigValue(HocClkConfigValue_AutoRamCpuCpuOCFreq) * 1000;
                                if (overrideHz > minHz)
                                    minHz = overrideHz;
                            }
                        }
                        cpuTick = 0;
                    }

                    if (newHz < minHz)
                        newHz = minHz;

                    if ((!goingDown || (cpuDownHoldRemaining == 0)) && clockManager::IsAssignableHz(HocClkModule_CPU, newHz)) {
                        board::SetHz(HocClkModule_CPU, newHz);
                        clockManager::gContext.freqs[HocClkModule_CPU] = newHz;
                        clockManager::gContext.stable.freqs[HocClkModule_CPU] = newHz;
                        cpuLastHz = newHz;
                    }
                }
            } else {
                isCpuGovernorInBoostMode = false;
                cpuDownHoldRemaining = 0;
                cpuLastHz = 0;
            }

            if (isGpuGovernorEnabled) {
                auto &table = clockManager::gFreqTable[HocClkModule_GPU];
                std::scoped_lock lock{ clockManager::gContextMutex };

                u32 gpuLoad = board::GetPartLoad(HocClkPartLoad_GPU);
                u32 tableMaxHz = table.list[table.count - 1];
                u32 desiredHz = SchedutilTargetHz(gpuLoad, tableMaxHz);
                u32 targetHz = ResolveTargetHz(HocClkModule_GPU);
                u32 maxHz = clockManager::GetMaxAllowedHz(HocClkModule_GPU, clockManager::gContext.profile);

                if (targetHz && desiredHz > targetHz)
                    desiredHz = targetHz;
                if (maxHz && desiredHz > maxHz)
                    desiredHz = maxHz;

                u32 newHz = table.list[TableIndexForHz(table, desiredHz)];
                bool goingDown = (gpuLastHz != 0) && (newHz < gpuLastHz);

                if (!goingDown)
                    gpuDownHoldRemaining = 0;
                else if (gpuDownHoldRemaining == 0)
                    gpuDownHoldRemaining = DOWN_HOLD_TICKS;

                if (gpuDownHoldRemaining > 0)
                    gpuDownHoldRemaining--;

                if ((!goingDown || (gpuDownHoldRemaining == 0)) && clockManager::IsAssignableHz(HocClkModule_GPU, newHz)) {
                    board::SetHz(HocClkModule_GPU, newHz);
                    clockManager::gContext.freqs[HocClkModule_GPU] = newHz;
                    clockManager::gContext.stable.freqs[HocClkModule_GPU] = newHz;
                    gpuLastHz = newHz;
                }
            } else {
                gpuDownHoldRemaining = 0;
                gpuLastHz = 0;
            }

            if (isVRREnabled && clockManager::gContext.profile != HocClkProfile_Docked && clockManager::gContext.isSaltyNXInstalled) {
                bool skipVrr = false;

                if (++vrrFocusTick > 100) {
                    vrrFocusTick = 0;
                    bool isApplicationOutOfFocus = false;
                    Result rc = processManagement::isApplicationOutOfFocus(&isApplicationOutOfFocus);
                    if (R_FAILED(rc) || isApplicationOutOfFocus) {
                        board::ResetToStockDisplay();
                        skipVrr = true;
                    }
                }

                if (!skipVrr) {
                    u8 fps = integrations::GetSaltyNXFPS();

                    if (fps != 254) {
                        std::scoped_lock lock{ clockManager::gContextMutex };

                        u32 targetHz = clockManager::gContext.overrideFreqs[HocClkModule_Display];
                        if (!targetHz) {
                            targetHz = config::GetAutoClockHz(clockManager::gContext.applicationId, HocClkModule_Display,
                                                              clockManager::gContext.profile, false);
                            if (!targetHz)
                                targetHz =
                                    config::GetAutoClockHz(HOCCLK_GLOBAL_PROFILE_TID, HocClkModule_Display, clockManager::gContext.profile, false);
                        }

                        u8 maxDisplay = targetHz ? (u8)targetHz : 60;
                        u8 minDisplay = board::GetConsoleType() == HocClkConsoleType_Aula ? 45 : 40;

                        if (maxDisplay != minDisplay) {
                            if (fps >= minDisplay && fps <= maxDisplay) {
                                board::SetHz(HocClkModule_Display, fps);
                                clockManager::gContext.freqs[HocClkModule_Display] = fps;
                                clockManager::gContext.realFreqs[HocClkModule_Display] = fps;
                                clockManager::gContext.stable.freqs[HocClkModule_Display] = fps;
                                clockManager::gContext.stable.realFreqs[HocClkModule_Display] = fps;
                            } else {
                                for (u32 i = 0; i < 10; i++) {
                                    u32 compareHz = fps * i;
                                    if (compareHz >= minDisplay && compareHz <= maxDisplay) {
                                        board::SetHz(HocClkModule_Display, compareHz);
                                        clockManager::gContext.freqs[HocClkModule_Display] = compareHz;
                                        clockManager::gContext.realFreqs[HocClkModule_Display] = compareHz;
                                        clockManager::gContext.stable.freqs[HocClkModule_Display] = compareHz;
                                        clockManager::gContext.stable.realFreqs[HocClkModule_Display] = compareHz;
                                        break;
                                    }
                                }
                            }

                            if (++vrrTick > 50) {
                                vrrTick = 0;
                                board::SetHz(HocClkModule_Display, maxDisplay);
                                svcSleepThread(50'000'000);
                            }
                        }
                    }
                }
            }

            svcSleepThread(POLL_NS);
        }
    }

    void startThreads() {
        threadCreate(&governorTHREAD, GovernorThread, nullptr, NULL, 0x2000, 0x3F, -2);
        threadStart(&governorTHREAD);
    }

    void exitThreads() {
        threadClose(&governorTHREAD);
    }
}  // namespace governor
