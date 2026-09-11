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

#include <crc32.h>
#include <cstdio>
#include <cstring>
#include <i2c.h>

#include "../board/board.hpp"
#include "../bpmp/bpmp.hpp"
#include "../display/aula.hpp"
#include "../display/display_refresh_rate.hpp"
#include "../file/config.hpp"
#include "../file/errors.hpp"
#include "../file/file_utils.hpp"
#include "../file/kip.hpp"
#include "../hos/integrations.hpp"
#include "../hos/process_management.hpp"
#include "../i2c/i2cDrv.h"
#include "../ipc/ipc_service.hpp"
#include "../soc/gm20b.hpp"
#include "../util/lockable_mutex.h"
#include "clock_manager.hpp"
#include "governor.hpp"

#define HOSPPC_HAS_BOOST (hosversionAtLeast(7, 0, 0))

namespace mgr {

    bool gRunning = false;
    LockableMutex gContextMutex;
    HocClkContext gContext = {};
    FreqTable gFreqTable[HocClkModule_EnumMax];
    /* Hack: Fixes emc 64 lut freq list hack edge case. */
    /* The clock in the config may differ from the actual max clock, so it's read out from the kip before migration or any other potential overwrite. */
    u32 patchedEmcMaxClock = {};
    std::uint64_t gLastTempLogNs = 0;
    std::uint64_t gLastFreqLogNs = 0;
    std::uint64_t gLastPowerLogNs = 0;
    std::uint64_t gLastCsvWriteNs = 0;

    bool IsAssignableHz(HocClkModule module, std::uint32_t hz) {
        switch (module) {
            case HocClkModule_CPU:
                return hz >= 500000000;
            case HocClkModule_MEM:
                return hz >= 665600000;
            default:
                return true;
        }
    }

    std::uint32_t GetMaxAllowedHz(HocClkModule module, HocClkProfile profile) {
        if (file::config::GetConfigValue(HocClkConfigValue_UncappedClocks)) {
            return ~0;  // Integer limit, uncapped clocks ON
        } else {
            if (module == HocClkModule_GPU) {
                if (profile < HocClkProfile_HandheldCharging) {
                    switch (board::GetSocType()) {
                        case HocClkSocType_Erista:
                            return 460800000;
                        case HocClkSocType_Mariko:
                            if (board::GetConsoleType() == HocClkConsoleType_Hoag) {
                                switch (file::config::GetConfigValue(KipConfigValue_marikoGpuUV)) {
                                    case 0 ... 2:
                                        return 614400000;
                                    case 3 ... 4:
                                        return 768000000;
                                    default:
                                        return 614400000;
                                }
                            } else {
                                switch (file::config::GetConfigValue(KipConfigValue_marikoGpuUV)) {
                                    case 0:
                                        return 614400000;
                                    case 1:
                                        return 691200000;
                                    case 2:
                                        return 768000000;
                                    case 3:
                                        return 844800000;
                                    case 4:
                                        return 921600000;
                                    default:
                                        return 614400000;
                                }
                            }
                        default:
                            return 460800000;
                    }
                } else if (profile <= HocClkProfile_HandheldChargingUSB) {
                    switch (board::GetSocType()) {
                        case HocClkSocType_Erista:
                            return 768000000;
                        case HocClkSocType_Mariko:
                            switch (file::config::GetConfigValue(KipConfigValue_marikoGpuUV)) {
                                case 0:
                                    return 844800000;
                                case 1:
                                    return 921600000;
                                case 2:
                                    return 998400000;
                                case 3:
                                    return 1075200000;
                                case 4:
                                    return 1152000000;
                                default:
                                    return 844800000;
                            }
                        default:
                            return 768000000;
                    }
                }
            } else if (module == HocClkModule_CPU) {
                if (profile < HocClkProfile_HandheldCharging && board::GetSocType() == HocClkSocType_Erista) {
                    return 1581000000;
                } else {
                    return ~0;
                }
            }
        }
        return 0;
    }

    std::uint32_t GetNearestHz(HocClkModule module, std::uint32_t inHz, std::uint32_t maxHz) {
        std::uint32_t *freqs = &gFreqTable[module].list[0];
        size_t count = gFreqTable[module].count - 1;

        size_t i = 0;
        while (i < count) {
            if (maxHz > 0 && freqs[i] >= maxHz) {
                break;
            }
            if (inHz <= ((std::uint64_t)freqs[i] + freqs[i + 1]) / 2) {
                break;
            }
            i++;
        }

        return freqs[i];
    }

