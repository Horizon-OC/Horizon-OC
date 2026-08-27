/*
 * Copyright (c) Souldbminer
 * Copyright (c) KazushiMe
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

namespace pllmb {

    constexpr u32 PtoPllm  = 2;
    constexpr u32 PtoPllmb = 37;

    constexpr u32 PllmMisc2         = 0x9C;
    constexpr u32 PllmMisc2PllmBit  = (1u << 8);
    constexpr u32 PllmMisc2PllmbBit = (1u << 9);

    constexpr u32 PtoCyclePeriod = (1u << 31); // PTO_CLK_CNT_STATUS busy bit

    u64 MeasurePtoHz(u32 ptoId, u32 divider, u32 preselReg, u32 preselMask);

    u64 MeasureRamClockHz();

} // namespace pllmb
