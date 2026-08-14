/*
 * Copyright (C) Switch-OC-Suite
 *
 * Copyright (c) 2023 hanai3Bi
 *
 * Copyright (c) B3711
 *
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
 */

#include <vector>
#include "../pcv.hpp"
#include "../../mtc_timing_value.hpp"
#include "pcv_mariko.hpp"
#include "pcv_mariko_cpu.hpp"
#include "pcv_mariko_gpu.hpp"
#include "pcv_mariko_mtc.hpp"
#include "calculate_timings_mariko.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    u32 *nsoStart;

    namespace {
        size_t g_nso_size = 0;
        uintptr_t g_cave_cursor = 0;
    }

    static uintptr_t CaveReserve(size_t count) {
        if (g_pcv_cave == 0 || g_cave_cursor == 0) {
            return 0;
        }
        if (g_cave_cursor + count * sizeof(u32) > g_pcv_cave + g_pcv_cave_size) {
            return 0;
        }
        const uintptr_t entry = g_cave_cursor;
        g_cave_cursor += count * sizeof(u32);
        return entry;
    }

    #if HOC_UART_LOG
    /* Redirect pcv's NvLog() calls to UART */
    Result NvLogUartRedirect(u32 *ptr) {
        const uintptr_t mapped_nso     = reinterpret_cast<uintptr_t>(nsoStart);
        const size_t    nso_size       = g_nso_size;
        const uintptr_t textEnd        = g_pcv_cave; /* .text ends where the cave begins */
        const uintptr_t vsnprintf_addr = reinterpret_cast<uintptr_t>(ptr);

        /* NvLog via the VDD_SOC log */
        static const char Fmt[] = "%s(%s): DVFS request VDD_SOC %d mV\n";
        constexpr size_t FmtLen = sizeof(Fmt) - 1;
        uintptr_t strAddr = 0;
        {
            const char *hay = reinterpret_cast<const char *>(mapped_nso);
            for (size_t i = 0; i + FmtLen <= nso_size; ++i) {
                if (std::memcmp(hay + i, Fmt, FmtLen) == 0) { strAddr = mapped_nso + i; break; }
            }
        }
        if (strAddr == 0) {
            LOGGING("NvLogRedirect: fmt string not found (vsnprintf@+%lx)", vsnprintf_addr - mapped_nso);
            R_THROW(ldr::ResultInvalidNvLogRedirect());
        }

        uintptr_t nvlog_addr = 0;
        for (u32 *p = nsoStart; reinterpret_cast<uintptr_t>(p + 2) <= textEnd; ++p) {
            const uintptr_t pc = reinterpret_cast<uintptr_t>(p);
            if (!AsmIsAdrp(p[0])) {
                continue;
            }
            const uintptr_t adrpPage = (pc & ~static_cast<uintptr_t>(0xFFFu)) + static_cast<uintptr_t>(AsmAdrpPageOffset(p[0]));
            if (adrpPage != (strAddr & ~static_cast<uintptr_t>(0xFFFu))) {
                continue;
            }
            const u32 reg = asm_get_rd(p[0]);
            if (!(AsmIsAddImm64(p[1]) && asm_get_rd(p[1]) == reg && AsmGetRn(p[1]) == reg && AsmGetImm12(p[1]) == (strAddr & 0xFFFu))) {
                continue;
            }
            for (u32 k = 2; k <= 12 && (pc + (k + 1) * 4) <= textEnd; ++k) {
                if (AsmIsBl(p[k])) { nvlog_addr = AsmBranchTarget(p[k], pc + k * 4); break; }
            }
            if (nvlog_addr != 0) {
                break;
            }
        }
        if (nvlog_addr == 0 || nvlog_addr < mapped_nso || nvlog_addr >= textEnd) {
            LOGGING("NvLogRedirect: NvLog entry not found (fmt@+%lx)", strAddr - mapped_nso);
            R_THROW(ldr::ResultInvalidNvLogRedirect());
        }

        const uintptr_t helper = CaveReserve(40);
        if (helper == 0) {
            LOGGING("NvLogRedirect: cave unavailable (cave=%lx size=%lx)",
                    static_cast<unsigned long>(g_pcv_cave), static_cast<unsigned long>(g_pcv_cave_size));
            R_THROW(ldr::ResultInvalidNvLogRedirect());
        }
        u32 *t = reinterpret_cast<u32 *>(helper);
        size_t n = 0;
        auto emit = [&](u32 ins) { t[n] = ins; ++n; };

        emit(AsmMakeSubImm64(31, 31, 0x200));
        emit(AsmMakeStpImm64(0, 1, 31, 0x100));
        emit(AsmMakeStpImm64(2, 3, 31, 0x110));
        emit(AsmMakeStpImm64(4, 5, 31, 0x120));
        emit(AsmMakeStpImm64(6, 7, 31, 0x130));
        emit(AsmMakeStpqImm(0, 1, 31, 0x140));
        emit(AsmMakeStpqImm(2, 3, 31, 0x160));
        emit(AsmMakeStpqImm(4, 5, 31, 0x180));
        emit(AsmMakeStpqImm(6, 7, 31, 0x1A0));
        emit(AsmMakeStrImm64(30, 31, 0x1E0));
        emit(AsmMakeAddImm64(9, 31, 0x200)); emit(AsmMakeStrImm64(9, 31, 0x1C0)); /* __stack  */
        emit(AsmMakeAddImm64(9, 31, 0x140)); emit(AsmMakeStrImm64(9, 31, 0x1C8)); /* __gr_top */
        emit(AsmMakeAddImm64(9, 31, 0x1C0)); emit(AsmMakeStrImm64(9, 31, 0x1D0)); /* __vr_top */
        emit(AsmMakeMovnW(9, 0x37)); emit(AsmMakeStrImm32(9, 31, 0x1D8)); /* __gr_offs = -56  */
        emit(AsmMakeMovnW(9, 0x7F)); emit(AsmMakeStrImm32(9, 31, 0x1DC)); /* __vr_offs = -128 */
        emit(AsmMakeAddImm64(0, 31, 0x00)); /* mov x0,sp (buf)  */
        emit(AsmMakeMovzW(1, 0x100)); /* size = 0x100     */
        emit(AsmMakeLdrImm64(2, 31, 0x100)); /* fmt (saved x0)   */
        emit(AsmMakeAddImm64(3, 31, 0x1C0)); /* ap               */
        emit(AsmMakeBl(helper + n * 4, vsnprintf_addr));
        emit(AsmMakeMovReg(1, 0)); /* len = retval     */
        emit(AsmMakeCmpImm32(1, 0x100));
        { const size_t at = n; emit(AsmMakeBCond(helper + at * 4, helper + (at + 2) * 4, 0x3u)); } /* b.lo +2 */
        emit(AsmMakeMovzW(1, 0xFF)); /* clamp len        */
        emit(AsmMakeAddImm64(0, 31, 0x00)); /* mov x0,sp (str)  */
        emit(AsmMakeSvc(0x27)); /* svcOutputDebugString */
        emit(AsmMakeLdrImm64(30, 31, 0x1E0));
        emit(AsmMakeAddImm64(31, 31, 0x200));
        emit(RetIns);

        /* Redirect the call sites as patching the actual function causes crash */
        const uintptr_t roStart = g_pcv_cave + g_pcv_cave_size; /* module .rodata start */
        size_t patchedSites = 0;
        for (u32 *p = nsoStart; reinterpret_cast<uintptr_t>(p + 1) <= textEnd; ++p) {
            if (!AsmIsBl(*p)) {
                continue;
            }
            const uintptr_t pc = reinterpret_cast<uintptr_t>(p);
            if (AsmBranchTarget(*p, pc) != nvlog_addr) {
                continue;
            }
            bool isFmtCall = false;
            for (u32 j = 1; j <= 8 && reinterpret_cast<uintptr_t>(p - j) >= reinterpret_cast<uintptr_t>(nsoStart); ++j) {
                const u32 w = *(p - j);
                if (AsmIsAdrp(w) && asm_get_rd(w) == 0) { /* adrp x0,<page> */
                    const uintptr_t wpc = pc - j * 4;
                    const uintptr_t tgtPage = (wpc & ~static_cast<uintptr_t>(0xFFFu)) + static_cast<uintptr_t>(AsmAdrpPageOffset(w));
                    if (tgtPage >= (roStart & ~static_cast<uintptr_t>(0xFFFu))) { isFmtCall = true; break; }
                }
            }
            if (isFmtCall) {
                PATCH_OFFSET(p, AsmMakeBl(pc, helper));
                ++patchedSites;
            }
        }

        LOGGING("NvLogRedirect: stub@+%lx vsnprintf@+%lx helper@+%lx instr=%zu sites=%zu",
                nvlog_addr - mapped_nso, vsnprintf_addr - mapped_nso, helper - mapped_nso, n, patchedSites);
        R_SUCCEED();
    }
    #endif

    /* Relocate C2/C3Bus to avoid issues*/
    Result BusFreqReloc(u32 *ptr) {
        const u32 busReg  = AsmGetRn(ptr[0]); /* ldr Xbuf,[Xbus,#0x10] : bus struct pointer */
        const u32 bufReg  = asm_get_rd(ptr[0]);  /*                        : freq-buffer arg    */
        const u32 bufOff  = AsmGetLdStImm64Off(ptr[0]); /*                        : bus->freqBuf offset */
        const u32 cntReg  = asm_get_rd(ptr[1]); /* add Xcnt,Xbus,#0x18   : arg2 (&count)       */
        const u32 railReg = asm_get_rd(ptr[2]); /* str Xrail,[Xbus,#0x50]: arg0 (rail)         */
        u32 *call = ptr + 3; /* the bl to relocate                          */
        const uintptr_t realFn = AsmBranchTarget(*call, reinterpret_cast<uintptr_t>(call));

        /* Pick 3 scratch registers */
        u32 s[3], sc = 0;
        for (u32 r = 9; r <= 15 && sc < 3; ++r) {
            if (r != busReg && r != bufReg && r != cntReg && r != railReg) {
                s[sc++] = r;
            }
        }
        R_UNLESS(sc == 3, ldr::ResultInvalidBusFreqReloc());

        const uintptr_t tramp = CaveReserve(9);
        R_UNLESS(tramp != 0, ldr::ResultInvalidBusFreqReloc());

        const uintptr_t region = g_pcv_scratch + HocBusFreqBufOffset; /* [0]=counter, +0x10 + i*0x400 = bufs */
        u32 *t = reinterpret_cast<u32 *>(tramp);
        size_t n = 0;
        auto emit = [&](u32 ins) { t[n] = ins; ++n; };
        emit(AsmMakeAdrp(tramp + n * 4, region, s[0])); /* adrp s0,<region>            */
        emit(AsmMakeAddImm64(s[0], s[0], region & 0xFFFu)); /* add  s0,s0,#lo              */
        emit(AsmMakeLdrImm32(s[1], s[0], 0x00)); /* s1 = counter               */
        emit(AsmMakeAddImm64(s[2], s[1], 1)); /* s2 = counter+1             */
        emit(AsmMakeStrImm32(s[2], s[0], 0x00)); /* counter++                  */
        emit(AsmMakeAddImm64(s[0], s[0], 0x10)); /* s0 = region+0x10 (buffers) */
        emit(AsmMakeAddShiftedReg64(bufReg, s[0], s[1], 10)); /* Xbuf = s0 + counter*0x400  */
        emit(AsmMakeStrImm64(bufReg, busReg, bufOff)); /* bus[freqBuf] = Xbuf        */
        emit(AsmMakeB(tramp + n * 4, realFn)); /* tail-call the real function */

        PATCH_OFFSET(call, AsmMakeBl(reinterpret_cast<uintptr_t>(call), tramp));
        const uintptr_t base = reinterpret_cast<uintptr_t>(nsoStart);
        (void) base;
        LOGGING("BusFreqReloc: call@+%lx -> tramp@+%lx realfn@+%lx (bus=x%u buf=x%u off=0x%x scratch=x%u,x%u,x%u)",
                reinterpret_cast<uintptr_t>(call) - base, tramp - base, realFn - base, busReg, bufReg, bufOff, s[0], s[1], s[2]);
        R_SUCCEED();
    }


    #if HOC_UART_LOG
    /* Force GetEffectiveVerbosityLevel to return a non-zero level so all NvLog runs. */
    Result ForceVerbosity(u32 *ptr) {
        if(C.pcvLogVerbosity != 0xff) {
            PATCH_OFFSET(&ptr[0], AsmMakeMovzW(0, static_cast<u16>(C.pcvLogVerbosity))); /* movz w0,#level */
            PATCH_OFFSET(&ptr[1], RetIns);                                                     /* ret            */
        }
        R_SUCCEED();
    }
    #endif

    /* Widen InitDram for a >32-entry EMC DVFS list. Freq array can be dropped to free 264 bytes, relocate the Soc LUT to that space */
    Result EmcSocLutReloc(u32 *ptr) {
        constexpr u32 Window = 48;

        u32 *freqStore = ScanAssembly(ptr - Window, Window, EmcSocFreqStoreAsm, asm_compare_no_rd); /* str x?,[x8,#0x18] */
        u32 *voltStore = ScanAssembly(ptr - Window, Window, EmcSocVoltStoreAsm, asm_compare_no_rd); /* str w?,[x8,#0x48] */
        u32 *readLoad  = ScanAssembly(ptr - Window, Window, EmcSocReadLoadAsm,  asm_compare_no_rd); /* ldr w?,[x9,#0x48] */
        R_UNLESS(freqStore && voltStore && readLoad, ldr::ResultInvalidEmcSocLut());

        u32 *voltBase = voltStore - 2;   /* `add Xb,Xsrc,Xi,LSL#2` (a cmn sits between it and store) */
        R_UNLESS(AsmIsAddShiftedReg64(*voltBase) && asm_get_rd(*voltBase) == AsmGetRn(*voltStore),
                 ldr::ResultInvalidEmcSocLut());

        /* adrp Xl ; add Xl,Xl,#off ; ... ; str Xl,[rail,#0x120] */
        const u32 lutReg  = asm_get_rd(ptr[0]);
        const u32 railReg = AsmGetRn(ptr[0]);
        R_UNLESS(AsmIsAdrp(ptr[-3]) && asm_get_rd(ptr[-3]) == lutReg, ldr::ResultInvalidEmcSocLut());
        R_UNLESS(AsmIsAddImm64(ptr[-2]) && asm_get_rd(ptr[-2]) == lutReg && AsmGetRn(ptr[-2]) == lutReg,
                 ldr::ResultInvalidEmcSocLut());

        const u32 srcBase = AsmGetRn(*voltBase);   /* rail ptr at +0x20 */
        const u32 wBase   = asm_get_rd(*voltBase); /* base reg */
        const u32 wIdx    = AsmGetRm(*voltBase);   /* loop index */

        PATCH_OFFSET(freqStore,     NopIns); /* Unneeded */
        PATCH_OFFSET(voltBase,      AsmMakeLdrImm64(wBase, srcBase, 0x20)); /* ldr Xb,[Xsrc,#0x20] (rail) */
        PATCH_OFFSET(voltStore - 1, AsmMakeAddImm64(wBase, wBase, 0x18));  /* add Xb,Xb,#0x18 (was cmn) */
        PATCH_OFFSET(voltStore,     AsmSetLdStRegOffset(*voltStore, wIdx)); /* str Wv,[Xb,Xi,LSL#2] -> rail+0x18+i*4 */
        PATCH_OFFSET(voltStore + 1, NopIns); /* Unneeded */

        /* rail+0x18 as the socMinLut pointer. */
        PATCH_OFFSET(ptr - 3, NopIns);
        PATCH_OFFSET(ptr - 2, AsmMakeAddImm64(lutReg, railReg, 0x18));/* add Xl,rail,#0x18 */

        /* Drop the abort branch in case of a bad read */
        for (u32 i = 1; i <= 4; ++i) {
            if (AsmIsBCond(readLoad[i])) {
                PATCH_OFFSET(&readLoad[i], NopIns);
                break;
            }
        }
        R_SUCCEED();
    }

    Result EmcDvfsCountLimit(u32 *ptr) {
        R_UNLESS(EmcDvfsCountPatternFn(ptr), ldr::ResultInvalidEmcDvfsCount());

        /* cmp w?,#0x21 -> cmp w?,#EmcDvfsTableEntryCount */
        PATCH_OFFSET(ptr, AsmSubsSetImm12(*ptr, static_cast<u16>(EmcDvfsTableEntryCount)));
        R_SUCCEED();
    }

    Result EmcRateListLimit(u32 *ptr) {
        /* ptr = cmp w?,#0x20 ; ptr[1] = csel w?,w?,w?,lt (w? = min(maxCount, 32)) ; ptr[2] = bl */
        R_UNLESS(EmcRateListPatternFn(ptr), ldr::ResultInvalidEmcRateList());

        /* The csel's Rm holds the 32 cap. */
        const u32 capReg = AsmGetRm(ptr[1]);
        const u32 capMov = AsmMakeMovzW(capReg, 0x20); /* movz w<Rm>,#0x20 */

        u32 *movPtr = nullptr;
        for (u32 i = 1; i <= 16; ++i) {
            if (*(ptr - i) == capMov) {
                movPtr = ptr - i;
                break;
            }
        }
        R_UNLESS(movPtr, ldr::ResultInvalidEmcRateList());

        /* min(maxCount, 32) -> min(maxCount, EmcDvfsTableEntryCount). */
        PATCH_OFFSET(ptr, AsmSubsSetImm12(*ptr, static_cast<u16>(EmcDvfsTableEntryCount)));     /* cmp  w?,#64 */
        PATCH_OFFSET(movPtr, asm_set_imm16(*movPtr, static_cast<u16>(EmcDvfsTableEntryCount))); /* movz w?,#64 */
        R_SUCCEED();
    }

    Result I2cSet_U8(I2cDevice dev, u8 reg, u8 val) {
        struct {
            u8 reg;
            u8 val;
        } __attribute__((packed)) cmd;

        I2cSession _session;
        R_TRY(i2cOpenSession(&_session, dev));

        cmd.reg    = reg;
        cmd.val    = val;
        Result res = i2csessionSendAuto(&_session, &cmd, sizeof(cmd), I2cTransactionOption_All);
        i2csessionClose(&_session);

        return res;
    }

    Result EmcVddqVolt(u32 *ptr) {
        regulator *entry = reinterpret_cast<regulator *>(reinterpret_cast<u8 *>(ptr) - offsetof(regulator, type_2_3.default_uv));

        constexpr u32 uv_step = 5'000;
        constexpr u32 uv_min  = 250'000;

        auto validator = [entry]() {
            R_UNLESS(entry->id               == 2,       ldr::ResultInvalidRegulatorEntry());
            R_UNLESS(entry->type             == 3,       ldr::ResultInvalidRegulatorEntry());
            R_UNLESS(entry->type_2_3.step_uv == uv_step, ldr::ResultInvalidRegulatorEntry());
            R_UNLESS(entry->type_2_3.min_uv  == uv_min,  ldr::ResultInvalidRegulatorEntry());
            R_SUCCEED();
        };

        R_TRY(validator());

        u32 emc_uv = C.marikoEmcVddqVolt;

        if (!emc_uv) {
            R_SKIP();
        }

        if (emc_uv % uv_step) {
            emc_uv = (emc_uv + uv_step - 1) / uv_step * uv_step; // rounding
        }

        PATCH_OFFSET(ptr, emc_uv);

        i2cInitialize();
        Result resultI2C = I2cSet_U8(I2cDevice_Max77812_2, 0x25, (emc_uv - uv_min) / uv_step);
        i2cExit();

        return resultI2C;
    }

    Result GetSocSpeedo(u32 &socSpeedo) {
        constexpr u64 FusePhysicalAddress = 0x7000F000;
        u64 virtualAddress                = 0;
        constexpr u64 Size                = 0x1000;

        u64 outSize;
        /* TODO: use svc::QueryMemoryMapping instead. */
        R_TRY(svcQueryMemoryMapping(&virtualAddress, &outSize, FusePhysicalAddress, Size));

        constexpr u32 FuseOffset      = 2048;
        constexpr u32 SocSpeedoOffset = 308;
        socSpeedo                     = *reinterpret_cast<u32 *>(virtualAddress + FuseOffset + SocSpeedoOffset);

        R_SUCCEED();
    }

    u32 GetSocProcessId(u32 socSpeedo) {
        if (socSpeedo <= 1597) {
            return 0;
        }

        if (socSpeedo <= 1708) {
            return 1;
        }

        /* >= 1709. */
        return 2;
    }

    Result SocVoltAsm(u32 *compareSpeedos) {
        constexpr u32 VoltageScanLimit = 10;
        /* Might actually be speedo id. */
        u32 *writeProcessId = ScanAssembly(compareSpeedos, VoltageScanLimit, SocVoltWriteProcessIdAsm, asm_compare_no_rd);
        R_UNLESS(writeProcessId != nullptr, ldr::ResultInvalidSocVoltPattern());
        u8 writeProcessIdRd = asm_get_rd(*writeProcessId);

        /* This writes 1050mV. */
        u32 *writeVoltage = ScanAssembly(writeProcessId, VoltageScanLimit, SocVoltWriteVoltageAsm, asm_compare_no_rd);
        R_UNLESS(writeVoltage != nullptr, ldr::ResultInvalidSocVoltPattern());
        u8 writeVoltageRd = asm_get_rd(*writeVoltage);

        /* A csel instruction is used to select the soc voltage limit register. */
        /* We care about its destination register since that is used for verification. */
        constexpr u32 VoltageSelectScanLimit = 24;
        u32 *selectVoltage                   = ScanAssembly(writeVoltage, VoltageSelectScanLimit, SocVoltSelectRegisterAsm, AsmCompareCselNoReg);
        R_UNLESS(selectVoltage != nullptr, ldr::ResultInvalidSocVoltPattern());
        /* Todo: check rm and rn? */
        u8 selectVoltageRd = asm_get_rd(*selectVoltage);

        /* rdCsel is then multiplied by 1000 to convert to uV. */
        /* This is pretty far down the function. */
        constexpr u32 MultiplierScanLimit = 200;
        u32 *multiplier                   = ScanAssembly(selectVoltage, MultiplierScanLimit, SocVoltMultiplyVoltsAsm, AsmCompareMullNoReg);
        R_UNLESS(multiplier != nullptr, ldr::ResultInvalidSocVoltPattern());
        u8 multiplierRn = AsmGetMullRn(*multiplier);
        u8 multiplierRm = AsmGetMullRm(*multiplier);
        /* One of the two registers has to be rdCsel. */
        R_UNLESS((multiplierRn == selectVoltageRd) || (multiplierRm == selectVoltageRd), ldr::ResultInvalidSocVoltPattern());
        u8 multiplierRd = asm_get_rd(*multiplier);

        /* Subs instruction is then used to verify against absolute limit. */
        u32 limitValidationPattern = AsmSubsSetRn(SocVoltValidateLimitAsm, multiplierRd);
        u32 *limitValidation = ScanAssembly(multiplier, VoltageScanLimit, limitValidationPattern, AsmSubsCompareNoReg);
        R_UNLESS(limitValidation != nullptr, ldr::ResultInvalidSocVoltPattern());

        /* There is a b.gt instruction right after (checks for socVoltageCap < socVoltageMax). */
        u32 *branchToAbort = limitValidation + 1;
        R_UNLESS(AsmCompareBrConNoImm19(*branchToAbort, SocVoltBranchToAbortAsm), ldr::ResultInvalidSocVoltPattern());

        if (!C.marikoSocVmax || C.marikoSocVmax <= 1000) {
            R_SKIP();
        }

        /* Adjust 1598 speedo minimum to ensure it always goes down process id 0 branch. */
        /* 2200 should be high enough :D */
        u32 compareSpeedosPatch = AsmSubsSetImm12(*compareSpeedos, 2200);
        PATCH_OFFSET(compareSpeedos, compareSpeedosPatch);

        u32 socSpeedo = 0;
        R_TRY(GetSocSpeedo(socSpeedo));

        /* Adjust processId from 0 to [process id of switch booting this]. */
        /* We're overwriting the orr instruction entirly. */
        u32 processId           = GetSocProcessId(socSpeedo);
        u32 writeProcessIdPatch = asm_set_rd(asm_set_imm16(SocVoltWriteVoltageAsm, processId), writeProcessIdRd);
        PATCH_OFFSET(writeProcessId, writeProcessIdPatch);

        /* Adjust voltage limit. */
        u32 voltageLimitPatch = asm_set_rd(asm_set_imm16(SocVoltWriteVoltageAsm, C.marikoSocVmax), writeVoltageRd);
        PATCH_OFFSET(writeVoltage, voltageLimitPatch);

        /* Branches to an abort if limits are invalid -- we patch the branch instruction with NOP. */
        PATCH_OFFSET(branchToAbort, NopIns);

        R_SUCCEED();
    }

    Result SocVoltLimit(u32 *ptr) {
        R_UNLESS(!std::memcmp(ptr - SocVoltLimitMaxDefaultIndex, socVoltLimitArray, sizeof(socVoltLimitArray)), ldr::ResultInvalidSocVoltLimit());
        if (!C.marikoSocVmax || C.marikoSocVmax <= SocVoltLimitOfficial) {
            R_SKIP();
        }

        constexpr u32 Step = 25;
        u32 maxVolt = C.marikoSocVmax;
        if (maxVolt % Step) {
            maxVolt = maxVolt / Step * Step; /* Round. */
        }

        u32 volt = SocVoltLimitOfficial;
        for (u32 i = 1; i < DvfsTableEntryCount - SocVoltLimitMaxDefaultIndex && volt < maxVolt; ++i) {
            volt += Step;
            PATCH_OFFSET(ptr + i, volt);
        }

        R_SUCCEED();
    }

    Result EmcRateSessLimit(u32 *ptr) {
        u32 movzI = 0;
        R_UNLESS(EmcRateSessFindClamp(ptr, nullptr, nullptr, &movzI), ldr::ResultInvalidEmcRateList());

        /* Reject cmd11 GetDvfsTable. */
        for (u32 i = 1; i <= 24; ++i) {
            const u32 w = ptr[i];
            if (AsmIsSubX29Imm(w) && AsmGetImm12(w) >= 0x20u) {  /* sub x?,x29,#>=0x20 */
                R_THROW(ldr::ResultInvalidEmcRateList());
            }
        }

        /*  mov x<desc>,x2 */
        u32 descReg = 0xFFu;
        for (u32 i = 1; i <= 24; ++i) {
            if (AsmIsMovReg(ptr[i], 2)) { descReg = asm_get_rd(ptr[i]); break; }
        }
        R_UNLESS(descReg != 0xFFu, ldr::ResultInvalidEmcRateList());

        /* Repoint the duplicated-imm pair */
        u32 *adds[8]; u32 addImm[8]; u32 nAdds = 0;
        for (u32 i = 1; i <= 24 && nAdds < 8; ++i) {
            const u32 w = ptr[i];
            if (AsmIsAddSpImm(w)) {   /* add x?,sp,#imm12 (shift 0) */
                adds[nAdds]   = ptr + i;
                addImm[nAdds] = AsmGetImm12(w);
                ++nAdds;
            }
        }
        u32 patched = 0;
        for (u32 a = 0; a < nAdds; ++a) {
            bool dup = false;
            for (u32 b = 0; b < nAdds; ++b) {
                if (a != b && addImm[a] == addImm[b]) { dup = true; break; }
            }
            if (dup) {
                PATCH_OFFSET(adds[a], AsmMakeLdrImm64(asm_get_rd(*adds[a]), descReg, 0)); /* ldr x?,[x<desc>] */
                ++patched;
            }
        }
        R_UNLESS(patched == 2, ldr::ResultInvalidEmcRateList());

        /* min(maxCount, 32) -> min(maxCount, EmcDvfsTableEntryCount) */
        PATCH_OFFSET(ptr,         AsmSubsSetImm12(*ptr, static_cast<u16>(EmcDvfsTableEntryCount)));           /* cmp  w?,#64 */
        PATCH_OFFSET(ptr + movzI, asm_set_imm16(*(ptr + movzI), static_cast<u16>(EmcDvfsTableEntryCount)));  /* movz w?,#64 */
        R_SUCCEED();
    }

    void Patch(uintptr_t mapped_nso, size_t nso_size) {
        nsoStart = reinterpret_cast<u32 *>(mapped_nso);

        g_pcv_scratch = mapped_nso + nso_size - HocPcvScratchSize;
        g_nso_size    = nso_size;
        g_cave_cursor = g_pcv_cave;   /* start the .text-cave bump allocator (0 if unavailable) */

        MtcGenerateFreqTables();

        u32 CpuCvbDefaultMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(CpuCvbTableDefault)->freq);
        u32 GpuCvbDefaultMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(GpuCvbTableDefault)->freq);

        PatcherEntry<u32> patches[] = {
            { "CPU Freq Vdd",      &CpuFreqVdd,            1, nullptr,  CpuClkOSLimit              },
            { "CPU Freq Table",     CpuFreqCvbTable<true>, 1, nullptr,  CpuCvbDefaultMaxFreq       },
            { "CPU Volt DVFS",     &CpuVoltDVFS,           1, nullptr,  CpuVminOfficial            },
            { "CPU Volt Thermals", &CpuVoltThermals,       1, nullptr,  CpuVminOfficial            },
            { "CPU Volt Dfll",     &CpuVoltDfll,           1, nullptr,  CpuTune0Low                },
            { "GPU Volt DVFS",     &GpuVoltDVFS,           1, nullptr,  GpuVminOfficial            },
            { "GPU Volt Thermals", &GpuVoltThermals,       1, nullptr,  GpuVminOfficial            },
            { "GPU Freq Table",     GpuFreqCvbTable<true>, 1, nullptr,  GpuCvbDefaultMaxFreq       },
            { "GPU Freq Asm",      &GpuFreqMaxAsm,         2,          &GpuMaxClockPatternFn       },
            { "GPU PLL Max",       &GpuFreqPllMax,         1, nullptr,  GpuClkPllMax               },
            { "GPU PLL Limit",     &GpuFreqPllLimit,       4, nullptr,  GpuClkPllLimit             },
            { "MEM Freq Mtc",      &MemFreqMtcTable,       1, nullptr,  EmcClkOSLimit              },
            { "MEM Freq Dvb",      &MemFreqDvbTable,       1, nullptr,  EmcClkOSLimit              },
            { "MEM Freq Max",      &MemFreqMax,            0, nullptr,  EmcClkOSLimit              },
            { "MEM Freq PLLM",     &MemFreqPllmLimit,      2, nullptr,  EmcClkPllmLimit            },
            { "MEM Vddq",          &EmcVddqVolt,           2, nullptr,  EmcVddqDefault             },
            { "MEM Vdd2",          &MemVoltHandler,        2, nullptr,  MemVdd2Default             },
            { "MEM Table Asm",     &MemMtcTableAsm,        1,          &MemMtcGetGetTablePatternFn },
            { "EMC DVFS Count",    &EmcDvfsCountLimit,     1,          &EmcDvfsCountPatternFn      },
            { "EMC SoC LUT",       &EmcSocLutReloc,        1,          &EmcSocLutPatternFn         },
            { "EMC Rate List",     &EmcRateListLimit,      0,          &EmcRateListPatternFn       },
            { "EMC Rate Sess",     &EmcRateSessLimit,      1,          &EmcRateSessPatternFn       },
            { "Bus Freq Reloc",    &BusFreqReloc,          1,          &BusFreqRelocPatternFn      },
            { "SOC Volt Asm",      &SocVoltAsm,            1,          &SocVoltPatternFn           },
            { "SOC Volt Limit",    &SocVoltLimit,          1, nullptr,  SocVoltLimitOfficial       },
            /* Debugging patches */
            #if HOC_UART_LOG
            { "NvLog Redirect",    &NvLogUartRedirect,     1,          &NvLogVsnprintfPatternFn,   0, 0, true },
            { "Force Verbosity",   &ForceVerbosity,        3,          &ForceVerbosityPatternFn,   0, 0, true },
            #endif
        };

        for (uintptr_t ptr = mapped_nso; ptr <= mapped_nso + nso_size - sizeof(MarikoMtcTable); ptr += sizeof(u32)) {
            u32 *ptr32 = reinterpret_cast<u32 *>(ptr);
            for (auto &entry : patches) {
                if (R_SUCCEEDED(entry.SearchAndApply(ptr32))) {
                    break;
                }
            }
        }

        for (auto &entry : patches) {
            LOGGING("%s Count: %zu", entry.description, entry.patched_count);
            if (R_FAILED(entry.CheckResult())) {
                panic::SmcError(panic::Patch);

                CRASH(entry.description);
            }
        }
    }

}
