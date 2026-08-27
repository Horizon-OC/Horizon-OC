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

    u32 DevReg(ActmonDev dev, u32 off);

    void EnableDev(ActmonDev dev, u32 freqKhz, u32 weight);

    void Init();

    void Update(HocClkBpmpSharedInfo &info);

} // namespace actmon