    void ResetToStockClocks() {
        board::ResetToStockCpu();
        if (file::config::GetConfigValue(HocClkConfigValue_LiveCpuUv)) {
            if (board::GetSocType() == HocClkSocType_Erista)
                board::SetDfllTunings(file::config::GetConfigValue(KipConfigValue_eristaCpuUV), 0, 1581000000);
            else
                board::SetDfllTunings(file::config::GetConfigValue(KipConfigValue_marikoCpuUVLow), file::config::GetConfigValue(KipConfigValue_marikoCpuUVHigh),
                                      board::CalculateTbreak(file::config::GetConfigValue(KipConfigValue_tableConf)));
        }

        board::ResetToStockGpu();
    }

    bool ConfigIntervalTimeout(HocClkConfigValue intervalMsConfigValue, std::uint64_t ns, std::uint64_t *lastLogNs) {
        std::uint64_t logInterval = file::config::GetConfigValue(intervalMsConfigValue) * 1000000ULL;
        bool shouldLog = logInterval && ((ns - *lastLogNs) > logInterval);

        if (shouldLog) {
            *lastLogNs = ns;
        }

        return shouldLog;
    }

    void RefreshFreqTableRow(HocClkModule module) {
        std::scoped_lock lock{ gContextMutex };

        std::uint32_t freqs[HOCCLK_FREQ_LIST_MAX];
        std::uint32_t count;

        file::utils::LogLine("[mgr] %s freq list refresh", board::GetModuleName(module, true));
        board::GetFreqList(module, &freqs[0], HOCCLK_FREQ_LIST_MAX, &count);

        std::uint32_t *hz = &gFreqTable[module].list[0];
        gFreqTable[module].count = 0;

        if (module == HocClkModule_GPU && board::GetSocType() == HocClkSocType_Mariko &&
            file::config::GetConfigValue(HocClkConfigValue_MarikoMiddleFreqs)) {
            constexpr u32 kStep = 38400000;
            constexpr u32 kPcvStep = 76800000;
            u32 kMax = ~0;
            for (u32 i = 0; i < count; i++) {
                for (u32 j = 0; j < count; j++) {
                    if (freqs[j] + kStep == freqs[i]) {
                        if (freqs[j] < kMax)
                            kMax = freqs[j];
                        break;
                    }
                }
            }
            if (kMax == ~0u) {
                kMax = 0;
                for (u32 i = 0; i < count; i++) {
                    if (freqs[i] > kMax)
                        kMax = freqs[i];
                }
            }

            board::SetMarikoGm20bCutoff(kMax);

            for (u32 f = kPcvStep; f <= kMax && gFreqTable[module].count < HOCCLK_FREQ_LIST_MAX; f += kStep) {
                if (f % kPcvStep != 0) {
                    *hz = f;
                    gFreqTable[module].count++;
                    hz++;
                } else {
                    for (u32 i = 0; i < count; i++) {
                        if (freqs[i] == f) {
                            *hz = f;
                            gFreqTable[module].count++;
                            hz++;
                            break;
                        }
                    }
                }
            }

            for (u32 i = 0; i < count && gFreqTable[module].count < HOCCLK_FREQ_LIST_MAX; i++) {
                if (freqs[i] > kMax && IsAssignableHz(module, freqs[i])) {
                    *hz = freqs[i];
                    gFreqTable[module].count++;
                    hz++;
                }
            }
            return;
        }

        if (module == HocClkModule_GPU && board::GetSocType() == HocClkSocType_Mariko)
            board::SetMarikoGm20bCutoff(0);

        for (std::uint32_t i = 0; i < count; i++) {
            if (!IsAssignableHz(module, freqs[i])) {
                continue;
            }

            // Workaround for PCV bug involving 38.4mhz step rate on erista
            if (module == HocClkModule_GPU && board::GetSocType() == HocClkSocType_Erista) {
                static const struct {
                    u32 hz;
                    HocClkConfigValue kval;
                } eristaGpuVoltMap[] = {
                    { 76800000, KipConfigValue_g_volt_e_76800 },     { 115200000, KipConfigValue_g_volt_e_115200 },
                    { 153600000, KipConfigValue_g_volt_e_153600 },   { 192000000, KipConfigValue_g_volt_e_192000 },
                    { 230400000, KipConfigValue_g_volt_e_230400 },   { 268800000, KipConfigValue_g_volt_e_268800 },
                    { 307200000, KipConfigValue_g_volt_e_307200 },   { 345600000, KipConfigValue_g_volt_e_345600 },
                    { 384000000, KipConfigValue_g_volt_e_384000 },   { 422400000, KipConfigValue_g_volt_e_422400 },
                    { 460800000, KipConfigValue_g_volt_e_460800 },   { 499200000, KipConfigValue_g_volt_e_499200 },
                    { 537600000, KipConfigValue_g_volt_e_537600 },   { 576000000, KipConfigValue_g_volt_e_576000 },
                    { 614400000, KipConfigValue_g_volt_e_614400 },   { 652800000, KipConfigValue_g_volt_e_652800 },
                    { 691200000, KipConfigValue_g_volt_e_691200 },   { 729600000, KipConfigValue_g_volt_e_729600 },
                    { 768000000, KipConfigValue_g_volt_e_768000 },   { 806400000, KipConfigValue_g_volt_e_806400 },
                    { 844800000, KipConfigValue_g_volt_e_844800 },   { 883200000, KipConfigValue_g_volt_e_883200 },
                    { 921600000, KipConfigValue_g_volt_e_921600 },   { 960000000, KipConfigValue_g_volt_e_960000 },
                    { 998400000, KipConfigValue_g_volt_e_998400 },   { 1036800000, KipConfigValue_g_volt_e_1036800 },
                    { 1075200000, KipConfigValue_g_volt_e_1075200 },
                };
                bool skip = false;
                for (auto &entry : eristaGpuVoltMap) {
                    if (entry.hz == freqs[i]) {
                        if (file::config::GetConfigValue(entry.kval) == 2000) {
                            skip = true;
                        }
                        break;
                    }
                }
                if (skip)
                    continue;
            }

            *hz = freqs[i];
            file::utils::LogLine("[mgr] %02u - %u - %u.%u MHz", gFreqTable[module].count, *hz, *hz / 1000000, *hz / 100000 - *hz / 1000000 * 10);

            gFreqTable[module].count++;
            hz++;
        }

        /* Since it is a pain to patch the vtables in ipc we can just hack the freqs in. You can still set them. */
        constexpr u64 EmcClkOSLimitHz = 1600000ULL * 1000;  // 1600 MHz
        const u64 maxHz = patchedEmcMaxClock * 1000;
        if (module == HocClkModule_MEM && board::GetSocType() == HocClkSocType_Mariko &&
            file::kip::kipAvailable && maxHz >= EmcClkOSLimitHz &&
            file::config::GetConfigValue(KipConfigValue_stepMode) == 4 /* 33 MHz */) {

            /* Drop the clkrst entries above the OS limit */
            u32 kept = 0;
            for (u32 i = 0; i < gFreqTable[module].count; ++i) {
                if (static_cast<u64>(gFreqTable[module].list[i]) <= EmcClkOSLimitHz) {
                    gFreqTable[module].list[kept++] = gFreqTable[module].list[i];
                }
            }
            gFreqTable[module].count = kept;

            auto push = [&](u64 freqHz) {
                if (freqHz > maxHz || gFreqTable[module].count >= HOCCLK_FREQ_LIST_MAX) {
                    return;
                }
                gFreqTable[module].list[gFreqTable[module].count++] = static_cast<u32>(freqHz);
            };

            static const u32 stepFreqs33[] = {
                1633000, 1666000, 1700000, 1733000, 1766000, 1800000, 1833000, 1866000, 1900000, 1933000,
                1966000, 2000000, 2033000, 2066000, 2100000, 2133000, 2166000, 2200000, 2233000, 2266000,
                2300000, 2333000, 2366000, 2400000, 2433000, 2466000, 2500000, 2533000, 2566000, 2600000,
                2633000, 2666000, 2700000, 2733000, 2766000, 2800000, 2833000, 2866000, 2900000, 2933000,
                2966000, 3000000, 3033000, 3066000, 3100000, 3133000, 3166000, 3200000, 3233000, 3266000,
                3300000, 3333000, 3366000, 3400000, 3433000, 3466000, 3500000,
            };
            for (u32 f : stepFreqs33) {
                push(static_cast<u64>(f) * 1000);
            }

            if (gFreqTable[module].count == 0 ||
                static_cast<u64>(gFreqTable[module].list[gFreqTable[module].count - 1]) != maxHz) {
                push(maxHz);
            }
        }

        file::utils::LogLine("[mgr] count = %u", gFreqTable[module].count);
    }

