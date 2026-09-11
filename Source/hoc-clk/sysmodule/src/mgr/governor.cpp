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

namespace mgr {

#define DOWN_HOLD_TICKS_DEFAULT 10 // 50 ms at 5ms poll – how long to hold while ramping down
#define STEP_UTIL_DEFAULT 900      // multiplier for step calculations (max freq at 90% load)

    u64 GovernorPollNs() {
        u64 ms = file::config::GetConfigValue(HocClkConfigValue_GovernorPollRateMs);
        if (ms < 1 || ms > 50)
            ms = 5;
        return ms * 1'000'000ULL;
    }

    u32 GovernorDownHoldTicks() {
        u64 ticks = file::config::GetConfigValue(HocClkConfigValue_GovernorDownHoldTicks);
        return (ticks <= 100) ? (u32)ticks : DOWN_HOLD_TICKS_DEFAULT;
    }

    u32 GovernorStepUtil() {
        u64 step = file::config::GetConfigValue(HocClkConfigValue_GovernorStepUtil);
        if (step < 100 || step > 1500)
            step = STEP_UTIL_DEFAULT;
        return (u32)step;
    }

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
        u32 tempTargetHz = mgr::gContext.overrideFreqs[HocClkModule_Governor];
        if (!tempTargetHz) {
            tempTargetHz = file::config::GetAutoClockHz(mgr::gContext.applicationId, HocClkModule_Governor, mgr::gContext.profile, true);
            if (!tempTargetHz)
                tempTargetHz = file::config::GetAutoClockHz(HOCCLK_GLOBAL_PROFILE_TID, HocClkModule_Governor, mgr::gContext.profile, true);
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
            file::utils::LogLine("[mgr] Governor state changed: CPU %s, GPU %s, VRR %s", newCpuGovernorState ? "enabled" : "disabled",
                               newGpuGovernorState ? "enabled" : "disabled", newVrrGovernorState ? "enabled" : "disabled");
            lastCpuGovernorState = newCpuGovernorState;
            lastGpuGovernorState = newGpuGovernorState;
            lastVrrGovernorState = newVrrGovernorState;
        }
    }

    u32 SchedutilTargetHz(u32 util, u32 tableMaxHz) {
        u64 hz = (u64)tableMaxHz * util / GovernorStepUtil();
        return (u32)(std::min(hz, static_cast<u64>(tableMaxHz)));
    }

    u32 TableIndexForHz(const mgr::FreqTable &table, u32 targetHz) {
        for (u32 i = 0; i < table.count; i++)
            if (table.list[i] >= targetHz)
                return i;
        return table.count - 1;
    }

    u32 ResolveTargetHz(HocClkModule module) {
        u32 hz = mgr::gContext.overrideFreqs[module];
        if (!hz)
            hz = file::config::GetAutoClockHz(mgr::gContext.applicationId, module, mgr::gContext.profile, false);
        if (!hz)
            hz = file::config::GetAutoClockHz(HOCCLK_GLOBAL_PROFILE_TID, module, mgr::gContext.profile, false);
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
            const u64 pollNs = GovernorPollNs();

            if (!mgr::gRunning) {
                cpuDownHoldRemaining = 0;
                cpuLastHz = 0;
                gpuDownHoldRemaining = 0;
                gpuLastHz = 0;
                svcSleepThread(pollNs);
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

                    auto &table = mgr::gFreqTable[HocClkModule_CPU];
                    std::scoped_lock lock{ mgr::gContextMutex };

                    u32 cpuLoad = board::GetPartLoad(HocClkPartLoad_CPUMax);
                    u32 tableMaxHz = table.list[table.count - 1];
                    u32 desiredHz = SchedutilTargetHz(cpuLoad, tableMaxHz);
                    u32 targetHz = ResolveTargetHz(HocClkModule_CPU);
                    u32 maxHz = mgr::GetMaxAllowedHz(HocClkModule_CPU, mgr::gContext.profile);

                    if (targetHz && desiredHz > targetHz)
                        desiredHz = targetHz;
                    if (maxHz && desiredHz > maxHz)
                        desiredHz = maxHz;

                    u32 newHz = table.list[TableIndexForHz(table, desiredHz)];
                    bool goingDown = (cpuLastHz != 0) && (newHz < cpuLastHz);

                    if (!goingDown)
                        cpuDownHoldRemaining = 0;
                    else if (cpuDownHoldRemaining == 0)
                        cpuDownHoldRemaining = GovernorDownHoldTicks();

                    if (cpuDownHoldRemaining > 0)
                        cpuDownHoldRemaining--;

                    if (++cpuTick > 50) {
                        minHz = file::config::GetConfigValue(HocClkConfigValue_CpuGovernorMinimumFreq);
                        if (file::config::GetConfigValue(HocClkConfigValue_AutoRAMCPUOverclock)) {
                            u32 ramHz = mgr::gContext.freqs[HocClkModule_MEM];
                            u32 threshold = (u32)file::config::GetConfigValue(HocClkConfigValue_AutoRamCpuRamOCThreshold) * 1000;
                            if (ramHz >= threshold) {
                                u32 overrideHz = (u32)file::config::GetConfigValue(HocClkConfigValue_AutoRamCpuCpuOCFreq) * 1000;
                                if (overrideHz > minHz)
                                    minHz = overrideHz;
                            }
                        }
                        cpuTick = 0;
                    }

                    if (newHz < minHz)
                        newHz = minHz;

                    if ((!goingDown || (cpuDownHoldRemaining == 0)) && mgr::IsAssignableHz(HocClkModule_CPU, newHz)) {
                        board::SetHz(HocClkModule_CPU, newHz);
                        mgr::gContext.freqs[HocClkModule_CPU] = newHz;
                        mgr::gContext.stable.freqs[HocClkModule_CPU] = newHz;
                        cpuLastHz = newHz;
                    }
                }
            } else {
                isCpuGovernorInBoostMode = false;
                cpuDownHoldRemaining = 0;
                cpuLastHz = 0;
            }

            if (isGpuGovernorEnabled) {
                auto &table = mgr::gFreqTable[HocClkModule_GPU];
                std::scoped_lock lock{ mgr::gContextMutex };

                u32 gpuLoad = board::GetPartLoad(HocClkPartLoad_GPU);
                u32 tableMaxHz = table.list[table.count - 1];
                u32 desiredHz = SchedutilTargetHz(gpuLoad, tableMaxHz);
                u32 targetHz = ResolveTargetHz(HocClkModule_GPU);
                u32 maxHz = mgr::GetMaxAllowedHz(HocClkModule_GPU, mgr::gContext.profile);

                if (targetHz && desiredHz > targetHz)
                    desiredHz = targetHz;
                if (maxHz && desiredHz > maxHz)
                    desiredHz = maxHz;

                u32 newHz = table.list[TableIndexForHz(table, desiredHz)];
                bool goingDown = (gpuLastHz != 0) && (newHz < gpuLastHz);

                if (!goingDown)
                    gpuDownHoldRemaining = 0;
                else if (gpuDownHoldRemaining == 0)
                    gpuDownHoldRemaining = GovernorDownHoldTicks();

                if (gpuDownHoldRemaining > 0)
                    gpuDownHoldRemaining--;

                if ((!goingDown || (gpuDownHoldRemaining == 0)) && mgr::IsAssignableHz(HocClkModule_GPU, newHz)) {
                    board::SetHz(HocClkModule_GPU, newHz);
                    mgr::gContext.freqs[HocClkModule_GPU] = newHz;
                    mgr::gContext.stable.freqs[HocClkModule_GPU] = newHz;
                    gpuLastHz = newHz;
                }
            } else {
                gpuDownHoldRemaining = 0;
                gpuLastHz = 0;
            }

            if (isVRREnabled && mgr::gContext.profile != HocClkProfile_Docked && mgr::gContext.isSaltyNXInstalled) {
                bool skipVrr = false;

                if (++vrrFocusTick > 100) {
                    vrrFocusTick = 0;
                    bool isApplicationOutOfFocus = false;
                    Result rc = hos::IsApplicationOutOfFocus(&isApplicationOutOfFocus);
                    if (R_FAILED(rc) || isApplicationOutOfFocus) {
                        board::ResetToStockDisplay();
                        skipVrr = true;
                    }
                }

                if (!skipVrr) {
                    u8 fps = hos::GetSaltyNXFPS();

                    if (fps != 254) {
                        std::scoped_lock lock{ mgr::gContextMutex };

                        u32 targetHz = mgr::gContext.overrideFreqs[HocClkModule_Display];
                        if (!targetHz) {
                            targetHz = file::config::GetAutoClockHz(mgr::gContext.applicationId, HocClkModule_Display,
                                                              mgr::gContext.profile, false);
                            if (!targetHz)
                                targetHz =
                                    file::config::GetAutoClockHz(HOCCLK_GLOBAL_PROFILE_TID, HocClkModule_Display, mgr::gContext.profile, false);
                        }

                        u8 maxDisplay = targetHz ? (u8)targetHz : 60;
                        u8 minDisplay = board::GetConsoleType() == HocClkConsoleType_Aula ? 45 : 40;

                        if (maxDisplay != minDisplay) {
                            if (fps >= minDisplay && fps <= maxDisplay) {
                                board::SetHz(HocClkModule_Display, fps);
                                mgr::gContext.freqs[HocClkModule_Display] = fps;
                                mgr::gContext.realFreqs[HocClkModule_Display] = fps;
                                mgr::gContext.stable.freqs[HocClkModule_Display] = fps;
                                mgr::gContext.stable.realFreqs[HocClkModule_Display] = fps;
                            } else {
                                for (u32 i = 0; i < 10; i++) {
                                    u32 compareHz = fps * i;
                                    if (compareHz >= minDisplay && compareHz <= maxDisplay) {
                                        board::SetHz(HocClkModule_Display, compareHz);
                                        mgr::gContext.freqs[HocClkModule_Display] = compareHz;
                                        mgr::gContext.realFreqs[HocClkModule_Display] = compareHz;
                                        mgr::gContext.stable.freqs[HocClkModule_Display] = compareHz;
                                        mgr::gContext.stable.realFreqs[HocClkModule_Display] = compareHz;
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

            svcSleepThread(pollNs);
        }
    }

    void StartThreads() {
        threadCreate(&governorTHREAD, GovernorThread, nullptr, NULL, 0x2000, 0x3F, -2);
        threadStart(&governorTHREAD);
    }

    void ExitThreads() {
        threadClose(&governorTHREAD);
    }
}  // namespace mgr
