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

#include <cstring>
#include <i2c.h>
#include <max17050.h>
#include <switch.h>
#include <tmp451.h>

#include "../file/config.hpp"
#include "../file/errors.hpp"
#include "../file/file_utils.hpp"
#include "../file/kip.hpp"
#include "../hos/apm_ext.h"
#include "../mgr/clock_manager.hpp"
#include "ipc_service.hpp"
#include <ipc_server.h>
#include <lockable_mutex.h>
namespace ipcService {

    namespace {

        bool gRunning = false;
        Thread gThread;
        LockableMutex gThreadMutex;
        IpcServer gServer;

        Result GetApiVersion(u32 *out_version) {
            *out_version = HOCCLK_IPC_API_VERSION;
            return 0;
        }

        Result GetVersionString(char *out_buf, size_t bufSize) {
            if (bufSize) {
                strncpy(out_buf, TARGET_VERSION, bufSize - 1);
            }
            return 0;
        }

        Result GetCurrentContext(HocClkContext *out_ctx) {
            *out_ctx = clockManager::GetCurrentContext();
            return 0;
        }

        Result ExitHandler() {
            clockManager::SetRunning(false);
            return 0;
        }

        Result GetProfileCount(std::uint64_t *tid, std::uint8_t *out_count) {
            if (!config::HasProfilesLoaded()) {
                return HOCCLK_ERROR(ConfigNotLoaded);
            }
            *out_count = config::GetProfileCount(*tid);
            return 0;
        }

        Result GetProfiles(std::uint64_t *tid, HocClkTitleProfileList *out_profiles) {
            if (!config::HasProfilesLoaded()) {
                return HOCCLK_ERROR(ConfigNotLoaded);
            }
            config::GetProfiles(*tid, out_profiles);
            return 0;
        }

        Result SetProfiles(HocClkIpc_SetProfiles_Args *args) {
            if (!config::HasProfilesLoaded()) {
                return HOCCLK_ERROR(ConfigNotLoaded);
            }
            HocClkTitleProfileList profiles = args->profiles;
            if (!config::SetProfiles(args->tid, &profiles, true)) {
                return HOCCLK_ERROR(ConfigSaveFailed);
            }
            return 0;
        }

        Result SetEnabled(std::uint8_t *enabled) {
            config::SetEnabled(*enabled);
            return 0;
        }

        Result SetOverride(HocClkIpc_SetOverride_Args *args) {
            if (!HOCCLK_ENUM_VALID(HocClkModule, args->module)) {
                return HOCCLK_ERROR(Generic);
            }
            config::SetOverrideHz(args->module, args->hz);
            return 0;
        }

        Result GetConfigValuesHandler(HocClkConfigValueList *out_configValues) {
            if (!config::HasProfilesLoaded()) {
                return HOCCLK_ERROR(ConfigNotLoaded);
            }
            config::GetConfigValues(out_configValues);
            return 0;
        }

        Result SetConfigValuesHandler(HocClkConfigValueList *configValues) {
            if (!config::HasProfilesLoaded()) {
                return HOCCLK_ERROR(ConfigNotLoaded);
            }
            HocClkConfigValueList copy = *configValues;
            if (!config::SetConfigValues(&copy, true)) {
                return HOCCLK_ERROR(ConfigSaveFailed);
            }
            return 0;
        }

        Result GetFreqList(HocClkIpc_GetFreqList_Args *args, std::uint32_t *out_list, std::size_t size, std::uint32_t *out_count) {
            if (!HOCCLK_ENUM_VALID(HocClkModule, args->module)) {
                return HOCCLK_ERROR(Generic);
            }
            if (args->maxCount != size / sizeof(*out_list)) {
                return HOCCLK_ERROR(Generic);
            }
            clockManager::GetFreqList(args->module, out_list, args->maxCount, out_count);
            return 0;
        }

        Result RequestGpuVoltage(HocClkIpc_RequestGpuVoltage_Args *args) {
            clockManager::ApplyGpuFreqVoltRequest(args->voltage, args->hz);
            return 0;
        }