    bool HandleSafetyFeatures(bool isBoost) {
        if (((tmp451TempSoc() / 1000) > (int)file::config::GetConfigValue(HocClkConfigValue_ThermalThrottleThreshold)) &&
            file::config::GetConfigValue(HocClkConfigValue_ThermalThrottle)) {
            ResetToStockClocks();
            return true;
        }

        return false;
    }
    void HandleMiscFeatures() {

        // these dont need to run that often, so dont bother
        static u32 tick = 0;
        if (++tick > 10) {
            tick = 0;

            if (file::config::GetConfigValue(HocClkConfigValue_BatteryChargeCurrent)) {
                I2c_Bq24193_SetFastChargeCurrentLimit(file::config::GetConfigValue(HocClkConfigValue_BatteryChargeCurrent));
            }

            if (file::config::GetConfigValue(HocClkConfigValue_InputCurrentLimit)) {
                I2c_Bq24193_SetInputCurrentLimit(file::config::GetConfigValue(HocClkConfigValue_InputCurrentLimit));
            }

            I2c_BuckConverter_SetMvOut(&I2c_Display, file::config::GetConfigValue(HocClkConfigValue_DisplayVoltage));

            if (board::GetConsoleType() == HocClkConsoleType_Aula)
                display::SetDisplayColorMode((AulaColorMode)file::config::GetConfigValue(HocClkConfigValue_AulaDisplayColorPreset));
            if (file::config::GetConfigValue(HocClkConfigValue_LiveCpuUv)) {
                board::HandleCpuUv();
            }
        }
    }

