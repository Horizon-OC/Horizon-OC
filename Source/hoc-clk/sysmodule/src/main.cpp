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

#include <cstdlib>
#include <cstring>
#include <malloc.h>
#include <switch.h>

#include "board/board.hpp"
#include "file/config.hpp"
#include "file/errors.hpp"
#include "file/file_utils.hpp"
#include "hos/process_management.hpp"
#include "ipc/ipc_service.hpp"
#include "mgr/clock_manager.hpp"


#define INNER_HEAP_SIZE 0x40000

extern "C" {
void virtmemSetup(void);

extern std::uint32_t __start__;

std::uint32_t __nx_applet_type = AppletType_None;
TimeServiceType __nx_time_service_type = TimeServiceType_System;
std::uint32_t __nx_fs_num_sessions = 1;
u32 __nx_nv_transfermem_size = 0x8000;
size_t nx_inner_heap_size = INNER_HEAP_SIZE;
char nx_inner_heap[INNER_HEAP_SIZE];
NvServiceType __nx_nv_service_type = NvServiceType_Factory;

// Ty to MasaGratoR for this!
// This is done to save some space as they have no practical use in our case
void *__real___cxa_throw(void *thrown_exception, void *pvar, void (*dest)(void *));
void *__real__Unwind_Resume();
void *__real___gxx_personality_v0();

void __wrap___cxa_throw(void *thrown_exception, void *pvar, void (*dest)(void *)) {
    abort();
}

void __wrap__Unwind_Resume() {
    return;
}

void __wrap___gxx_personality_v0() {
    return;
}

void __libnx_initheap(void) {
    void *addr = nx_inner_heap;
    size_t size = nx_inner_heap_size;

    /* Newlib Heap Management */
    extern char *fake_heap_start;
    extern char *fake_heap_end;

    fake_heap_start = (char *)addr;
    fake_heap_end = (char *)addr + size;

    virtmemSetup();
}

void __appInit(void) {
    if (R_FAILED(smInitialize())) {
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));
    }

    Result rc = setsysInitialize();
    if (R_SUCCEEDED(rc)) {
        SetSysFirmwareVersion fw;
        rc = setsysGetFirmwareVersion(&fw);
        if (R_SUCCEEDED(rc))
            hosversionSet(MAKEHOSVERSION(fw.major, fw.minor, fw.micro));
        setsysExit();
    }

    // rc = fanInitialize();
    // if (R_FAILED(rc))
    //     diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_ShouldNotHappen));

    rc = i2cInitialize();
    if (R_FAILED(rc))
        diagAbortWithResult(MAKERESULT(Module_Libnx, LibnxError_ShouldNotHappen));
}

void __appExit(void) {
    // CloseFanControllerThread();
    // fanExit();
    i2cExit();
    setsysExit();
    fsdevUnmountAll();
    fsExit();
    smExit();
}
}

int main(int argc, char **argv) {
    Result rc = fileUtils::Initialize();
    if (R_FAILED(rc)) {
        fatalThrow(rc);
        return 1;
    }
    config::Initialize();
    config::Refresh();  // Get config from file

    board::Initialize();
    processManagement::Initialize();

    processManagement::WaitForQLaunch();

    clockManager::Initialize();
    ipcService::Initialize();

    clockManager::SetRunning(true);
    config::SetEnabled(true);
    ipcService::SetRunning(true);
    // TemperaturePoint *table;
    // ReadConfigFile(&table);
    // InitFanController(table);
    // StartFanControllerThread();

    while (clockManager::Running()) {
        clockManager::Tick();
        clockManager::WaitForNextTick();
    }

    ipcService::SetRunning(false);
    ipcService::Exit();
    clockManager::Exit();
    processManagement::Exit();
    board::Exit();
    config::Exit();
    fileUtils::LogLine("Exiting hoc-clk");
    svcSleepThread(1000000ULL);
    fileUtils::Exit();

    return 0;
}
