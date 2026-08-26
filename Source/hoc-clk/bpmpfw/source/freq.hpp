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
#include "regs.hpp"

#include <hocclk/bpmp.h>

namespace freq {

    constexpr u32 CarBase            = 0x60006000;
    constexpr u32 CarPtoClkCntCntl   = 0x60;
    constexpr u32 CarPtoClkCntStatus = 0x64;

    constexpr u32 PtoCntEn       = (1u << 9);
    constexpr u32 PtoCntRst      = (1u << 10);
    constexpr u32 PtoClkEnable   = (1u << 13);
    constexpr u32 PtoSrcSelShift = 14;
    constexpr u32 PtoDivSelDiv1  = (1u << 23);
    constexpr u32 PtoClkCntBusy  = (1u << 31);
    constexpr u32 PtoClkCntMask  = 0xFFFFFF;

    constexpr u32 ClkPtoCclkGDiv2 = 0x13;
    constexpr u32 ClkPtoEmc       = 0x24;

    inline u64 MeasurePtoFreqHz(u32 srcId, u32 multiplier) {
        constexpr u32 win = 16;
        constexpr u32 osc = 32768;

        const u32 val = (srcId << PtoSrcSelShift) | PtoDivSelDiv1 | PtoClkEnable | (win - 1);

        MMIO32(CarBase + CarPtoClkCntCntl) = val;
        (void)MMIO32(CarBase + CarPtoClkCntCntl);
        usleep(2);

        MMIO32(CarBase + CarPtoClkCntCntl) = val | PtoCntRst;
        (void)MMIO32(CarBase + CarPtoClkCntCntl);
        usleep(2);

        MMIO32(CarBase + CarPtoClkCntCntl) = val;
        (void)MMIO32(CarBase + CarPtoClkCntCntl);
        usleep(2);

        MMIO32(CarBase + CarPtoClkCntCntl) = val | PtoCntEn;
        (void)MMIO32(CarBase + CarPtoClkCntCntl);
        usleep((1000000u * win / osc) + 12 + 2); // 502us

        while (MMIO32(CarBase + CarPtoClkCntStatus) & PtoClkCntBusy)
            ;

        const u32 cnt = MMIO32(CarBase + CarPtoClkCntStatus) & PtoClkCntMask;

        MMIO32(CarBase + CarPtoClkCntCntl) = 0;
        (void)MMIO32(CarBase + CarPtoClkCntCntl);
        usleep(2);

        return (static_cast<u64>(cnt) * multiplier * osc) / win;
    }

    inline void Update(HocClkBpmpSharedInfo &info) {
        info.freqMem = static_cast<u32>(MeasurePtoFreqHz(ClkPtoEmc, 1) / 1000);
        info.freqCpu = static_cast<u32>(MeasurePtoFreqHz(ClkPtoCclkGDiv2, 2) / 1000);
    }

} // namespace freq