    u32 ClampGpuVoltage(u32 voltage) {
        static const u32 maxGpuVoltage = board::GetSocType() == HocClkSocType_Mariko ? 960 : 995;
        return std::min(voltage, maxGpuVoltage);
    }

    u32 GetCurrentNearestFrequency(HocClkModule module) {
        /* Target freq may not match actual frequency so don't even bother with that. */
        u32 hz = board::GetHz(module);
        u32 maxHz = GetMaxAllowedHz(module, gContext.profile);
        return GetNearestHz(module, hz, maxHz);
    }

    u32 GetNearestOverrideHz(HocClkModule module) {
        u32 targetHz = gContext.overrideFreqs[module];
        if (!targetHz) {
            targetHz = file::config::GetAutoClockHz(gContext.applicationId, module, gContext.profile, false);
            if (!targetHz) {
                targetHz = file::config::GetAutoClockHz(HOCCLK_GLOBAL_PROFILE_TID, module, gContext.profile, false);
            }
        }

        if (targetHz) {
            targetHz = GetNearestHz(module, targetHz, GetMaxAllowedHz(module, gContext.profile));
        }

        return targetHz;
    }

    void ApplyGpuFreqVoltRequest(u32 voltage, u32 hz) {
        /* Apply nothing with disabled voltage. */
        constexpr u32 DisabledVoltage = 2000;
        if (voltage == DisabledVoltage) {
            hos::WriteNotification("Horizon OC\nDeactivated frequency.\nReboot to apply.");
            return;
        }

        voltage = ClampGpuVoltage(voltage);
        u32 currentFreq = GetCurrentNearestFrequency(HocClkModule_GPU);
        /* If not freq was provided, use the current freq. */
        if (hz == 0) {
            hz = currentFreq;
        }

        board::PcvHijackGpuFrequency(voltage, hz);
        /* Update the voltage using the currently nearest valid gpu frequency. */
        board::SetHz(HocClkModule_GPU, currentFreq);
    }

    void ApplyGpuDvfs(u32 targetHz) {
        s32 dvfsOffset = file::config::GetConfigValue(HocClkConfigValue_DVFSOffset);
        dvfsOffset = std::max(dvfsOffset, -80);
        u32 vmin = board::GetMinimumGpuVmin(targetHz / 1000000, board::GetGpuSpeedoBracket());

        if (vmin) {
            vmin += dvfsOffset;
        }

        /* Prevent console from combusting if for some reason bad shit happens :P */
        vmin = ClampGpuVoltage(vmin);

        /* Hijack gpu volt table. */
        board::PcvHijackGpuVolts(vmin);

        /* Update gpu frequency to actually use the voltage. */
        if (targetHz) {
            board::SetHz(HocClkModule_GPU, GetCurrentNearestFrequency(HocClkModule_GPU));
        } else {
            /* If the target frequency is zero, we reset the frequency to ensure it gets updated even without any frequency override. */
            board::ResetToStockGpu();
        }
    }

    void DVFSReset() {
        if (file::config::GetConfigValue(HocClkConfigValue_DVFSMode) == DVFSMode_Hijack) {
            board::PcvHijackGpuVolts(0);  // Reset to vMin

            u32 targetHz = GetNearestOverrideHz(HocClkModule_GPU);

            board::ResetToStockGpu();
            if (targetHz)
                board::SetHz(HocClkModule_GPU, targetHz);
        }
    }

