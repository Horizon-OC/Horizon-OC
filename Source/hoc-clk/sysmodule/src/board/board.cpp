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

#include <battery.h>
#include <hocclk.h>
#include <i2c.h>
#include <max17050.h>
#include <notification.h>
#include <pwm.h>
#include <registers.h>
#include <switch.h>
#include <t210.h>
#include <tmp451.h>

#include "../bpmp/bpmp.hpp"
#include "../display/display_refresh_rate.hpp"
#include "../file/file_utils.hpp"
#include "../hos/apm_ext.h"
#include "../hos/integrations.hpp"
#include "../hos/rgltr.h"
#include "../tsensor/aotag.hpp"
#include "../tsensor/soctherm.hpp"
#include "board.hpp"
#include "board_fuse.hpp"
#include "board_load.hpp"
#include "board_misc.hpp"
#include "board_volt.hpp"
#include <ipc_server.h>
#include <lockable_mutex.h>

namespace board {

    u64 clkVirtAddr, dsiVirtAddr, apbVirtAddr, fuseVirtAddr, sysVirtAddr, actmonVirtAddr;

    HocClkSocType gSocType;
    u8 gDramID;
    HocClkConsoleType gConsoleType = HocClkConsoleType_Icosa;
    FuseData fuseData;
    u8 speedoBracket;
    PwmChannelSession iCon;

    u32 fd = 0, fd2 = 0;

#define PMC_BASE 0x7000E400
#define APB_MISC_GP_HIDREV 0x804
#define GP_HIDREV_MAJOR_T210 0x1
#define GP_HIDREV_MAJOR_T210B01 0x2
#define APB_BASE 0x70000000
#define FUSE_RESERVED_ODMX(x) (0x1C8 + 4 * (x))
#define FUSE_OFFSET 0x800
    void FetchHardwareInfos() {
        ReadFuses(fuseData, fuseVirtAddr);
        SetGpuBracket(fuseData.gpuSpeedo, speedoBracket);

        u32 hidrev = *(u32 *)(apbVirtAddr + APB_MISC_GP_HIDREV);
        if (((hidrev >> 4) & 0xF) >= GP_HIDREV_MAJOR_T210B01) {
            gSocType = HocClkSocType_Mariko;
        } else {
            gSocType = HocClkSocType_Erista;
        }

        u32 odm4 = *(u32 *)(fuseVirtAddr + FUSE_OFFSET + FUSE_RESERVED_ODMX(4));

        if (gSocType == HocClkSocType_Mariko) {
            switch ((odm4 & 0xF0000) >> 16) {
                case 2:
                    gConsoleType = HocClkConsoleType_Hoag;
                    break;
                case 4:
                    gConsoleType = HocClkConsoleType_Aula;
                    break;
                case 1:
                default:
                    gConsoleType = HocClkConsoleType_Iowa;
            }
        } else {
            gConsoleType = HocClkConsoleType_Icosa;
        }

        gDramID = (odm4 & 0xF8) >> 3;
        // Get extended dram id info.
        if (gSocType == HocClkSocType_Mariko) {
            gDramID |= (odm4 & 0x7000) >> 7;
        }
    }

