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

#pragma once

#include <hocclk.h>
#include <switch.h>

#include "../mapping/mem_map.hpp"
#include "board_freq.hpp"
#include "board_fuse.hpp"
#include "board_load.hpp"
#include "board_name.hpp"
#include "board_profile.hpp"
#include "board_sensor.hpp"
#include "board_volt.hpp"

#define HOSSVC_HAS_CLKRST (hosversionAtLeast(8, 0, 0))
#define HOSSVC_HAS_TC (hosversionAtLeast(5, 0, 0))

namespace board {
    extern u64 clkVirtAddr, dsiVirtAddr, apbVirtAddr, fuseVirtAddr, sysVirtAddr, actmonVirtAddr;
    extern HocClkSocType gSocType;
    extern u8 gDramID;
    extern HocClkConsoleType gConsoleType;
    extern FuseData fuseData;
    extern u8 speedoBracket;

    void Initialize();
    void Exit();
    HocClkSocType GetSocType();
    HocClkConsoleType GetConsoleType();
    u8 GetDramID();
    u8 GetGpuSpeedoBracket();
    bool IsDram8GB();
    void SetDisplayRefreshDockedState(bool docked);
    FuseData *GetFuseData();

}  // namespace board