    void HandleFreqReset(HocClkModule module, bool isBoost, bool didHijackPcv) {
        switch (module) {
            case HocClkModule_CPU:
                if (!(isBoost || (file::config::GetConfigValue(HocClkConfigValue_OverwriteBoostMode) && isBoost)))
                    board::ResetToStockCpu();
                if (file::config::GetConfigValue(HocClkConfigValue_LiveCpuUv)) {
                    if (board::GetSocType() == HocClkSocType_Erista)
                        board::SetDfllTunings(file::config::GetConfigValue(KipConfigValue_eristaCpuUV), 0, 1581000000);
                    else
                        board::SetDfllTunings(file::config::GetConfigValue(KipConfigValue_marikoCpuUVLow),
                                              file::config::GetConfigValue(KipConfigValue_marikoCpuUVHigh),
                                              board::CalculateTbreak(file::config::GetConfigValue(KipConfigValue_tableConf)));
                }
                break;
            case HocClkModule_GPU:
                board::ResetToStockGpu();
                break;
            case HocClkModule_MEM:
                board::ResetToStockMem();
                if (!didHijackPcv) {
                    DVFSReset();
                    didHijackPcv = true;
                }
                break;
            case HocClkModule_Display:
                if (file::config::GetConfigValue(HocClkConfigValue_OverwriteRefreshRate)) {
                    board::ResetToStockDisplay();
                }
                break;
            default:
                break;
        }
    }

    /* Memory frequency gets reset when starting a game during boost mode, this reapplies it. */
    void GameStartMemWar() {
        u32 targetRamHz = GetNearestOverrideHz(HocClkModule_MEM);
        if (!targetRamHz) {
            return;
        }

        u32 nearestFreq = GetCurrentNearestFrequency(HocClkModule_MEM);

        if (targetRamHz != nearestFreq) {
            if (file::config::GetConfigValue(HocClkConfigValue_DVFSMode) == DVFSMode_Hijack) {
                ApplyGpuDvfs(targetRamHz);
            }

            board::SetHz(HocClkModule_MEM, targetRamHz);
        }
    }

    void SetClocks(bool isBoost) {
        std::uint32_t targetHz = 0;
        std::uint32_t maxHz = 0;
        std::uint32_t nearestHz = 0;
        static bool prepareBoostExit = false;

        bool didHijackPcv = false;
        bool skipCpuDueToBoost = isBoost && !file::config::GetConfigValue(HocClkConfigValue_OverwriteBoostMode);
        if (skipCpuDueToBoost) {
            board::SetHz(HocClkModule_CPU, board::GetHz(HocClkModule_CPU));
            prepareBoostExit = true;
            GameStartMemWar();
            return;  // Return if we aren't overwriting boost mode
        }

        if (prepareBoostExit) {
            board::SetHz(HocClkModule_CPU, board::GetHz(HocClkModule_CPU));
            prepareBoostExit = false;
        }

        u32 ramTargetHz = GetNearestOverrideHz(HocClkModule_MEM);

        bool returnRaw = false;  // Return a value scaled to MHz instead of raw value
        for (unsigned int module = 0; module < HocClkModule_EnumMax; module++) {
            u32 oldHz = board::GetHz((HocClkModule)module);  // Get Old hz (used primarily for DVFS Logic)

            if (module > HocClkModule_MEM)
                returnRaw = true;
            else
                returnRaw = false;
            targetHz = gContext.overrideFreqs[module];
            if (!targetHz) {
                targetHz = file::config::GetAutoClockHz(gContext.applicationId, (HocClkModule)module, gContext.profile, returnRaw);
                if (!targetHz)
                    targetHz = file::config::GetAutoClockHz(HOCCLK_GLOBAL_PROFILE_TID, (HocClkModule)module, gContext.profile, returnRaw);
            }

            if (module == HocClkModule_Governor) {
                mgr::HandleGovernor(targetHz);
            }

            bool noCPU = mgr::isCpuGovernorEnabled;
            bool noGPU = mgr::isGpuGovernorEnabled;
            bool noDisp = mgr::isVRREnabled;
            if (noDisp && module == HocClkModule_Display)
                continue;

            if (module == HocClkModule_Display && file::config::GetConfigValue(HocClkConfigValue_OverwriteRefreshRate) && !noDisp) {
                if (targetHz) {
                    board::SetHz(HocClkModule_Display, targetHz);
                    gContext.freqs[HocClkModule_Display] = targetHz;
                    gContext.realFreqs[HocClkModule_Display] = targetHz;

                    gContext.stable.freqs[HocClkModule_Display] = targetHz;
                    gContext.stable.realFreqs[HocClkModule_Display] = targetHz;
                } else {
                    HandleFreqReset(HocClkModule_Display, isBoost, didHijackPcv);
                }
            }

            // The modules above MEM require special handling
            if (module > HocClkModule_MEM) {
                continue;
            }

            if ((skipCpuDueToBoost || noCPU) && module == HocClkModule_CPU)
                continue;
            if (noGPU && module == HocClkModule_GPU)
                continue;

            u32 autoCpuOcHz = 0;
            if (module == HocClkModule_CPU && file::config::GetConfigValue(HocClkConfigValue_AutoRAMCPUOverclock) && !isBoost &&
                !mgr::isCpuGovernorEnabled && (board::GetSocType() == HocClkSocType_Mariko)) {
                u32 threshold = (u32)file::config::GetConfigValue(HocClkConfigValue_AutoRamCpuRamOCThreshold) * 1000;
                if (ramTargetHz >= threshold)
                    autoCpuOcHz = (u32)file::config::GetConfigValue(HocClkConfigValue_AutoRamCpuCpuOCFreq) * 1000;
            }

            if (targetHz || autoCpuOcHz) {
                maxHz = GetMaxAllowedHz((HocClkModule)module, gContext.profile);
                nearestHz = targetHz ? GetNearestHz((HocClkModule)module, targetHz, maxHz) : 0;

                if (autoCpuOcHz > nearestHz)
                    nearestHz = GetNearestHz(HocClkModule_CPU, autoCpuOcHz, maxHz);

                if (nearestHz != gContext.freqs[module]) {
                    file::utils::LogLine("[mgr] %s clock set : %u.%u MHz (target = %u.%u MHz)", board::GetModuleName((HocClkModule)module, true),
                                       nearestHz / 1000000, nearestHz / 100000 - nearestHz / 1000000 * 10, targetHz / 1000000,
                                       targetHz / 100000 - targetHz / 1000000 * 10);

                    // The logic MUST be done in this order otherwise you WILL get crashes
                    if (module == HocClkModule_MEM && targetHz > oldHz && file::config::GetConfigValue(HocClkConfigValue_DVFSMode) == DVFSMode_Hijack) {
                        ApplyGpuDvfs(targetHz);
                    }
                    board::SetHz((HocClkModule)module, nearestHz);
                    gContext.freqs[module] = nearestHz;

                    if (module < HocClkModuleStable_EnumMax) {
                        gContext.stable.freqs[module] = nearestHz;
                    }

                    if (module == HocClkModule_MEM && targetHz < oldHz && file::config::GetConfigValue(HocClkConfigValue_DVFSMode) == DVFSMode_Hijack) {
                        ApplyGpuDvfs(targetHz);
                    }

                    if (module == HocClkModule_MEM && file::config::GetConfigValue(HocClkConfigValue_DVFSMode) == DVFSMode_Hijack)
                        didHijackPcv = false;
                }
            } else {
                HandleFreqReset((HocClkModule)module, isBoost, didHijackPcv);
            }
        }
    }

