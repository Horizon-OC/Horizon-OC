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

#pragma once
#include <hocclk.h>
#include <switch.h>


namespace board {

    constexpr u32 GpuVoltTableTempCount = 6;
    constexpr u32 GpuVoltFreqTempCount  = 32;
    constexpr size_t GpuVoltTableSize   = GpuVoltTableTempCount * GpuVoltFreqTempCount;

    struct GpuVoltData {
        u32 voltTableStock[GpuVoltTableTempCount][GpuVoltFreqTempCount] = {};
        u32 voltTableRamOc[GpuVoltTableTempCount][GpuVoltFreqTempCount] = {};
        u32 voltTableLive[GpuVoltTableTempCount][GpuVoltFreqTempCount]  = {};

        u64 voltTableAddress;
        u32 ramVmin;
    };

    struct UnkRegulator {
        u32 voltageMin;
        u32 voltageStep;
        u32 voltageMax;
    };

    struct CpuDfllData {
        u32 tune0Low;
        u32 tune0High;
        u32 tune1Low;
        u32 tune1High;
        // u32 tune_high_min_millivolts;
        // u32 tune_high_margin_millivolts;
        // u64 dvco_calibration_max;
    };

    void SetDfllTunings(u32 levelLow, u32 levelHigh, u32 tbreakPoint);
    void CacheDfllData();
    u32 CalculateTbreak(u32 table);
    u32 GetVoltage(HocClkVoltage voltage);
    void CacheGpuVoltTable();
    void PcvHijackGpuVolts(u32 vmin);
    void PcvHijackGpuFrequency(u32 voltage, u32 hz);
    u32 GetMinimumGpuVmin(u32 freqMhz, u32 bracket);

}  // namespace board
