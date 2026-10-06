/*
 * Copyright (c) Souldbminer, Lightos_ and Horizon OC Contributors
 *
 * Copyright (c) B3711
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

#include <battery.h>
#include <cstring>
#include <hocclk.h>
#include <memmem.h>
#include <registers.h>
#include <switch.h>

#include "../file/file_utils.hpp"
#include "../hos/rgltr.h"
#include "../i2c/i2cDrv.h"
#include "board.hpp"
#include "board_freq.hpp"
#include "board_volt.hpp"

namespace board {

    GpuVoltData voltData = {};
    u32 cpuVoltTable[32] = {};  // 32LUT
    u64 cldvfs;
    CpuDfllData cachedTune;

    /* ... This really needs some cleanup... */
    namespace {
        struct EristaCpuUvEntry {
            u32 tune0;
            u32 tune1;
        };

        struct MarikoCpuUvEntry {
            u32 tune0_low;
            u32 tune0_high;
            u32 tune1_low;
            u32 tune1_high;
        };

        EristaCpuUvEntry eristaCpuUvTableLowBracket[6] = {
            // <2118 CPU speedo
            { 0xFFEAD0FF, 0x25501d0 }, { 0xffff, 0x27007ff }, { 0xefff, 0x27407ff },
            { 0xdfff, 0x27807ff },     { 0xdfdf, 0x27a07ff }, { 0xcfdf, 0x37007ff },
        };

        EristaCpuUvEntry eristaCpuUvTableHighBracket[6] = {
            { 0xFFEAD0FF, 0x20091d9 }, { 0xffff, 0x27007ff }, { 0xefff, 0x27407ff },
            { 0xdfff, 0x27807ff },     { 0xdfdf, 0x27a07ff }, { 0xcfdf, 0x37007ff },
        };

        MarikoCpuUvEntry marikoCpuUvLow[12] = {
            { 0xffa0, 0xffff, 0x21107ff, 0 },         { 0x0, 0xffdf, 0x21107ff, 0x27207ff },    { 0xffdf, 0xffdf, 0x21107ff, 0x27307ff },
            { 0xffff, 0xffdf, 0x21107ff, 0x27407ff }, { 0x0, 0xffdf, 0x21607ff, 0x27707ff },    { 0x0, 0xffdf, 0x21607ff, 0x27807ff },
            { 0x0, 0xdfff, 0x21607ff, 0x27b07ff },    { 0xdfff, 0xdfff, 0x21707ff, 0x27b07ff }, { 0xdfff, 0xdfff, 0x21707ff, 0x27c07ff },
            { 0xdfff, 0xdfff, 0x21707ff, 0x27d07ff }, { 0xdfff, 0xdfff, 0x21707ff, 0x27e07ff }, { 0xdfff, 0xdfff, 0x21707ff, 0x27f07ff },
        };

        MarikoCpuUvEntry marikoCpuUvHigh[12] = {
            { 0x0, 0xffff, 0, 0 },         { 0x0, 0xffdf, 0, 0x27207ff }, { 0x0, 0xffdf, 0, 0x27307ff }, { 0x0, 0xffdf, 0, 0x27407ff },
            { 0x0, 0xffdf, 0, 0x27707ff }, { 0x0, 0xffdf, 0, 0x27807ff }, { 0x0, 0xdfff, 0, 0x27b07ff }, { 0x0, 0xdfff, 0, 0x27c07ff },
            { 0x0, 0xdfff, 0, 0x27d07ff }, { 0x0, 0xdfff, 0, 0x27e07ff }, { 0x0, 0xdfff, 0, 0x27f07ff }, { 0x0, 0xdfff, 0, 0x27f07ff },
        };
    }  // namespace

    void CacheDfllData() {
        Result rc = QueryMemoryMapping(&cldvfs, CLDVFS_REGION_BASE, CLDVFS_REGION_SIZE);
        ASSERT_RESULT_OK(rc, "QueryMemoryMapping (cldvfs)");

        if (GetSocType() == HocClkSocType_Erista) {
            cachedTune.tune0Low = *reinterpret_cast<u32 *>(cldvfs + CL_DVFS_TUNE0_0);
            cachedTune.tune1Low = *reinterpret_cast<u32 *>(cldvfs + CL_DVFS_TUNE1_0);
        }
    }

    /* TODO: clean up this code. */
    void SetDfllTunings(u32 levelLow, u32 levelHigh, u32 tbreakPoint) {
        u32 *tune0_ptr = reinterpret_cast<u32 *>(cldvfs + CL_DVFS_TUNE0_0);
        u32 *tune1_ptr = reinterpret_cast<u32 *>(cldvfs + CL_DVFS_TUNE1_0);

        if (GetSocType() == HocClkSocType_Mariko) {
            if (GetHz(HocClkModule_CPU) < tbreakPoint && (levelLow || levelHigh)) {
                if (levelLow) {
                    *tune0_ptr = marikoCpuUvLow[levelLow - 1].tune0_low;
                    *tune1_ptr = marikoCpuUvLow[levelLow - 1].tune1_low;
                }
                return;
            } else {
                if (levelLow) {
                    *tune0_ptr = marikoCpuUvLow[levelLow - 1].tune0_low;
                    *tune1_ptr = marikoCpuUvLow[levelLow - 1].tune1_low;
                }
                if (levelHigh) {
                    *tune0_ptr = marikoCpuUvHigh[levelHigh - 1].tune0_high;
                    *tune1_ptr = marikoCpuUvHigh[levelHigh - 1].tune1_high;
                }
                return;
            }
            if (GetHz(HocClkModule_CPU) < tbreakPoint || (!levelLow)) {  // account for tbreak
                *tune0_ptr = 0xCFFF;
                *tune1_ptr = 0xFF072201;
                return;
            } else if (GetHz(HocClkModule_CPU) >= tbreakPoint || (!levelHigh)) {
                *tune0_ptr = cachedTune.tune0High;  // per console?
                *tune1_ptr = 0xFFF7FF3F;
                return;
            }
        } else {
            *tune0_ptr = fuseData.cpuSpeedo > 2118 ? eristaCpuUvTableHighBracket[levelLow].tune0 : eristaCpuUvTableLowBracket[levelLow].tune0;
            *tune1_ptr = fuseData.cpuSpeedo > 2118 ? eristaCpuUvTableHighBracket[levelLow].tune1 : eristaCpuUvTableLowBracket[levelLow].tune1;
        }
    }

    /*
    enum TableConfig: u32 {
        DEFAULT_TABLE = 1,
        TBREAK_1581 = 2,
        TBREAK_1683 = 3,
        EXTREME_TABLE = 4,
    };
    */
    u32 CalculateTbreak(u32 table) {
        if (GetSocType() == HocClkSocType_Erista) {
            return 1581000000;
        } else {
            switch (table) {
                case 1 ... 2:
                case 4:
                    return 1581000000;
                case 3:
                    return 1683000000;
                default:
                    return 1581000000;
            }
        }
    }

    /*
    * Switch Power domains (max77620):
    * Name  | Usage         | uV step | uV min | uV default | uV max  | Init
    *-------+---------------+---------+--------+------------+---------+------------------
    *  sd0  | SoC           | 12500   | 600000 |  625000    | 1400000 | 1.125V (pkg1.1)
    *  sd1  | SDRAM         | 12500   | 600000 | 1125000    | 1125000 | 1.1V   (pkg1.1)
    *  sd2  | ldo{0-1, 7-8} | 12500   | 600000 | 1325000    | 1350000 | 1.325V (pcv)
    *  sd3  | 1.8V general  | 12500   | 600000 | 1800000    | 1800000 |
    *  ldo0 | Display Panel | 25000   | 800000 | 1200000    | 1200000 | 1.2V   (pkg1.1)
    *  ldo1 | XUSB, PCIE    | 25000   | 800000 | 1050000    | 1050000 | 1.05V  (pcv)
    *  ldo2 | SDMMC1        | 50000   | 800000 | 1800000    | 3300000 |
    *  ldo3 | GC ASIC       | 50000   | 800000 | 3100000    | 3100000 | 3.1V   (pcv)
    *  ldo4 | RTC           | 12500   | 800000 |  850000    |  850000 | 0.85V  (AO, pcv)
    *  ldo5 | GC Card       | 50000   | 800000 | 1800000    | 1800000 | 1.8V   (pcv)
    *  ldo6 | Touch, ALS    | 50000   | 800000 | 2900000    | 2900000 | 2.9V   (pcv)
    *  ldo7 | XUSB          | 50000   | 800000 | 1050000    | 1050000 | 1.05V  (pcv)
    *  ldo8 | XUSB, DP, MCU | 50000   | 800000 | 1050000    | 2800000 | 1.05V/2.8V (pcv)

    typedef enum {
        PcvPowerDomainId_Max77620_Sd0  = 0x3A000080,
        PcvPowerDomainId_Max77620_Sd1  = 0x3A000081, // vdd2
        PcvPowerDomainId_Max77620_Sd2  = 0x3A000082,
        PcvPowerDomainId_Max77620_Sd3  = 0x3A000083,
        PcvPowerDomainId_Max77620_Ldo0 = 0x3A0000A0,
        PcvPowerDomainId_Max77620_Ldo1 = 0x3A0000A1,
        PcvPowerDomainId_Max77620_Ldo2 = 0x3A0000A2,
        PcvPowerDomainId_Max77620_Ldo3 = 0x3A0000A3,
        PcvPowerDomainId_Max77620_Ldo4 = 0x3A0000A4,
        PcvPowerDomainId_Max77620_Ldo5 = 0x3A0000A5,
        PcvPowerDomainId_Max77620_Ldo6 = 0x3A0000A6,
        PcvPowerDomainId_Max77620_Ldo7 = 0x3A0000A7,
        PcvPowerDomainId_Max77620_Ldo8 = 0x3A0000A8,
        PcvPowerDomainId_Max77621_Cpu  = 0x3A000003,
        PcvPowerDomainId_Max77621_Gpu  = 0x3A000004,
        PcvPowerDomainId_Max77812_Cpu  = 0x3A000003,
        PcvPowerDomainId_Max77812_Gpu  = 0x3A000004,
        PcvPowerDomainId_Max77812_Dram = 0x3A000005, // vddq
    } PowerDomainId;
    */
    /*
     Note: I think Nintendo's I2C driver (or my driver, but it looks correct to me)
    */
    u32 GetVoltage(HocClkVoltage voltage) {
        u32 out = 0;
        BatteryChargeInfo info;
        RgltrSession s;
        switch (voltage) {
            case HocClkVoltage_SOC:
                out = I2c_BuckConverter_GetUvOut(&I2c_SOC);
                break;
            case HocClkVoltage_EMCVDD2:
                out = I2c_BuckConverter_GetUvOut(&I2c_VDD2);
                break;
            case HocClkVoltage_CPU:
                if (GetSocType() == HocClkSocType_Mariko) {
                    out = I2c_BuckConverter_GetUvOut(&I2c_Mariko_CPU);
                } else {
                    rgltrOpenSession(&s, PcvPowerDomainId_Max77621_Cpu);
                    rgltrGetVoltage(&s, &out);
                    rgltrCloseSession(&s);
                }
                break;
            case HocClkVoltage_GPU:
                if (GetSocType() == HocClkSocType_Mariko) {
                    out = I2c_BuckConverter_GetUvOut(&I2c_Mariko_GPU);
                } else {
                    rgltrOpenSession(&s, PcvPowerDomainId_Max77621_Gpu);
                    rgltrGetVoltage(&s, &out);
                    rgltrCloseSession(&s);
                }
                break;
            case HocClkVoltage_EMCVDDQ:
                if (GetSocType() == HocClkSocType_Mariko) {
                    out = I2c_BuckConverter_GetUvOut(&I2c_Mariko_DRAM_VDDQ);
                } else {
                    out = I2c_BuckConverter_GetUvOut(&I2c_VDD2);
                }
                break;
            case HocClkVoltage_Display:
                out = I2c_BuckConverter_GetUvOut(&I2c_Display);
                break;
            case HocClkVoltage_Battery:
                batteryInfoGetChargeInfo(&info);
                out = info.VoltageAvg;
                break;
            default:
                ASSERT_ENUM_VALID(HocClkVoltage, voltage);
        }

        return out > 0 ? out : 0;
    }

    Handle GetPcvHandle() {
        constexpr u64 PcvID = 0x10000000000001a;
        u64 processIDList[80]{};
        s32 processCount = 0;
        Handle handle = INVALID_HANDLE;

        DebugEventInfo debugEvent{};

        /* Get all running processes. */
        Result resultGetProcessList = svcGetProcessList(&processCount, processIDList, std::size(processIDList));
        if (R_FAILED(resultGetProcessList)) {
            return INVALID_HANDLE;
        }

        /* Try to find pcv. */
        for (int i = 0; i < processCount; ++i) {
            if (handle != INVALID_HANDLE) {
                svcCloseHandle(handle);
                handle = INVALID_HANDLE;
            }

            /* Try to debug process, if it fails, try next process. */
            Result resultSvcDebugProcess = svcDebugActiveProcess(&handle, processIDList[i]);
            if (R_FAILED(resultSvcDebugProcess)) {
                continue;
            }

            /* Try to get a debug event. */
            Result resultDebugEvent = svcGetDebugEvent(&debugEvent, handle);
            if (R_SUCCEEDED(resultDebugEvent)) {
                if (debugEvent.info.create_process.program_id == PcvID) {
                    return handle;
                }
            }
        }

        /* Failed to get handle. */
        return INVALID_HANDLE;
    }

    bool ValidateHandle(Handle handle) {
        if (handle == INVALID_HANDLE) {
            file::utils::LogLine("Invalid handle!");
            return false;
        }

        return true;
    }

    void CacheGpuVoltTable() {
        // Likely CPU regulator?
        UnkRegulator reg = {
            .voltageMin = 600000,
            .voltageStep = 12500,
            .voltageMax = 1400000,
        };

        Handle handle = GetPcvHandle();
        if (!ValidateHandle(handle)) {
            return;
        }

        MemoryInfo memoryInfo = {};
        u64 address = 0;
        u32 pageInfo = 0;
        constexpr u32 PageSize = 0x1000;
        u8 buffer[PageSize];

        /* Loop until failure. */
        while (true) {
            /* Find pcv heap. */
            while (true) {
                Result resultProcessMemory = svcQueryDebugProcessMemory(&memoryInfo, &pageInfo, handle, address);
                address = memoryInfo.addr + memoryInfo.size;

                if (R_FAILED(resultProcessMemory) || !address) {
                    svcCloseHandle(handle);
                    file::utils::LogLine("[dvfs] Failed to get process data. %u", R_DESCRIPTION(resultProcessMemory));
                    handle = INVALID_HANDLE;
                    return;
                }

                if (memoryInfo.size && (memoryInfo.perm & 3) == 3 && static_cast<char>(memoryInfo.type) == 0x4) {
                    /* Found valid memory. */
                    break;
                }
            }

            for (u64 base = 0; base < memoryInfo.size; base += PageSize) {
                u32 memorySize = std::min(memoryInfo.size, static_cast<u64>(PageSize));
                if (R_FAILED(svcReadDebugProcessMemory(buffer, handle, base + memoryInfo.addr, memorySize))) {
                    break;
                }

                u8 *resultFindReg = static_cast<u8 *>(memmem_impl(buffer, sizeof(buffer), &reg, sizeof(reg)));
                u32 index = resultFindReg - buffer;

                if (!resultFindReg) {
                    continue;
                }

                /* 800mV on Mariko, 950mV on Erista. */
                u32 bootVoltage = GetSocType() == HocClkSocType_Mariko ? 800 : 950;
                constexpr u32 GpuVoltageTableOffset = 312;
                if (!std::memcmp(&buffer[index + GpuVoltageTableOffset], &bootVoltage, sizeof(bootVoltage))) {
                    std::memcpy(voltData.voltTableStock, &buffer[index + GpuVoltageTableOffset], sizeof(voltData.voltTableStock));
                    std::memcpy(voltData.voltTableLive, voltData.voltTableStock, sizeof(voltData.voltTableStock));
                    voltData.voltTableAddress = base + memoryInfo.addr + GpuVoltageTableOffset + index;
                }

                constexpr u32 CpuVoltageTableOffset = 0xB8;
                std::memcpy(cpuVoltTable, &buffer[index + CpuVoltageTableOffset], sizeof(cpuVoltTable));  // TODO: verify the CPU table
                svcCloseHandle(handle);
                handle = INVALID_HANDLE;

                // Print info AFTER we exit the handle to avoid hangs
                for (int i = 0; i < (int)std::size(cpuVoltTable); ++i) {
                    file::utils::LogLine("[dvfs] cpu volt %d: %u mV", i, cpuVoltTable[i]);
                }

                for (int i = 0; i < (int)std::size(voltData.voltTableStock); ++i) {
                    file::utils::LogLine("[dvfs] gpu volt %d: %u mV", i, voltData.voltTableStock[0][i]);
                }
                return;
            }
        }

        svcCloseHandle(handle);
        handle = INVALID_HANDLE;
        return;
    }

    // Result ReadGpuVoltTable(u32 *table) {
    //     Handle handle = GetPcvHandle();
    //     if (!ValidateHandle(handle)) {
    //         return 1;
    //     }
    //
    //     Result rc = svcReadDebugProcessMemory(table, handle, voltData.voltTableAddress, sizeof(voltData.voltTableStock));
    //     svcCloseHandle(handle);
    //     return rc;
    // }

    Result WriteGpuVoltTable(u32 *table) {
        Handle handle = GetPcvHandle();
        if (!ValidateHandle(handle)) {
            return 1;
        }

        Result rc = svcWriteDebugProcessMemory(handle, table, voltData.voltTableAddress, sizeof(voltData.voltTableStock));
        svcCloseHandle(handle);
        return rc;
    }

    void PcvHijackGpuVolts(u32 vmin) {
        u32 table[GpuVoltTableSize];
        std::memcpy(table, voltData.voltTableLive, sizeof(voltData.voltTableLive));

        if (voltData.ramVmin == vmin) {
            return;
        }

        for (u32 i = 0; i < std::size(table); ++i) {
            if (table[i] && table[i] <= vmin) {
                table[i] = vmin;
            }
        }

        ASSERT_RESULT_OK(WriteGpuVoltTable(table), "WriteGpuVoltTable");
        voltData.ramVmin = vmin;
        file::utils::LogLine("[dvfs] voltage set to %u mV", vmin);
    }

    u32 GetGpuFreqIndex(u32 hz) {
        u32 list[HOCCLK_FREQ_LIST_MAX];
        u32 count;

        GetFreqList(HocClkModule_GPU, list, HOCCLK_FREQ_LIST_MAX, &count);

        for (u32 i = 0; i < count; ++i) {
            if (list[i] == hz) {
                return i;
            }
        }

        return HOCCLK_FREQ_LIST_MAX;
    }

    void PcvHijackGpuFrequency(u32 voltage, u32 hz) {
        u32 freqIndex = GetGpuFreqIndex(hz);
        if (freqIndex == HOCCLK_FREQ_LIST_MAX) {
            return;
        }

        bool voltageReset = (voltage == 0);

        for (u32 i = 0; i < GpuVoltTableTempCount; ++i) {
            if (voltageReset) {
                voltage = voltData.voltTableStock[i][freqIndex];
            }

            voltData.voltTableLive[i][freqIndex] = voltage;
        }

        if (voltageReset) {
            voltage = voltData.voltTableStock[GpuVoltTableTempCount - 2 /* 70C */][freqIndex];
        }

        if (voltage >= voltData.ramVmin) {
            /* Only update the table if it's safe to do so. */
            ASSERT_RESULT_OK(WriteGpuVoltTable(reinterpret_cast<u32 *>(voltData.voltTableLive)), "WriteGpuVoltTable");
        }
    }

    u32 GetMinimumGpuVmin(u32 freqMhz, u32 bracket) {
        u32 baseVolt = 800;
        if (GetSocType() == HocClkSocType_Mariko) {
            static const u32 ramTable[][22] = {
                {
                    2133, 2200, 2266, 2300, 2366, 2400, 2433, 2466, 2533, 2566, 2600,
                    2633, 2700, 2733, 2766, 2833, 2866, 2900, 2933, 3033, 3066, 3100,
                },  // Bracket 0
                {
                    2300, 2366, 2433, 2466, 2533, 2566, 2633, 2700, 2733, 2800, 2833,
                    2900, 2933, 2966, 3033, 3066, 3100, 3133, 3166, 3200, 3233, 3266,
                },  // Bracket 1
                {
                    2433, 2466, 2533, 2566, 2600, 2666, 2766, 2800, 2833, 2866, 2933,
                    2966, 3033, 3066, 3100, 3133, 3166, 3200, 3233, 3300, 3333, 3366,
                },  // Bracket 2
                {
                    2500, 2533, 2600, 2633, 2666, 2733, 2800, 2866, 2900, 2966, 3033,
                    3100, 3166, 3200, 3233, 3266, 3300, 3333, 3366, 3400, 3400, 3400,
                },  // Bracket 3
            };

            static const u32 gpuVoltArray[] = {
                590, 600, 610, 620, 630, 640, 650, 660, 670, 680, 690, 700, 710, 720, 730, 740, 750, 760, 770, 780, 790, 800,
            };

            if (freqMhz <= 1600)
                return 0;  // DVFS doesnt work below 1600MHz, it will just use vMin
            if (bracket >= std::size(ramTable))
                bracket = 0;

            u32 bracketStart = ramTable[bracket][0];

            u32 rampStartVolt = (bracket == 0) ? 535 : 525;  // Do not touch!
            u32 rampSpan = 590 - rampStartVolt;

            if (freqMhz >= 1633 && freqMhz < bracketStart) {
                u32 raw = rampStartVolt + ((freqMhz - 1633) * rampSpan) / (bracketStart - 1633);
                u32 volt = ((raw + 2) / 5) * 5;
                if (volt < rampStartVolt)
                    volt = rampStartVolt;
                if (volt > 590)
                    volt = 590;
                return volt;
            }

            baseVolt = gpuVoltArray[std::size(gpuVoltArray) - 1];
            for (u32 i = 0; i < std::size(gpuVoltArray); ++i) {
                if (freqMhz <= ramTable[bracket][i]) {
                    baseVolt = gpuVoltArray[i];
                    break;
                }
            }
            return baseVolt;
        } else {
            struct DvfsEntry {
                u32 freq;
                u32 volt;
            };
            static const DvfsEntry ramTable[][19] = {
                { { 1733, 725 },
                  { 1800, 730 },
                  { 1866, 735 },
                  { 1920, 740 },
                  { 1958, 745 },
                  { 1996, 750 },
                  { 2035, 755 },
                  { 2073, 760 },
                  { 2112, 765 },
                  { 2131, 770 },
                  { 2150, 775 },
                  { 2169, 780 },
                  { 2188, 785 },
                  { 2227, 790 },
                  { 2265, 795 },
                  { 2304, 800 },
                  { 2342, 805 },
                  { 2380, 810 },
                  { 2400, 815 } },  // Bracket 0
                { { 1733, 715 },
                  { 1800, 720 },
                  { 1866, 725 },
                  { 1920, 730 },
                  { 1958, 735 },
                  { 1996, 740 },
                  { 2035, 745 },
                  { 2073, 750 },
                  { 2112, 755 },
                  { 2131, 760 },
                  { 2150, 765 },
                  { 2169, 770 },
                  { 2188, 775 },
                  { 2227, 780 },
                  { 2265, 785 },
                  { 2304, 790 },
                  { 2342, 795 },
                  { 2380, 800 },
                  { 2400, 805 } },  // Bracket 1
                { { 1733, 705 },
                  { 1800, 710 },
                  { 1866, 715 },
                  { 1920, 720 },
                  { 1958, 725 },
                  { 1996, 730 },
                  { 2035, 735 },
                  { 2073, 740 },
                  { 2112, 745 },
                  { 2131, 750 },
                  { 2150, 755 },
                  { 2169, 760 },
                  { 2188, 765 },
                  { 2227, 770 },
                  { 2265, 775 },
                  { 2304, 780 },
                  { 2342, 785 },
                  { 2380, 790 },
                  { 2400, 795 } },  // Bracket 2
                { { 1733, 695 },
                  { 1800, 700 },
                  { 1866, 705 },
                  { 1920, 710 },
                  { 1958, 715 },
                  { 1996, 720 },
                  { 2035, 725 },
                  { 2073, 730 },
                  { 2112, 735 },
                  { 2131, 740 },
                  { 2150, 745 },
                  { 2169, 750 },
                  { 2188, 755 },
                  { 2227, 760 },
                  { 2265, 765 },
                  { 2304, 770 },
                  { 2342, 775 },
                  { 2380, 780 },
                  { 2400, 785 } },  // Bracket 3
            };

            if (freqMhz <= 1600)
                return 0;  // DVFS doesnt work below 1600MHz, it will just use vMin
            if (bracket >= std::size(ramTable))
                bracket = 0;

            const auto &entries = ramTable[bracket];
            baseVolt = entries[std::size(entries) - 1].volt;
            for (const auto &entry : entries) {
                if (freqMhz <= entry.freq) {
                    baseVolt = entry.volt;
                    break;
                }
            }
            return baseVolt;
        }
    }

    namespace {
        constexpr size_t LutSize = 33;
        constexpr u32 PmicVmin   = 250;
        constexpr u32 PmicStep   = 5;

        struct {
            volatile u32 *lut  = nullptr;
            u32 table[LutSize] = {};
            u32 lastVmin       = 0;
            u32 outputCfg      = 0;
            bool initialized   = false;
        } cpuVoltData;
    }

    namespace {
        inline void DataSyncBarrier() {
            __asm__ volatile("dsb sy" ::: "memory");
        }

        void VolatileCopyDwords(volatile u32 *dst, const volatile u32 *src, size_t dwords) {
            for (size_t i = 0; i < dwords; ++i) {
                dst[i] = src[i];
            }

            DataSyncBarrier();
        }

        u32 GetLutCode(u32 mv) {
            return mv <= PmicVmin ? 0 : (mv - PmicVmin) / PmicStep;
        }

        [[maybe_unused]] u32 GetLutVolt(u32 code) {
            return code * PmicStep + PmicVmin;
        }

        void InitializeLutPtr() {
            cpuVoltData.lut = reinterpret_cast<volatile u32 *>(cldvfs + CL_DVFS_LUT_TABLE_0);
        }

        void CacheCpuLut() {
            VolatileCopyDwords(cpuVoltData.table, cpuVoltData.lut, LutSize);
        }

        /* TODO: Dynamically get this from the PCV LUT? */
        void UnstuckFreqWar() {
            u32 hz = board::GetHz(HocClkModule_CPU);
            /* Voltage needs to be above the forced minimum voltage, so we set max for a short time. */
            /* This frequency is only applied for ~250ms. */
            board::SetHz(HocClkModule_CPU, ~0);
            board::SetHz(HocClkModule_CPU, hz);
        }

        void AdjustMinLutIndex() {
            constexpr u32 Shift = 8;
            constexpr u32 Mask  = 0x3F << Shift;
            /* Index is always 0 for us. */
            constexpr u32 MinLutIndex = 0;

            volatile u32 *reg = reinterpret_cast<volatile u32 *>(cldvfs + CL_DVFS_OUTPUT_CFG_0);

            *reg = (*reg & ~Mask) | ((MinLutIndex << Shift) & Mask);

            static_cast<void>(*reg);
        }

        void RestoreMinLutIndex() {
            *reinterpret_cast<volatile u32 *>(cldvfs + CL_DVFS_OUTPUT_CFG_0) = cpuVoltData.outputCfg;
        }

        void RestoreCpuLut() {
            cpuVoltData.lastVmin = 0;
            VolatileCopyDwords(cpuVoltData.lut, cpuVoltData.table, LutSize);
            RestoreMinLutIndex();
            UnstuckFreqWar();
        }

        u32 FloorLutTable(u32 *lut, u32 minCode) {
            u32 floored = 0;
            while (floored < LutSize && lut[floored] < minCode) {
                lut[floored++] = minCode;
            }
            return floored;
        }

        u32 StripRedundantLutEntries(u32 *lut, u32 redundant) {
            const u32 strippedCount = LutSize - redundant;
            std::memmove(lut, lut + redundant, sizeof(u32) * strippedCount);

            return strippedCount;
        }

        /* Naiive interpolation. */
        u32 InterpolateLut(u32 low, u32 high) {
            return low + (high - low) / 2;
        }

        /* Find biggest deltas, shift to create a new slot in between it, interpolate later. */
        u32 SplitAndInterpolateDeltas(u32 *lut, u32 validCount) {
            while (validCount < LutSize) {
                u32 biggestDelta = 0;
                u32 upperIdx = 0;

                for (u32 i = 0; i + 1 < validCount; ++i) {
                    const u32 delta = lut[i + 1] - lut[i];
                    if (delta > biggestDelta) {
                        biggestDelta = delta;
                        upperIdx = i + 1;
                    }
                }

                if (biggestDelta < 2) {
                    break;
                }

                const u32 mid = InterpolateLut(lut[upperIdx - 1], lut[upperIdx]);
                std::memmove(lut + upperIdx + 1, lut + upperIdx, (validCount - upperIdx) * sizeof(u32));
                lut[upperIdx] = mid;
                ++validCount;
            }

            return validCount;
        }

        void PadLutTail(u32 *lut, u32 validCount) {
            for (u32 i = validCount; i < LutSize; ++i) {
                lut[i] = lut[validCount - 1];
            }
        }
    }

    void InitializeCpuLut() {
        InitializeLutPtr();
        CacheCpuLut();
        cpuVoltData.outputCfg = *reinterpret_cast<volatile u32 *>(cldvfs + CL_DVFS_OUTPUT_CFG_0);

        cpuVoltData.initialized = true;
    }

    /* TODO: Verify table on consoles */
    u32 GetMinimumCpuVmin(u32 freqMhz, u32 bracket) {
        struct CpuVminEntry {
            u32 freq;
            u32 volt;
        };

        static const CpuVminEntry cpuVminRamTable[/* Bracket */ 7][/* Entry */ 12] = {
            { {1866, 550}, {1996, 560}, {2133, 570}, {2400, 580}, {2533, 590}, {2666, 600}, {2800, 610}, {2933, 620}, {3066, 630}, {3200, 640}, {3333, 660}, {3466, 680} },  // Bracket 0 (Speedo 1751-1800)
            { {1866, 555}, {1996, 565}, {2133, 575}, {2400, 585}, {2533, 595}, {2666, 605}, {2800, 615}, {2933, 625}, {3066, 635}, {3200, 645}, {3333, 665}, {3466, 685} },  // Bracket 1 (Speedo 1701-1750)
            { {1866, 560}, {1996, 570}, {2133, 580}, {2400, 590}, {2533, 600}, {2666, 610}, {2800, 620}, {2933, 630}, {3066, 640}, {3200, 650}, {3333, 670}, {3466, 690} },  // Bracket 2 (Speedo 1651-1700)
            { {1866, 565}, {1996, 575}, {2133, 585}, {2400, 595}, {2533, 605}, {2666, 615}, {2800, 625}, {2933, 635}, {3066, 645}, {3200, 655}, {3333, 675}, {3466, 695} },  // Bracket 3 (Speedo 1601-1650)
            { {1866, 570}, {1996, 580}, {2133, 590}, {2400, 600}, {2533, 610}, {2666, 620}, {2800, 630}, {2933, 640}, {3066, 650}, {3200, 660}, {3333, 680}, {3466, 700} },  // Bracket 4 (Speedo 1551-1600)
            { {1866, 575}, {1996, 585}, {2133, 595}, {2400, 605}, {2533, 615}, {2666, 625}, {2800, 635}, {2933, 645}, {3066, 655}, {3200, 665}, {3333, 685}, {3466, 705} },  // Bracket 5 (Speedo 1501-1550)
            { {1866, 580}, {1996, 590}, {2133, 600}, {2400, 610}, {2533, 620}, {2666, 630}, {2800, 640}, {2933, 650}, {3066, 660}, {3200, 670}, {3333, 690}, {3466, 710} },  // Bracket 6 (Speedo 1451-1500)
        };

        if (freqMhz <= 1600) {
            return 0;  // DVFS doesnt work below 1600MHz, it will just use vMin
        }

        if (bracket >= std::size(cpuVminRamTable)) {
            bracket = 0;
        }

        const auto &entries = cpuVminRamTable[bracket];
        u32 baseVolt = entries[std::size(entries) - 1].volt;
        for (const auto &entry : entries) {
            if (freqMhz <= entry.freq) {
                baseVolt = entry.volt;
                break;
            }
        }

        return baseVolt;
    }

    void ApplyCpuMinVolt(u32 vmin, bool force) {
        if(!force) {
            if (!cpuVoltData.initialized || (vmin == cpuVoltData.lastVmin)) {
                return;
            }
        }

        file::utils::LogLine("[dvfs]: %setting cpu vmin to %dmV", force ? "Forcefully s" : "S", vmin);

        if (vmin == 0) {
            RestoreCpuLut();
            file::utils::LogLine("[dvfs]: Restored LUT");
            return;
        }

        u32 tmpLut[LutSize];
        std::memcpy(tmpLut, cpuVoltData.table, sizeof(tmpLut));

        const u32 minCode = std::min(GetLutCode(vmin), tmpLut[LutSize - 1]);

        const u32 floored = FloorLutTable(tmpLut, minCode);
        if (floored == 0) {
            RestoreCpuLut();
            file::utils::LogLine("[dvfs]: Floored, restored LUT");
            return;
        }

        u32 redundant = floored - 1;
        if (floored < LutSize && tmpLut[floored] == minCode) {
            redundant = floored;
        }

        u32 valid = StripRedundantLutEntries(tmpLut, redundant);
        valid     = SplitAndInterpolateDeltas(tmpLut, valid);
        PadLutTail(tmpLut, valid);

        VolatileCopyDwords(cpuVoltData.lut, tmpLut, LutSize);
        AdjustMinLutIndex();

        UnstuckFreqWar();

        cpuVoltData.lastVmin = vmin;
    }

}