    bool RefreshContext() {
        bool hasChanged = false;

        gContext.bpmpInfo = *bpmp::GetSharedInfo();

        std::uint32_t mode = 0;
        Result rc = apmExtGetCurrentPerformanceConfiguration(&mode);
        ASSERT_RESULT_OK(rc, "apmExtGetCurrentPerformanceConfiguration");

        std::uint64_t applicationId = hos::GetCurrentApplicationId();
        if (applicationId != gContext.applicationId) {
            file::utils::LogLine("[mgr] TitleID change: %016lX", applicationId);
            gContext.applicationId = applicationId;
            hasChanged = true;
        }

        HocClkProfile profile = board::GetProfile();
        if (profile != gContext.profile) {
            file::utils::LogLine("[mgr] Profile change: %s", board::GetProfileName(profile, true));
            gContext.profile = profile;
            hasChanged = true;
        }

        // restore clocks to stock values on app or profile change
        if (hasChanged) {
            board::ResetToStock();
            if (file::config::GetConfigValue(HocClkConfigValue_DVFSMode) == DVFSMode_Hijack) {
                board::PcvHijackGpuVolts(0);
                board::ResetToStockGpu();
            }
            WaitForNextTick();
        }

        std::uint32_t hz = 0;
        for (unsigned int module = 0; module < HocClkModule_EnumMax; module++) {
            hz = board::GetHz((HocClkModule)module);
            if (hz != 0 && hz != gContext.freqs[module]) {
                file::utils::LogLine("[mgr] %s clock change: %u.%u MHz", board::GetModuleName((HocClkModule)module, true), hz / 1000000,
                                   hz / 100000 - hz / 1000000 * 10);
                gContext.freqs[module] = hz;

                if (module < HocClkModuleStable_EnumMax) {
                    gContext.stable.freqs[module] = hz;
                }
                hasChanged = true;
            }

            hz = file::config::GetOverrideHz((HocClkModule)module);
            if (hz != gContext.overrideFreqs[module]) {
                if (hz) {
                    file::utils::LogLine("[mgr] %s override change: %u.%u MHz", board::GetModuleName((HocClkModule)module, true), hz / 1000000,
                                       hz / 100000 - hz / 1000000 * 10);
                }
                gContext.overrideFreqs[module] = hz;

                if (module < HocClkModuleStable_EnumMax) {
                    gContext.stable.overrideFreqs[module] = hz;
                }
                hasChanged = true;
            }
        }

        std::uint64_t ns = armTicksToNs(armGetSystemTick());

        // temperatures do not and should not force a refresh, hasChanged untouched
        std::uint32_t millis = 0;
        bool shouldLogTemp = ConfigIntervalTimeout(HocClkConfigValue_TempLogIntervalMs, ns, &gLastTempLogNs);
        for (unsigned int sensor = 0; sensor < HocClkThermalSensor_EnumMax; sensor++) {
            millis = board::GetTemperatureMilli((HocClkThermalSensor)sensor);
            if (shouldLogTemp) {
                file::utils::LogLine("[mgr] %s temp: %u.%u °C", board::GetThermalSensorName((HocClkThermalSensor)sensor, true), millis / 1000,
                                   (millis - millis / 1000 * 1000) / 100);
            }
            gContext.temps[sensor] = millis;

            if (sensor < HocClkThermalSensorStable_EnumMax) {
                gContext.stable.temps[sensor] = millis;
            }
        }

        // power stats do not and should not force a refresh, hasChanged untouched
        std::int32_t mw = 0;
        bool shouldLogPower = ConfigIntervalTimeout(HocClkConfigValue_PowerLogIntervalMs, ns, &gLastPowerLogNs);
        for (unsigned int sensor = 0; sensor < HocClkPowerSensor_EnumMax; sensor++) {
            mw = board::GetPowerMw((HocClkPowerSensor)sensor);
            if (shouldLogPower) {
                file::utils::LogLine("[mgr] Power %s: %d mW", board::GetPowerSensorName((HocClkPowerSensor)sensor, false), mw);
            }
            gContext.power[sensor] = mw;

            if (sensor < HocClkPowerSensorStable_EnumMax) {
                gContext.stable.power[sensor] = mw;
            }
        }

        // real freqs do not and should not force a refresh, hasChanged untouched
        std::uint32_t realHz = 0;
        bool shouldLogFreq = ConfigIntervalTimeout(HocClkConfigValue_FreqLogIntervalMs, ns, &gLastFreqLogNs);
        for (unsigned int module = 0; module < HocClkModule_EnumMax; module++) {
            realHz = board::GetRealHz((HocClkModule)module);
            if (shouldLogFreq) {
                file::utils::LogLine("[mgr] %s real freq: %u.%u MHz", board::GetModuleName((HocClkModule)module, true), realHz / 1000000,
                                   realHz / 100000 - realHz / 1000000 * 10);
            }
            gContext.realFreqs[module] = realHz;

            if (module < HocClkModuleStable_EnumMax) {
                gContext.stable.realFreqs[module] = realHz;
            }
        }

        // ram load do not and should not force a refresh, hasChanged untouched
        for (unsigned int loadSource = 0; loadSource < HocClkPartLoad_EnumMax; loadSource++) {
            gContext.partLoad[loadSource] = board::GetPartLoad((HocClkPartLoad)loadSource);

            if (loadSource < HocClkPartLoadStable_EnumMax) {
                gContext.stable.partLoad[loadSource] = board::GetPartLoad((HocClkPartLoad)loadSource);
            }
        }

        for (unsigned int voltageSource = 0; voltageSource < HocClkVoltage_EnumMax; voltageSource++) {
            gContext.voltages[voltageSource] = board::GetVoltage((HocClkVoltage)voltageSource);

            if (voltageSource < HocClkVoltageStable_EnumMax) {
                gContext.stable.voltages[voltageSource] = board::GetVoltage((HocClkVoltage)voltageSource);
            }
        }

        if (ConfigIntervalTimeout(HocClkConfigValue_CsvWriteIntervalMs, ns, &gLastCsvWriteNs)) {
            file::utils::WriteContextToCsv(&gContext);
        }

        // this->context->maxDisplayFreq = board::GetHighestDockedDisplayRate();
        u32 targetHz = gContext.overrideFreqs[HocClkModule_Display];
        if (!targetHz) {
            targetHz = file::config::GetAutoClockHz(gContext.applicationId, HocClkModule_Display, gContext.profile, true);
            if (!targetHz)
                targetHz = file::config::GetAutoClockHz(HOCCLK_GLOBAL_PROFILE_TID, HocClkModule_Display, gContext.profile, true);
        }

        if (board::GetConsoleType() != HocClkConsoleType_Hoag)
            board::SetDisplayRefreshDockedState(gContext.profile == HocClkProfile_Docked);

        if (gContext.isSaltyNXInstalled)
            gContext.fps = hos::GetSaltyNXFPS();
        else
            gContext.fps = 254;  // N/A

        if (gContext.isSaltyNXInstalled)
            gContext.resolutionHeight = hos::GetSaltyNXResolutionHeight();
        else
            gContext.resolutionHeight = 0;  // N/A

        return hasChanged;
    }

