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

/* --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include <i2c.h>
#include <max17050.h>
#include <tmp451.h>

#include "../hos/apm_ext.h"
#include "file_utils.hpp"
#include "config.hpp"
#include <ipc_server.h>
#include <lockable_mutex.h>
#include <string>
#include <utility>
#include <vector>

extern "C" void __libnx_init_time(void);

namespace file::utils {

    namespace {

        u64 bootTimeS;
        LockableMutex g_log_mutex;
        LockableMutex g_csv_mutex;
        std::atomic_bool g_has_initialized = false;
        bool g_log_enabled = false;
        bool g_uart_enabled = false;
        bool g_flags_resolved = false;

        constexpr size_t MaxPendingLines = 64;
        std::vector<std::pair<u64, std::string>> g_pending_lines;

        void EmitLine(u64 secs, const char *buff, int len) {
            if (g_uart_enabled) {
                svcOutputDebugString(buff, len);
            }

            if (g_log_enabled) {
                FILE *file = fopen(FILE_LOG_FILE_PATH, "a");

                if (file) {
                    fprintf(file, "[%luls] %s", secs, buff);
                    fclose(file);
                }
            }
        }

        void InitializeThreadFunc(void *args) {
            Initialize();
        }

    }  // namespace

    void InitializeLogging() {
        const bool logEnabled  = config::GetConfigValue(HocClkConfigValue_FileLogging);
        const bool uartEnabled = config::GetConfigValue(HocClkConfigValue_UartLogging);

        std::scoped_lock lock{ g_log_mutex };
        g_log_enabled  = logEnabled;
        g_uart_enabled = uartEnabled;
        g_flags_resolved = true;

        if (g_log_enabled || g_uart_enabled) {
            for (const auto &[secs, line] : g_pending_lines) {
                EmitLine(secs, line.c_str(), (int)line.size());
            }
        }
        g_pending_lines.clear();
        g_pending_lines.shrink_to_fit();
    }

    bool IsInitialized() {
        return g_has_initialized;
    }

    bool IsLogEnabled() {
        return g_log_enabled;
    }

    bool IsUartEnabled() {
        return g_uart_enabled;
    }

    void LogLine(const char *format, ...) {
        std::scoped_lock lock{ g_log_mutex };

        if (!g_has_initialized) {
            return;
        }

        if (g_flags_resolved && !g_log_enabled && !g_uart_enabled) {
            return;
        }

        va_list args;
        va_start(args, format);

        char buff[0x200];
        int len = vsnprintf(buff, sizeof(buff), format, args);
        va_end(args);

        if (len < 0) {
            return;
        }
        // Leave room for the newline + NUL.
        if ((size_t)len >= sizeof(buff) - 1) {
            len = sizeof(buff) - 2;
        }
        buff[len++] = '\n';
        buff[len] = '\0';

        timespec now = {};
        clock_gettime(CLOCK_REALTIME, &now);
        const u64 secs = now.tv_sec - bootTimeS;

        if (!g_flags_resolved) {
            if (g_pending_lines.size() < MaxPendingLines) {
                g_pending_lines.emplace_back(secs, std::string(buff, len));
            }
            return;
        }

        EmitLine(secs, buff, len);
    }

    void WriteContextToCsv(const HocClkContext *context) {
        std::scoped_lock lock{ g_csv_mutex };

        FILE *file = fopen(FILE_CONTEXT_CSV_PATH, "a");

        if (file) {
            // Print header
            if (!ftell(file)) {
                fprintf(file, "timestamp,profile,app_tid");

                for (unsigned int module = 0; module < HocClkModule_EnumMax; module++) {
                    fprintf(file, ",%s_hz", hocclkFormatModule((HocClkModule)module, false));
                }

                for (unsigned int sensor = 0; sensor < HocClkThermalSensor_EnumMax; sensor++) {
                    fprintf(file, ",%s_milliC", hocclkFormatThermalSensor((HocClkThermalSensor)sensor, false));
                }

                for (unsigned int module = 0; module < HocClkModule_EnumMax; module++) {
                    fprintf(file, ",%s_real_hz", hocclkFormatModule((HocClkModule)module, false));
                }

                for (unsigned int sensor = 0; sensor < HocClkPowerSensor_EnumMax; sensor++) {
                    fprintf(file, ",%s_mw", hocclkFormatPowerSensor((HocClkPowerSensor)sensor, false));
                }

                fprintf(file, "\n");
            }

            struct timespec now;
            clock_gettime(CLOCK_REALTIME, &now);

            fprintf(file, "%ld%03ld,%s,%016lx", now.tv_sec, now.tv_nsec / 1000000UL, hocclkFormatProfile(context->profile, false),
                    context->applicationId);

            for (unsigned int module = 0; module < HocClkModule_EnumMax; module++) {
                fprintf(file, ",%d", context->freqs[module]);
            }

            for (unsigned int sensor = 0; sensor < HocClkThermalSensor_EnumMax; sensor++) {
                fprintf(file, ",%d", context->temps[sensor]);
            }

            for (unsigned int module = 0; module < HocClkModule_EnumMax; module++) {
                fprintf(file, ",%d", context->realFreqs[module]);
            }

            for (unsigned int sensor = 0; sensor < HocClkPowerSensor_EnumMax; sensor++) {
                fprintf(file, ",%d", context->power[sensor]);
            }

            fprintf(file, "\n");
            fclose(file);
        }
    }

    void SetBootTime() {
        timespec bootTime = {};
        clock_gettime(CLOCK_REALTIME, &bootTime);
        bootTimeS = bootTime.tv_sec;
    }

    void InitializeAsync() {
        Thread initThread = { 0 };
        threadCreate(&initThread, InitializeThreadFunc, NULL, NULL, 0x4000, 0x15, 0);
        threadStart(&initThread);
    }

    Result Initialize() {
        Result rc = 0;

        if (R_SUCCEEDED(rc)) {
            rc = timeInitialize();
        }

        __libnx_init_time();
        timeExit();
        SetBootTime();

        if (R_SUCCEEDED(rc)) {
            rc = fsInitialize();
        }

        if (R_SUCCEEDED(rc)) {
            rc = fsdevMountSdmc();
        }

        if (R_SUCCEEDED(rc)) {
            g_has_initialized = true;
            LogLine("=== hoc-clk " TARGET_VERSION " ===");
            LogLine("by m4xw, natinusala, p-sam, Souldbminer, Lightos_ and Dominatorul");
        }

        return rc;
    }

    void Exit() {
        if (!g_has_initialized) {
            return;
        }

        g_has_initialized = false;
        g_log_enabled = false;
        g_uart_enabled = false;

        fsdevUnmountAll();
        fsExit();
    }

}  // namespace file::utils
