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

#include <stdio.h>
#include <string.h>

#include "actmon.hpp"
#include "aotag.hpp"
#include "bpmp_mmu.hpp"
#include "freq.hpp"
#include "libc_platform.hpp"
#include "pllmb.hpp"
#include "regs.hpp"
#include "sensors.hpp"

#include <hocclk/bpmp.h>

extern "C" [[noreturn]] void _panic_reboot();

namespace {

    HocClkBpmpSharedInfo &SharedInfo() {
        return *reinterpret_cast<HocClkBpmpSharedInfo *>(WorkRamStart);
    }

} // namespace

[[noreturn]] void HandleShutdown(HocClkBpmpSharedInfo &info) {
    /* Deinit MMU otherwise sleep will fatal. */
    bpmpMmu::Disable();
    info.magic = 0x0;
    UartPutsForce("[hoc-bpmpfw]: Halting...\n");
    for (;;)
        ;
}

void HandleCommand(HocClkBpmpSharedInfo &info) {
    switch (info.cmd) {
        case HocClkBpmpCmd_None:
            break;
        case HocClkBpmpCmd_RequestShutdown:
            HandleShutdown(info);
            break;
        case HocClkBpmpCmd_SetUartEnabled:
            g_uartLoggingEnabled = (info.cmdArg1 != 0);
            info.cmd = HocClkBpmpCmd_None;
            break;
        default:
            info.cmd = HocClkBpmpCmd_None;
            break;
    }
}

void MainLoop(HocClkBpmpSharedInfo &info) {
    for (;;) {
        HandleCommand(info);

        const u32 temp1 = MMIO32(SocthermBase + SocthermSensorTemp1);
        const u32 temp2 = MMIO32(SocthermBase + SocthermSensorTemp2);

        info.tempCpu  = TranslateSocthermTemp(static_cast<u16>(temp1 >> 16));
        info.tempGpu  = TranslateSocthermTemp(static_cast<u16>(temp1));
        info.tempMem  = TranslateSocthermTemp(static_cast<u16>(temp2 >> 16));
        info.tempPllx = TranslateSocthermTemp(static_cast<u16>(temp2));

        freq::Update(info);
        actmon::Update(info);
        aotag::Update(info);
        info.freqMemPll = static_cast<u32>(pllmb::MeasureRamClockHz() / 1000);

        msleep(250);
    }
}

extern "C" void main() {
    bpmpMmu::Enable();

    InitializeLibc();

    UartPutsForce("[hoc-bpmpfw]: Starting bpmpfw\n");

    HocClkBpmpSharedInfo &info = SharedInfo();
    memset(&info, 0, sizeof(info));
    info.status = 0;

    actmon::Init();

    info.magic = HOCCLK_BPMP_MAGIC; // written last

    /* If the main loop exits then it is a panic or invalid state */
    MainLoop(info);

    UartPutsForce("[hoc-bpmpfw]: PANIC\n");
    
    /* Kill execution */    
    _panic_reboot();
}