        Result ServiceHandlerFunc(void *arg, const IpcServerRequest *r, u8 *out_data, size_t *out_dataSize) {
            (void)arg;
            switch (r->data.cmdId) {
                case HocClkIpcCmd_GetApiVersion:
                    *out_dataSize = sizeof(u32);
                    return GetApiVersion((u32 *)out_data);

                case HocClkIpcCmd_GetVersionString:
                    if (r->hipc.meta.num_recv_buffers >= 1) {
                        return GetVersionString((char *)hipcGetBufferAddress(r->hipc.data.recv_buffers),
                                                hipcGetBufferSize(r->hipc.data.recv_buffers));
                    }
                    break;

                case HocClkIpcCmd_GetCurrentContext:
                    if (r->data.size >= sizeof(std::uint64_t) && r->hipc.meta.num_recv_buffers >= 1) {
                        size_t bufSize = hipcGetBufferSize(r->hipc.data.recv_buffers);
                        if (bufSize >= sizeof(HocClkContext)) {
                            return GetCurrentContext((HocClkContext *)hipcGetBufferAddress(r->hipc.data.recv_buffers));
                        }
                    }
                    break;

                case HocClkIpcCmd_Exit:
                    return ExitHandler();

                case HocClkIpcCmd_GetProfileCount:
                    if (r->data.size >= sizeof(std::uint64_t)) {
                        *out_dataSize = sizeof(std::uint8_t);
                        return GetProfileCount((std::uint64_t *)r->data.ptr, (std::uint8_t *)out_data);
                    }
                    break;

                case HocClkIpcCmd_GetProfiles:
                    if (r->data.size >= sizeof(std::uint64_t) && r->hipc.meta.num_recv_buffers >= 1) {
                        size_t bufSize = hipcGetBufferSize(r->hipc.data.recv_buffers);
                        if (bufSize >= sizeof(HocClkTitleProfileList)) {
                            return GetProfiles((std::uint64_t *)r->data.ptr,
                                               (HocClkTitleProfileList *)hipcGetBufferAddress(r->hipc.data.recv_buffers));
                        }
                    }
                    break;

                case HocClkIpcCmd_SetProfiles:
                    if (r->data.size >= sizeof(HocClkIpc_SetProfiles_Args)) {
                        return SetProfiles((HocClkIpc_SetProfiles_Args *)r->data.ptr);
                    }
                    break;

                case HocClkIpcCmd_SetEnabled:
                    if (r->data.size >= sizeof(std::uint8_t)) {
                        return SetEnabled((std::uint8_t *)r->data.ptr);
                    }
                    break;

                case HocClkIpcCmd_SetOverride:
                    if (r->data.size >= sizeof(HocClkIpc_SetOverride_Args)) {
                        return SetOverride((HocClkIpc_SetOverride_Args *)r->data.ptr);
                    }
                    break;

                case HocClkIpcCmd_GetConfigValues:
                    if (r->hipc.meta.num_recv_buffers >= 1) {
                        size_t bufSize = hipcGetBufferSize(r->hipc.data.recv_buffers);
                        if (bufSize >= sizeof(HocClkConfigValueList)) {
                            return GetConfigValuesHandler((HocClkConfigValueList *)hipcGetBufferAddress(r->hipc.data.recv_buffers));
                        }
                    }
                    break;

                case HocClkIpcCmd_SetConfigValues:
                    if (r->hipc.meta.num_send_buffers >= 1) {
                        size_t bufSize = hipcGetBufferSize(r->hipc.data.send_buffers);
                        if (bufSize >= sizeof(HocClkConfigValueList)) {
                            return SetConfigValuesHandler((HocClkConfigValueList *)hipcGetBufferAddress(r->hipc.data.send_buffers));
                        }
                    }
                    break;

                case HocClkIpcCmd_GetFreqList:
                    if (r->data.size >= sizeof(HocClkIpc_GetFreqList_Args) && r->hipc.meta.num_recv_buffers >= 1) {
                        *out_dataSize = sizeof(std::uint32_t);
                        return GetFreqList((HocClkIpc_GetFreqList_Args *)r->data.ptr,
                                           (std::uint32_t *)hipcGetBufferAddress(r->hipc.data.recv_buffers),
                                           hipcGetBufferSize(r->hipc.data.recv_buffers), (std::uint32_t *)out_data);
                    }
                    break;

                case HocClkIpcCmd_SetKipData:
                    if (r->data.size >= 0) {
                        kip::SetKipData();
                        return 0;
                    }
                    break;

                case HocClkIpcCmd_RequestGpuVoltage:
                    if (r->data.size >= sizeof(HocClkIpc_RequestGpuVoltage_Args)) {
                        return RequestGpuVoltage((HocClkIpc_RequestGpuVoltage_Args *)r->data.ptr);
                    }
                    break;
            }

            return HOCCLK_ERROR(Generic);
        }

        void ProcessThreadFunc(void *arg) {
            (void)arg;
            Result rc;
            while (true) {
                rc = ipcServerProcess(&gServer, &ServiceHandlerFunc, nullptr);
                if (R_FAILED(rc)) {
                    if (rc == KERNELRESULT(Cancelled)) {
                        return;
                    }
                    if (rc != KERNELRESULT(ConnectionClosed)) {
                        fileUtils::LogLine("[ipc] ipcServerProcess: [0x%x] %04d-%04d", rc, R_MODULE(rc), R_DESCRIPTION(rc));
                    }
                }
            }
        }

    }  // namespace

    void Initialize() {
        std::int32_t priority;
        Result rc = svcGetThreadPriority(&priority, CUR_THREAD_HANDLE);
        ASSERT_RESULT_OK(rc, "svcGetThreadPriority");
        rc = ipcServerInit(&gServer, HOCCLK_IPC_SERVICE_NAME, 42);
        ASSERT_RESULT_OK(rc, "ipcServerInit");
        rc = threadCreate(&gThread, &ProcessThreadFunc, nullptr, NULL, 0x4000, priority, -2);
        ASSERT_RESULT_OK(rc, "threadCreate");
        gRunning = false;
    }

    void Exit() {
        SetRunning(false);
        Result rc = threadClose(&gThread);
        ASSERT_RESULT_OK(rc, "threadClose");
        rc = ipcServerExit(&gServer);
        ASSERT_RESULT_OK(rc, "ipcServerExit");
    }

    void SetRunning(bool running) {
        std::scoped_lock lock{ gThreadMutex };
        if (gRunning == running) {
            return;
        }

        gRunning = running;

        if (running) {
            Result rc = threadStart(&gThread);
            ASSERT_RESULT_OK(rc, "threadStart");
        } else {
            svcCancelSynchronization(gThread.handle);
            threadWaitForExit(&gThread);
        }
    }

}  // namespace ipcService