    /* TODO: Check for config */
    void Initialize() {
        Result rc = 0;

        if (HOSSVC_HAS_CLKRST) {
            rc = clkrstInitialize();
            ASSERT_RESULT_OK(rc, "clkrstInitialize");
        } else {
            rc = pcvInitialize();
            ASSERT_RESULT_OK(rc, "pcvInitialize");
        }

        rc = apmExtInitialize();
        ASSERT_RESULT_OK(rc, "apmExtInitialize");

        rc = psmInitialize();
        ASSERT_RESULT_OK(rc, "psmInitialize");

        if (HOSSVC_HAS_TC) {
            rc = tcInitialize();
            ASSERT_RESULT_OK(rc, "tcInitialize");
        }

        rc = max17050Initialize();
        ASSERT_RESULT_OK(rc, "max17050Initialize");

        rc = tmp451Initialize();
        ASSERT_RESULT_OK(rc, "tmp451Initialize");

        rc = pmdmntInitialize();
        ASSERT_RESULT_OK(rc, "pmdmntInitialize");

        rc = rgltrInitialize();
        ASSERT_RESULT_OK(rc, "rgltrInitialize");

        rc = QueryMemoryMapping(&clkVirtAddr, 0x60006000, 0x1000);
        ASSERT_RESULT_OK(rc, "QueryMemoryMapping (clk)");

        rc = QueryMemoryMapping(&dsiVirtAddr, 0x54300000, 0x40000);
        ASSERT_RESULT_OK(rc, "QueryMemoryMapping (dsi)");

        rc = QueryMemoryMapping(&apbVirtAddr, 0x70000000, 0x1000);
        ASSERT_RESULT_OK(rc, "QueryMemoryMapping (apb)");

        rc = QueryMemoryMapping(&fuseVirtAddr, 0x7000F000, 0x1000);
        ASSERT_RESULT_OK(rc, "QueryMemoryMapping (fuse)");

        rc = QueryMemoryMapping(&sysVirtAddr, 0x6000C000, 0x1000);
        ASSERT_RESULT_OK(rc, "QueryMemoryMapping (sys)");
        actmonVirtAddr = sysVirtAddr + 0x800;

        FetchHardwareInfos();

        Result nvCheck = 1;
        if (R_SUCCEEDED(nvInitialize())) {
            nvCheck = nvOpen(&fd, "/dev/nvhost-ctrl-gpu");
            Result nvCheck_sched = nvOpen(&fd2, "/dev/nvsched-ctrl");
            /* This can be improved. */
            NvSchedSucceed(nvCheck_sched);

            if (R_SUCCEEDED(nvCheck_sched)) {
                SchedSetFD2(fd2);
            }
        }

        StartLoad(nvCheck, fd);

        batteryInfoInitialize();

        tsensor::InitializeSoctherm();  // SOCTHERM must be init before AOTAG

        Result bpmpfwRc = bpmp::StartBpmfwExecution();
        if (R_FAILED(bpmpfwRc)) {
            fileUtils::LogLine("[bpmp] StartBpmfwExecution failed: 0x%x", bpmpfwRc);
        }

        // PMC exosphere check
        SecmonArgs args = {};
        args.X[0] = 0xF0000002;
        args.X[1] = PMC_BASE;
        svcCallSecureMonitor(&args);

        if (args.X[1] != PMC_BASE) {  // if param 1 is identical read failed
            tsensor::InitializeAotag(GetSocType() == HocClkSocType_Mariko);
        }

        Result pwmCheck = 1;
        if (hosversionAtLeast(6, 0, 0) && R_SUCCEEDED(pwmInitialize())) {
            pwmCheck = pwmOpenSession2(&iCon, 0x3D000001);
        }

        StartMiscThread(pwmCheck, &iCon);

        display::DisplayRefreshConfig cfg = { .clkVirtAddr = clkVirtAddr,
                                              .dsiVirtAddr = dsiVirtAddr,
                                              .isLite = (GetConsoleType() == HocClkConsoleType_Hoag),
                                              .isRetroSUPER = false };
        display::Initialize(&cfg);

        CacheDfllData();
        CacheGpuVoltTable();
    }

    void Exit() {
        if (HOSSVC_HAS_CLKRST) {
            clkrstExit();
        } else {
            pcvExit();
        }

        apmExtExit();
        psmExit();
        rgltrExit();
        if (HOSSVC_HAS_TC) {
            tcExit();
        }

        max17050Exit();
        tmp451Exit();
        display::Shutdown();

        ExitLoad();

        ExitMiscThread();

        pwmChannelSessionClose(&iCon);
        pwmExit();
        batteryInfoExit();
        pmdmntExit();
        nvExit();
    }

    HocClkSocType GetSocType() {
        return gSocType;
    }

    HocClkConsoleType GetConsoleType() {
        return gConsoleType;
    }

    u8 GetDramID() {
        return gDramID;
    }

    bool IsDram8GB() {
        SecmonArgs args = {};
        args.X[0] = 0xF0000002;
        args.X[1] = MC_REGISTER_BASE + MC_EMEM_CFG_0;
        svcCallSecureMonitor(&args);

        if (args.X[1] == (MC_REGISTER_BASE + MC_EMEM_CFG_0)) {  // if param 1 is identical read failed
            notification::writeNotification("Horizon OC\nSecmon read failed!\n This may be a hardware issue!");
            return false;
        }

        return args.X[1] == 0x00002000 ? true : false;
    }

    /* TODO: Put this into a different file. */
    void SetDisplayRefreshDockedState(bool docked) {
        if (GetConsoleType() != HocClkConsoleType_Hoag) {
            display::SetDockedState(docked);
        }
    }

    FuseData *GetFuseData() {
        return &fuseData;
    }

    u8 GetGpuSpeedoBracket() {
        return speedoBracket;
    }

}  // namespace board
