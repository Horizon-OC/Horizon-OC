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

#pragma once
#include "freq.hpp"
#include "regs.hpp"

#include <hocclk/bpmp.h>

namespace actmon {

    constexpr u32 ActmonBase    = 0x6000C800;
    constexpr u32 ActmonDevBase = ActmonBase + 0x80;
    constexpr u32 ActmonDevSize = 0x40;

    constexpr u32 ActmonGlbStatus     = 0x0;
    constexpr u32 ActmonGlbPeriodCtrl = 0x4;
    constexpr u32 ActmonMccpuMonAct   = (1u << 8);
    constexpr u32 ActmonMcallMonAct   = (1u << 9);

    constexpr u32 ActmonDevCtrlKVal3       = (3u << 10);
    constexpr u32 ActmonDevCtrlEnbPeriodic = (1u << 18);
    constexpr u32 ActmonDevCtrlEnb         = (1u << 31);

    constexpr u32 ActmonPeriodMs = 20;

    enum ActmonDev : u32 {
        DevCpu = 0,
        DevBpmp,
        DevAhb,
        DevApb,
        DevCpuFreq,
        DevMcAll,
        DevMcCpu,
    };

    constexpr u32 DevCtrl        = 0x00;
    constexpr u32 DevInitAvg     = 0x0C;
    constexpr u32 DevCountWeight = 0x18;
    constexpr u32 DevAvgCount    = 0x20;

    inline u32 DevReg(ActmonDev dev, u32 off) {
        return ActmonDevBase + (static_cast<u32>(dev) * ActmonDevSize) + off;
    }

    inline void EnableDev(ActmonDev dev, u32 freqKhz, u32 weight) {
        MMIO32(DevReg(dev, DevInitAvg))     = freqKhz * ActmonPeriodMs / 2;
        MMIO32(DevReg(dev, DevCountWeight)) = weight;
        MMIO32(DevReg(dev, DevCtrl))        = ActmonDevCtrlEnb | ActmonDevCtrlEnbPeriodic | ActmonDevCtrlKVal3;
    }

    inline void Init() {
        const u32 emcFreqKhz = static_cast<u32>(freq::MeasurePtoFreqHz(freq::ClkPtoEmc, 1) / 1000);

        if (!(MMIO32(ActmonBase + ActmonGlbStatus) & ActmonMcallMonAct)) {
            MMIO32(ActmonBase + ActmonGlbPeriodCtrl) = (ActmonPeriodMs - 1) & 0xFF;
            EnableDev(DevMcAll, emcFreqKhz, 256 * 4);
        }

        if (!(MMIO32(ActmonBase + ActmonGlbStatus) & ActmonMccpuMonAct)) {
            EnableDev(DevMcCpu, emcFreqKhz, 256 * 4);
        }
    }

    inline void Update(HocClkBpmpSharedInfo &info) {
        const u32 emcFreqKhz = info.freqMem;
        if (emcFreqKhz == 0) {
            return;
        }

        const u32 avgAll = MMIO32(DevReg(DevMcAll, DevAvgCount));
        const u32 avgCpu = MMIO32(DevReg(DevMcCpu, DevAvgCount));

        // Get 1000 -> 100.0.
        info.emcLoadAll = static_cast<u32>((static_cast<u64>(avgAll) * 10 * 100) / (emcFreqKhz * ActmonPeriodMs));
        info.emcLoadCpu = static_cast<u32>((static_cast<u64>(avgCpu) * 10 * 100) / (emcFreqKhz * ActmonPeriodMs));

        info.emcBwAll = static_cast<u32>((static_cast<u64>(emcFreqKhz) * 16 * info.emcLoadAll) / 1000000);
        info.emcBwCpu = static_cast<u32>((static_cast<u64>(emcFreqKhz) * 16 * info.emcLoadCpu) / 1000000);

        // Not 100% accurate but should be enough.
        info.emcBwGpu = (info.emcBwAll > info.emcBwCpu) ? (info.emcBwAll - info.emcBwCpu) : 0;
    }

} // namespace actmon
