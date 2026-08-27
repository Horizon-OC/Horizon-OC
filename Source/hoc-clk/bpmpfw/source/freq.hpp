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

    u64 MeasurePtoFreqHz(u32 srcId, u32 multiplier);

    void Update(HocClkBpmpSharedInfo &info);

} // namespace freq
