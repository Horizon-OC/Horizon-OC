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

#include <cstdio>
#include <cstring>
#include <switch.h>

#include "../board/board.hpp"
#include "../file/file_utils.hpp"
#include "../mapping/mem_map.hpp"
#include "bpmp.hpp"

namespace bpmp {

    namespace {

        constexpr u64 FlowCtlrPhysBase = 0x60007000;
        constexpr u64 EvpPhysBase = 0x6000F000;

        constexpr u32 ClkRstRstDevLSet = 0x300;
        constexpr u32 ClkRstRstDevLClr = 0x304;
        constexpr u32 RstDevLCopRstBit = (1u << 1); /* SWR_COP_RST */

        constexpr u32 ApbMiscSlaveSecurityReg0 = 0xC00;
        constexpr u32 ApbMiscSlaveSecurityReg1 = 0xC04;
        constexpr u32 ApbMiscSlaveSecurityReg2 = 0xC08;

        /* Relative to actmonVirtAddr */
        constexpr u32 ActmonCopCtrl       = 0xC0;
        constexpr u32 ActmonCopIntrStatus = 0xE4;

        constexpr u32 EvpCopResetVector         = 0x200;
        constexpr u32 EvpCopUndefVector         = 0x204;
        constexpr u32 EvpCopSwiVector           = 0x208;
        constexpr u32 EvpCopPrefetchAbortVector = 0x20C;
        constexpr u32 EvpCopDataAbortVector     = 0x210;
        constexpr u32 EvpCopRsvdVector          = 0x214;
        constexpr u32 EvpCopIrqVector           = 0x218;
        constexpr u32 EvpCopFiqVector           = 0x21C;

        constexpr u32 FlowCtlrHaltCopEvents = 0x004;
        constexpr u32 FlowModeNone          = 0x00000000;

        constexpr u64 FwIramPhysBase = 0x40004000;
        constexpr u32 FwStagingSize  = 0x8000;

        alignas(4096) u8 s_fwStageBuf[FwStagingSize];

        inline volatile u32 &Mmio(u64 base_va, u32 offset) {
            return *reinterpret_cast<volatile u32 *>(base_va + offset);
        }

        // Late in the boot chain, otherwise a fatal will occur from invalid boot state
        constexpr u32 pscDependencies[] = { PscPmModuleId_Olsc };
        constexpr PscPmModuleId PscModuleId = (PscPmModuleId)705;

        PscPmModule s_pscModule;
        bool s_pscPrepared = false;
        Thread s_pscThread;
        bool s_pscExit = false;

        void PscThreadFunc(void *) {
            while (!s_pscExit) {
                Result rc = eventWait(&s_pscModule.event, 1'000'000'000ULL);
                if (R_FAILED(rc)) {
                    continue; // Timeout
                }

                PscPmState state;
                u32 flags;
                rc = pscPmModuleGetRequest(&s_pscModule, &state, &flags);
                if (R_SUCCEEDED(rc)) {
                    if (state == PscPmState_Awake) {
                        Result wrc = StartBpmfwExecution();
                        if (R_FAILED(wrc)) {
                            fileUtils::LogLine("[bpmp] restart after wake failed: 0x%x", wrc);
                        }
                    }

                    pscPmModuleAcknowledge(&s_pscModule, state);
                }
            }
        }

    } // namespace