    void Initialize() {
        gContext = {};
        gContext.applicationId = 0;
        gContext.profile = HocClkProfile_Handheld;

        /* Load the KIP customize table before building the freq tables: the MEM freq-list
           synthesis in RefreshFreqTableRow reads marikoEmcMaxClock / stepMode from it. */
        file::kip::GetKipData();

        for (unsigned int module = 0; module < HocClkModule_EnumMax; module++) {
            gContext.freqs[module] = 0;
            gContext.realFreqs[module] = 0;
            gContext.overrideFreqs[module] = 0;

            if (module < HocClkModuleStable_EnumMax) {
                gContext.stable.freqs[module] = 0;
                gContext.stable.realFreqs[module] = 0;
                gContext.stable.overrideFreqs[module] = 0;
            }

            RefreshFreqTableRow((HocClkModule)module);
        }

        gRunning = false;
        gLastTempLogNs = 0;
        gLastCsvWriteNs = 0;

        board::FuseData *fuse = board::GetFuseData();

        gContext.speedos[HocClkSpeedo_CPU] = fuse->cpuSpeedo;
        gContext.speedos[HocClkSpeedo_GPU] = fuse->gpuSpeedo;
        gContext.speedos[HocClkSpeedo_SOC] = fuse->socSpeedo;
        gContext.iddq[HocClkSpeedo_CPU] = fuse->cpuIDDQ;
        gContext.iddq[HocClkSpeedo_GPU] = fuse->gpuIDDQ;
        gContext.iddq[HocClkSpeedo_SOC] = fuse->socIDDQ;
        gContext.waferX = fuse->waferX;
        gContext.waferY = fuse->waferY;

        gContext.dramID = board::GetDramID();
        gContext.isDram8GB = board::IsDram8GB();
        gContext.consoleType = board::GetConsoleType();
        gContext.isFirstLoad = file::config::GetConfigValue(HocClkConfigValue_IsFirstLoad);

        board::SetGpuSchedulingMode((GpuSchedulingMode)file::config::GetConfigValue(HocClkConfigValue_GPUScheduling),
                                    (GpuSchedulingOverrideMethod)file::config::GetConfigValue(HocClkConfigValue_GPUSchedulingMethod));
        gContext.gpuSchedulingMode = (GpuSchedulingMode)file::config::GetConfigValue(HocClkConfigValue_GPUScheduling);

        gContext.isSysDockInstalled = hos::GetSysDockState();
        gContext.isSaltyNXInstalled = hos::GetSaltyNXState();
        if (gContext.isSaltyNXInstalled) {
            hos::LoadSaltyNX();
        }

        mgr::StartThreads();
    }

