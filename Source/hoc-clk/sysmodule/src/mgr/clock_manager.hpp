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

#include "../util/lockable_mutex.h"

namespace clockManager {

    struct FreqTable {
        std::uint32_t count;
        std::uint32_t list[HOCCLK_FREQ_LIST_MAX];
    };

    extern bool hasChanged;

    // instance variables
    extern bool gRunning;
    extern LockableMutex gContextMutex;
    extern HocClkContext gContext;
    extern FreqTable gFreqTable[HocClkModule_EnumMax];
    extern u32 patchedEmcMaxClock;
    extern std::uint64_t gLastTempLogNs;
    extern std::uint64_t gLastFreqLogNs;
    extern std::uint64_t gLastPowerLogNs;
    extern std::uint64_t gLastCsvWriteNs;

    void Initialize();
    void Exit();

    HocClkContext GetCurrentContext();

    void SetRunning(bool running);
    bool Running();

    std::uint32_t GetMaxAllowedHz(HocClkModule module, HocClkProfile profile);
    bool IsAssignableHz(HocClkModule module, std::uint32_t hz);

    void GetFreqList(HocClkModule module, std::uint32_t *list, std::uint32_t maxCount, std::uint32_t *outCount);

    void ApplyGpuFreqVoltRequest(u32 voltage, u32 hz);

    void Tick();
    void WaitForNextTick();
}  // namespace clockManager