    Result StartBpmfwExecution() {
        if (!IsPatchedExosphere()) {
            fileUtils::LogLine("[bpmp] Cannot start without patched exosphere");
            return (Result)1;
        }

        std::memset(s_fwStageBuf, 0, sizeof(s_fwStageBuf));

        FILE *f = fopen("sdmc:/config/horizon-oc/bpmpfw.bin", "rb");
        if (!f) {
            fileUtils::LogLine("[bpmp] StartBpmfwExecution: bpmpfw.bin not found");
            return (Result)1;
        }

        const size_t fw_size = fread(s_fwStageBuf, 1, sizeof(s_fwStageBuf), f);
        fclose(f);

        if (fw_size == 0) {
            fileUtils::LogLine("[bpmp] StartBpmfwExecution: bpmpfw.bin is empty");
            return (Result)1;
        }

        fileUtils::LogLine("[bpmp] StartBpmfwExecution: loaded %zu bytes, loading at 0x%llx",
                            fw_size, static_cast<unsigned long long>(FwIramPhysBase));

        /* Set BPMP reset */
        Mmio(board::clkVirtAddr, ClkRstRstDevLSet) = RstDevLCopRstBit;

        /* Grant BPMP access to all a bunch of registers that are useful */
        Mmio(board::apbVirtAddr, ApbMiscSlaveSecurityReg0) = 0;
        Mmio(board::apbVirtAddr, ApbMiscSlaveSecurityReg1) = 0;
        Mmio(board::apbVirtAddr, ApbMiscSlaveSecurityReg2) = 0;

        /* Set up exeption vectors for bpmp */
        const u32 fw_reset_vector = static_cast<u32>(FwIramPhysBase + 0x0);
        const u32 fw_panic_vector = static_cast<u32>(FwIramPhysBase + 0x4);

        SmcReadWriteRegister(EvpPhysBase + EvpCopResetVector,         0xFFFFFFFF, fw_reset_vector);
        SmcReadWriteRegister(EvpPhysBase + EvpCopUndefVector,         0xFFFFFFFF, fw_panic_vector);
        SmcReadWriteRegister(EvpPhysBase + EvpCopSwiVector,           0xFFFFFFFF, fw_panic_vector);
        SmcReadWriteRegister(EvpPhysBase + EvpCopPrefetchAbortVector, 0xFFFFFFFF, fw_panic_vector);
        SmcReadWriteRegister(EvpPhysBase + EvpCopDataAbortVector,     0xFFFFFFFF, fw_panic_vector);
        SmcReadWriteRegister(EvpPhysBase + EvpCopRsvdVector,          0xFFFFFFFF, fw_panic_vector);
        SmcReadWriteRegister(EvpPhysBase + EvpCopIrqVector,           0xFFFFFFFF, fw_panic_vector);
        SmcReadWriteRegister(EvpPhysBase + EvpCopFiqVector,           0xFFFFFFFF, fw_panic_vector);

        /* Disable actmon monitoring to avoid hang on FW >4.0.0 */
        Mmio(board::actmonVirtAddr, ActmonCopCtrl) = 0;
        Mmio(board::actmonVirtAddr, ActmonCopIntrStatus) = Mmio(board::actmonVirtAddr, ActmonCopIntrStatus);

        /* Load the BPMPFW into iram per page. TODO: determine if this is nessesary */
        constexpr size_t PageSize = 4096;
        for (size_t ofs = 0; ofs < fw_size; ofs += PageSize) {
            const size_t chunk = (fw_size - ofs) < PageSize ? (fw_size - ofs) : PageSize;

            Result copy_rc = SmcCopyToIram(FwIramPhysBase + ofs, s_fwStageBuf + ofs, static_cast<u32>(chunk));
            if (R_FAILED(copy_rc)) {
                fileUtils::LogLine("[bpmp] SmcCopyToIram failed at +0x%zx: 0x%x", ofs, copy_rc);
                return copy_rc;
            }
        }

        /* Clear BPMP reset. */
        Mmio(board::clkVirtAddr, ClkRstRstDevLClr) = RstDevLCopRstBit;

        /* Clear the halt written by bootloader. */
        SmcReadWriteRegister(FlowCtlrPhysBase + FlowCtlrHaltCopEvents, 0xFFFFFFFF, FlowModeNone);

        return 0;
    }

    void StartSleepMonitorThread() {
        if (!IsPatchedExosphere()) {
            fileUtils::LogLine("[bpmp] Cant start BPMP without exosphere patch");
            return;
        }

        Result rc = pscmInitialize();
        if (R_FAILED(rc)) {
            fileUtils::LogLine("[bpmp] pscmInitialize failed: 0x%x", rc);
            return;
        }

        rc = pscmGetPmModule(&s_pscModule, PscModuleId, pscDependencies, sizeof(pscDependencies) / sizeof(u32), true);
        if (R_FAILED(rc)) {
            fileUtils::LogLine("[bpmp] pscmGetPmModule failed: 0x%x", rc);
            pscmExit();
            return;
        }
        s_pscPrepared = true;

        rc = threadCreate(&s_pscThread, PscThreadFunc, nullptr, NULL, 0x1000, 0x10, 3);
        if (R_FAILED(rc)) {
            fileUtils::LogLine("[bpmp] failed to create psc thread: 0x%x", rc);
            return;
        }
        threadStart(&s_pscThread);
    }

    void StopSleepMonitorThread() {
        if (!s_pscPrepared) {
            return;
        }

        s_pscExit = true;
        eventFire(&s_pscModule.event);
        threadWaitForExit(&s_pscThread);
        threadClose(&s_pscThread);

        pscPmModuleFinalize(&s_pscModule);
        pscPmModuleClose(&s_pscModule);
        pscmExit();
        s_pscPrepared = false;
    }

} // namespace bpmp