    void Exit() {
        mgr::ExitThreads();
    }

    HocClkContext GetCurrentContext() {
        std::scoped_lock lock{ gContextMutex };
        return gContext;
    }

    void SetRunning(bool running) {
        gRunning = running;
    }

    bool Running() {
        return gRunning;
    }

    void GetFreqList(HocClkModule module, std::uint32_t *list, std::uint32_t maxCount, std::uint32_t *outCount) {
        ASSERT_ENUM_VALID(HocClkModule, module);

        *outCount = std::min(maxCount, gFreqTable[module].count);
        memcpy(list, &gFreqTable[module].list[0], *outCount * sizeof(gFreqTable[0].list[0]));
    }

    void Tick() {
        std::scoped_lock lock{ gContextMutex };
        std::uint32_t mode = 0;
        Result rc = apmExtGetCurrentPerformanceConfiguration(&mode);
        ASSERT_RESULT_OK(rc, "apmExtGetCurrentPerformanceConfiguration");

        bool isBoost = apmExtIsBoostMode(mode);

        bool shouldSkipClockSet = HandleSafetyFeatures(isBoost);
        HandleMiscFeatures();

        // GPU clock should always be the same unless PCV has overwriten our change, so reset it
        if ((RefreshContext() || file::config::Refresh() || (board::GetRealHz(HocClkModule_GPU) != gContext.freqs[HocClkModule_GPU])) &&
            !shouldSkipClockSet) {
            SetClocks(isBoost);
        }
    }

    void WaitForNextTick() {
        if (!bpmp::IsAwake()) {
            svcSleepThread(250 * 1000000ULL);  // 250ms
            return;
        }

        if (board::GetHz(HocClkModule_MEM) > 665000000)
            svcSleepThread(file::config::GetConfigValue(HocClkConfigValue_PollingIntervalMs) * 1000000ULL);
        else
            svcSleepThread(5000 * 1000000ULL);  // 5 seconds in sleep mode
    }
}  // namespace mgr
