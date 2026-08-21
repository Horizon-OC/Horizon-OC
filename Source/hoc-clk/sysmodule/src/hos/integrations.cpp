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

#include <SaltyNX.h>

#include "integrations.hpp"
#include "process_management.hpp"
#include <sys/stat.h>

namespace integrations {

    namespace {

        NxFpsSharedBlock *gNxFps = nullptr;
        SharedMemory gSharedMemory = {};
        bool gSharedMemoryUsed = false;
        Handle gRemoteSharedMemory = 1;
        u64 gPrevTid = 0;
        u8 resolutionLookup = 0;
        bool CheckSaltyNXPort() {
            Handle saltysd;

            for (int i = 0; i < 67; i++) {
                if (R_SUCCEEDED(svcConnectToNamedPort(&saltysd, "InjectServ"))) {
                    svcCloseHandle(saltysd);
                    break;
                }
                if (i == 66)
                    return false;
                svcSleepThread(1'000'000);
            }

            for (int i = 0; i < 67; i++) {
                if (R_SUCCEEDED(svcConnectToNamedPort(&saltysd, "InjectServ"))) {
                    svcCloseHandle(saltysd);
                    return true;
                }
                svcSleepThread(1'000'000);
            }

            return false;
        }

        void SearchSharedMemoryBlock(uintptr_t base) {
            ptrdiff_t search_offset = 0;
            while (search_offset < 0x1000) {
                gNxFps = (NxFpsSharedBlock *)(base + search_offset);
                if (gNxFps->MAGIC == 0x465053)
                    return;
                search_offset += 4;
            }
            gNxFps = nullptr;
        }

        void LoadSharedMemory() {
            if (SaltySD_Connect())
                return;
            SaltySD_GetSharedMemoryHandle(&gRemoteSharedMemory);
            SaltySD_Term();
            shmemLoadRemote(&gSharedMemory, gRemoteSharedMemory, 0x1000, Perm_Rw);
            if (!shmemMap(&gSharedMemory))
                gSharedMemoryUsed = true;
        }

    }  // namespace

    bool GetSysDockState() {
        struct stat st = { 0 };
        return stat("sdmc:/atmosphere/contents/42000000000000A0/flags/boot2.flag", &st) == 0;
    }

    bool GetSaltyNXState() {
        struct stat st = { 0 };
        return stat("sdmc:/atmosphere/contents/0000000000534C56/flags/boot2.flag", &st) == 0;
    }

    void LoadSaltyNX() {
        if (!CheckSaltyNXPort())
            return;
        LoadSharedMemory();
    }

    u8 GetSaltyNXFPS() {
        if (!gSharedMemoryUsed)
            return 254;

        u64 tid = processManagement::GetCurrentApplicationId();
        if (tid == 0)
            return 254;

        if (gPrevTid != tid) {
            gNxFps = nullptr;
            gPrevTid = tid;
        }

        if (!gNxFps) {
            uintptr_t base = (uintptr_t)shmemGetAddr(&gSharedMemory);
            SearchSharedMemoryBlock(base);
        }

        return gNxFps ? gNxFps->FPS : 254;
    }

    u16 GetSaltyNXResolutionHeight() {
        if (!gSharedMemoryUsed)
            return 0;

        u64 tid = processManagement::GetCurrentApplicationId();
        if (tid == 0)
            return 0;

        if (gPrevTid != tid) {
            gNxFps = nullptr;
            gPrevTid = tid;
            resolutionLookup = 0;
        }

        if (!gNxFps) {
            uintptr_t base = (uintptr_t)shmemGetAddr(&gSharedMemory);
            SearchSharedMemoryBlock(base);
        }

        if (gNxFps) {
            if (!resolutionLookup) {
                gNxFps->renderCalls[0].calls = 0xFFFF;
                resolutionLookup = 1;
                return 0;
            } else if (resolutionLookup == 1) {
                if (gNxFps->renderCalls[0].calls != 0xFFFF)
                    resolutionLookup = 2;
                else
                    return 0;
            }

            return gNxFps->renderCalls[0].height == 0 ? gNxFps->viewportCalls[0].height : gNxFps->renderCalls[0].height;
        }
        return 0;
    }

}  // namespace integrations
