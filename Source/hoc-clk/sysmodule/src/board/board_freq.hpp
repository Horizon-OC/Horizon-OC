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
#include <i2c.h>
#include <max17050.h>
#include <switch.h>
#include <tmp451.h>

#include "../file/errors.hpp"
#include "../hos/apm_ext.h"
#include <ipc_server.h>
#include <lockable_mutex.h>


namespace board {

    void SetHz(HocClkModule module, u32 hz);
    void SetMarikoGm20bCutoff(u32 hz);

    u32 GetHz(HocClkModule module);
    u32 GetRealHz(HocClkModule module);
    void GetFreqList(HocClkModule module, u32 *outList, u32 maxCount, u32 *outCount);
    u32 GetHighestDockedDisplayRate();
    void HandleCpuUv();

    void ResetToStock();
    void ResetToStockDisplay();

    template <typename Getter> void ResetToStockModule(Getter getHzFunc, HocClkModule module) {
        Result rc = 0;

        if (hosversionAtLeast(9, 0, 0)) {
            u32 confId = 0;
            rc = apmExtGetCurrentPerformanceConfiguration(&confId);
            ASSERT_RESULT_OK(rc, "apmExtGetCurrentPerformanceConfiguration");

            HocClkApmConfiguration *apmConfiguration = nullptr;
            for (size_t i = 0; hocclk_g_apm_configurations[i].id; ++i) {

                if (hocclk_g_apm_configurations[i].id == confId) {
                    apmConfiguration = &hocclk_g_apm_configurations[i];
                    break;
                }
            }

            if (!apmConfiguration) {
                ERROR_THROW("Unknown apm configuration: %x", confId);
            }

            SetHz(module, getHzFunc(*apmConfiguration));
        } else {
            u32 mode = 0;
            rc = apmExtGetPerformanceMode(&mode);
            ASSERT_RESULT_OK(rc, "apmExtGetPerformanceMode");

            rc = apmExtSysRequestPerformanceMode(mode);
            ASSERT_RESULT_OK(rc, "apmExtSysRequestPerformanceMode");
        }
    }

    inline void ResetToStockCpu() {
        ResetToStockModule([](const HocClkApmConfiguration &cfg) { return cfg.cpu_hz; }, HocClkModule_CPU);
    }

    inline void ResetToStockGpu() {
        ResetToStockModule([](const HocClkApmConfiguration &cfg) { return cfg.gpu_hz; }, HocClkModule_GPU);
    }

    inline void ResetToStockMem() {
        ResetToStockModule([](const HocClkApmConfiguration &cfg) { return cfg.mem_hz; }, HocClkModule_MEM);
    }

}  // namespace board
