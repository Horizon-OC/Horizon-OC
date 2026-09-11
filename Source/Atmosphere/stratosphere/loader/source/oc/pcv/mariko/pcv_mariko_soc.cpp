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
 */

#include "../pcv.hpp"
#include "pcv_mariko_soc.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    namespace {

        struct {
            u32 *site = nullptr;
        } busInitCache;

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

    }

    Result SocVoltAsm(u32 *compareSpeedos) {
        using namespace _asm::field;

        constexpr u32 VoltageScanLimit = 10;
        /* Might actually be speedo id. */
        u32 *writeProcessId = _asm::ScanAssembly(compareSpeedos, VoltageScanLimit, SocVoltWriteProcessIdAsm, Rd);
        R_UNLESS(writeProcessId != nullptr, ldr::ResultInvalidSocVoltPattern());
        u8 writeProcessIdRd = _asm::Get(*writeProcessId, Rd);

        /* This writes 1050mV. */
        u32 *writeVoltage = _asm::ScanAssembly(writeProcessId, VoltageScanLimit, SocVoltWriteVoltageAsm, Rd);
        R_UNLESS(writeVoltage != nullptr, ldr::ResultInvalidSocVoltPattern());
        u8 writeVoltageRd = _asm::Get(*writeVoltage, Rd);

        /* A csel instruction is used to select the soc voltage limit register. */
        /* We care about its destination register since that is used for verification. */
        constexpr u32 VoltageSelectScanLimit = 24;
        u32 *selectVoltage                   = _asm::ScanAssembly(writeVoltage, VoltageSelectScanLimit, SocVoltSelectRegisterAsm, Rd, Rn, Rm);
        R_UNLESS(selectVoltage != nullptr, ldr::ResultInvalidSocVoltPattern());
        /* Todo: check rm and rn? */
        u8 selectVoltageRd = _asm::Get(*selectVoltage, Rd);

        /* rdCsel is then multiplied by 1000 to convert to uV. */
        /* This is pretty far down the function. */
        constexpr u32 MultiplierScanLimit = 200;
        u32 *multiplier                   = _asm::ScanAssembly(selectVoltage, MultiplierScanLimit, SocVoltMultiplyVoltsAsm, Rd, Rn, Rm);
        R_UNLESS(multiplier != nullptr, ldr::ResultInvalidSocVoltPattern());
        u8 multiplierRn = _asm::Get(*multiplier, Rn);
        u8 multiplierRm = _asm::Get(*multiplier, Rm);
        /* One of the two registers has to be rdCsel. */
        R_UNLESS((multiplierRn == selectVoltageRd) || (multiplierRm == selectVoltageRd), ldr::ResultInvalidSocVoltPattern());
        u8 multiplierRd = _asm::Get(*multiplier, Rd);

        /* Subs instruction is then used to verify against absolute limit. */
        /* NB: Rn is written into the pattern and then ignored by the comparison; only Rm is matched. */
        u32 limitValidationPattern = _asm::Set(SocVoltValidateLimitAsm, Rn, multiplierRd);
        u32 *limitValidation = _asm::ScanAssembly(multiplier, VoltageScanLimit, limitValidationPattern, Rd, Rn);
        R_UNLESS(limitValidation != nullptr, ldr::ResultInvalidSocVoltPattern());

        /* There is a b.gt instruction right after (checks for socVoltageCap < socVoltageMax). */
        u32 *branchToAbort = limitValidation + 1;
        R_UNLESS(_asm::Ignoring(*branchToAbort, SocVoltBranchToAbortAsm, Imm19), ldr::ResultInvalidSocVoltPattern());

        if (!C.marikoSocVmax || C.marikoSocVmax <= 1000) {
            R_SKIP();
        }

        /* Adjust 1598 speedo minimum to ensure it always goes down process id 0 branch. */
        /* 2200 should be high enough :D */
        u32 compareSpeedosPatch = _asm::Set(*compareSpeedos, Imm12, 2200);
        PATCH_OFFSET(compareSpeedos, compareSpeedosPatch);

        u32 socSpeedo = 0;
        R_TRY(GetSocSpeedo(socSpeedo));

        /* Adjust processId from 0 to [process id of switch booting this]. */
        /* We're overwriting the orr instruction entirly. */
        u32 processId           = GetSocProcessId(socSpeedo);
        u32 writeProcessIdPatch = _asm::Encode(_asm::op::MovzW, {Rd, writeProcessIdRd}, {Imm16, processId});
        PATCH_OFFSET(writeProcessId, writeProcessIdPatch);

        /* Adjust voltage limit. */
        u32 voltageLimitPatch = _asm::Encode(_asm::op::MovzW, {Rd, writeVoltageRd}, {Imm16, C.marikoSocVmax});
        PATCH_OFFSET(writeVoltage, voltageLimitPatch);

        /* Branches to an abort if limits are invalid -- we patch the branch instruction with NOP. */
        PATCH_OFFSET(branchToAbort, _asm::NopIns);

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

    HOOK_PAYLOAD_FN u64 BusFreqExtendImpl(SharedClkBus *bus) {
        HookPayloadData *data = HOOK_PAYLOAD_PTR(HookPayloadData, m_HookPayloadData);

        u32 *table            = data->busData.next;
        bus->allowedFreqTable = table;
        data->busData.next    = table + EmcDvfsTableEntryCount;

        /* Call the original function. */
        return reinterpret_cast<u64 (*)(SharedClkBus *)>(data->busData.originalFnCallback)(bus);
    }

    /* Relocate and extend C2/C3Bus to avoid issues (likely buffer overflow) */
    Result BusFreqReloc(u32 *ptr) {
        using namespace _asm::field;

        const u32 bus = _asm::Get(ptr[0], Rn);
        R_UNLESS(_asm::IsOp(ptr[1], _asm::op::AddImm64, Rd, Rn, Imm12) && _asm::Get(ptr[1], Imm12) == 0x18 && _asm::Get(ptr[1], Rn) == bus,
                 ldr::ResultInvalidBusFreqReloc()); /* add Xcnt,Xbus,#0x18 */
        R_UNLESS(_asm::IsOp(ptr[2], _asm::op::StrImm64, Rt, Rn, Off8) && _asm::Get(ptr[2], Off8) == 0x50 && _asm::Get(ptr[2], Rn) == bus,
                 ldr::ResultInvalidBusFreqReloc()); /* str Xrail,[Xbus,#0x50] */
        R_UNLESS(_asm::IsOp(ptr[3], _asm::op::Bl, Imm26), ldr::ResultInvalidBusFreqReloc()); /* bl GetDvfsRailUniqueFreqList */

        constexpr u32 PrologueMargin = 140;
        u32 *functionPrologue        = _asm::FindFnPrologue(ptr, PrologueMargin, nsoStart);
        R_UNLESS(functionPrologue != nullptr, ldr::ResultInvalidBusFreqReloc());

        busInitCache.site = functionPrologue;

        R_SUCCEED();
    }

    /* Widen InitDram for a >32-entry EMC DVFS list. Freq array can be dropped to free 264 bytes, relocate the Soc LUT to that space */
    Result EmcSocLutReloc(u32 *ptr) {
        constexpr u32 Window = 48;

        using namespace _asm::field;

        R_UNLESS(_asm::Ignoring(ptr[1], EmcSocLutCountStoreAsm, Rt), ldr::ResultInvalidEmcSocLut()); /* str w?,[x0,#0x154] */

        u32 *freqStore = _asm::ScanAssembly(ptr - Window, Window, EmcSocFreqStoreAsm, Rt); /* str x?,[x8,#0x18] */
        u32 *voltStore = _asm::ScanAssembly(ptr - Window, Window, EmcSocVoltStoreAsm, Rt); /* str w?,[x8,#0x48] */
        u32 *readLoad  = _asm::ScanAssembly(ptr - Window, Window, EmcSocReadLoadAsm,  Rt); /* ldr w?,[x9,#0x48] */
        R_UNLESS(freqStore && voltStore && readLoad, ldr::ResultInvalidEmcSocLut());

        u32 *voltBase = voltStore - 2;   /* `add Xb,Xsrc,Xi,LSL#2` (a cmn sits between it and store) */
        R_UNLESS(_asm::IsOp(*voltBase, _asm::op::AddShifted64, Rd, Rn, Rm, Imm6) && _asm::Get(*voltBase, Rd) == _asm::Get(*voltStore, Rn), ldr::ResultInvalidEmcSocLut());

        /* adrp Xl ; add Xl,Xl,#off ; ... ; str Xl,[rail,#0x120] */
        const u32 lutReg  = _asm::Get(ptr[0], Rt);
        const u32 railReg = _asm::Get(ptr[0], Rn);
        R_UNLESS(_asm::IsOp(ptr[-3], _asm::op::Adrp, Rd, ImmAdrpHi, ImmAdrpLo) && _asm::Get(ptr[-3], Rd) == lutReg, ldr::ResultInvalidEmcSocLut());
        R_UNLESS(_asm::IsOp(ptr[-2], _asm::op::AddImm64, Rd, Rn, Imm12) && _asm::Get(ptr[-2], Rd) == lutReg && _asm::Get(ptr[-2], Rn) == lutReg, ldr::ResultInvalidEmcSocLut());

        const u32 srcBase = _asm::Get(*voltBase, Rn); /* rail ptr at +0x20 */
        const u32 wBase   = _asm::Get(*voltBase, Rd); /* base reg */
        const u32 wIdx    = _asm::Get(*voltBase, Rm); /* loop index */

        PATCH_OFFSET(freqStore,     _asm::NopIns); /* Unneeded */
        PATCH_OFFSET(voltBase,      _asm::Encode(_asm::op::LdrImm64, {Rt, wBase}, {Rn, srcBase}, {Off8, 0x20})); /* ldr Xb,[Xsrc,#0x20] (rail) */
        PATCH_OFFSET(voltStore - 1, _asm::Encode(_asm::op::AddImm64, {Rd, wBase}, {Rn, wBase}, {Imm12, 0x18}));  /* add Xb,Xb,#0x18 (was cmn) */
        PATCH_OFFSET(voltStore,     _asm::ToRegOffset(*voltStore, wIdx));                                  /* str Wv,[Xb,Xi,LSL#2] -> rail+0x18+i*4 */
        PATCH_OFFSET(voltStore + 1, _asm::NopIns); /* Unneeded */

        /* rail+0x18 as the socMinLut pointer. */
        PATCH_OFFSET(ptr - 3, _asm::NopIns);
        PATCH_OFFSET(ptr - 2, _asm::Encode(_asm::op::AddImm64, {Rd, lutReg}, {Rn, railReg}, {Imm12, 0x18})); /* add Xl,rail,#0x18 */

        /* Drop the abort branch in case of a bad read */
        for (u32 i = 1; i <= 4; ++i) {
            if (_asm::IsOp(readLoad[i], _asm::op::BCond, Imm19, BCond)) {
                PATCH_OFFSET(&readLoad[i], _asm::NopIns);
                break;
            }
        }
        R_SUCCEED();
    }

    Result EmcDvfsCountLimit(u32 *ptr) {
        R_UNLESS(_asm::IsOp(*(ptr - 1), _asm::op::Cbz, _asm::field::Imm19, _asm::field::Rt), ldr::ResultInvalidEmcDvfsCount()); /* cbz w?,<skip> */
        R_UNLESS(_asm::Ignoring(*(ptr + 1), _asm::Encode(_asm::op::BCond, {_asm::field::BCond, _asm::cond::Cs}), _asm::field::Imm19),
                 ldr::ResultInvalidEmcDvfsCount()); /* b.cs <abort> */

        /* cmp w?,#0x21 -> cmp w?,#EmcDvfsTableEntryCount */
        PATCH_OFFSET(ptr, _asm::Set(*ptr, _asm::field::Imm12, EmcDvfsTableEntryCount));
        R_SUCCEED();
    }

    Result EmcRateListLimit(u32 *ptr) {
        R_UNLESS(_asm::Ignoring(ptr[1], EmcRateCapCselAsm, _asm::field::Rd, _asm::field::Rn, _asm::field::Rm)
                 && _asm::Get(ptr[0], _asm::field::Rn) == _asm::Get(ptr[1], _asm::field::Rn),
                 ldr::ResultInvalidEmcRateList()); /* csel w?,w?,w?,lt, min(reg, 0x20) */
        R_UNLESS(_asm::IsOp(ptr[2], _asm::op::Bl, _asm::field::Imm26), ldr::ResultInvalidEmcRateList());

        /* The csel's Rm holds the 32 cap. */
        const u32 capReg = _asm::Get(ptr[1], _asm::field::Rm);
        const u32 capMov = _asm::Encode(_asm::op::MovzW, {_asm::field::Rd, capReg}, {_asm::field::Imm16, 0x20}); /* movz w<Rm>,#0x20 */

        u32 *movPtr = nullptr;
        for (u32 i = 1; i <= 16; ++i) {
            if (*(ptr - i) == capMov) {
                movPtr = ptr - i;
                break;
            }
        }
        R_UNLESS(movPtr, ldr::ResultInvalidEmcRateList());

        /* min(maxCount, 32) -> min(maxCount, EmcDvfsTableEntryCount). */
        PATCH_OFFSET(ptr,    _asm::Set(*ptr,    _asm::field::Imm12, EmcDvfsTableEntryCount)); /* cmp  w?,#64 */
        PATCH_OFFSET(movPtr, _asm::Set(*movPtr, _asm::field::Imm16, EmcDvfsTableEntryCount)); /* movz w?,#64 */
        R_SUCCEED();
    }

    Result EmcRateSessLimit(u32 *ptr) {
        u32 movzI = 0;
        R_UNLESS(EmcRateSessFindClamp(ptr, nullptr, nullptr, &movzI), ldr::ResultInvalidEmcRateList());

        /* Reject cmd11 GetDvfsTable. */
        for (u32 i = 1; i <= 24; ++i) {
            const u32 w = ptr[i];
            if (_asm::IsOp(w, _asm::op::SubImm64, _asm::field::Rd, _asm::field::Rn, _asm::field::Imm12)
                && _asm::Get(w, _asm::field::Rn) == _asm::reg::Fp && _asm::Get(w, _asm::field::Imm12) >= 0x20u) {  /* sub x?,x29,#>=0x20 */
                R_THROW(ldr::ResultInvalidEmcRateList());
            }
        }

        /*  mov x<desc>,x2 */
        u32 descReg = 0xFFu;
        for (u32 i = 1; i <= 24; ++i) {
            if (_asm::Ignoring(ptr[i], _asm::MovReg64(0, 2), _asm::field::Rd)) { descReg = _asm::Get(ptr[i], _asm::field::Rd); break; }
        }
        R_UNLESS(descReg != 0xFFu, ldr::ResultInvalidEmcRateList());

        /* Repoint the duplicated-imm pair */
        u32 *adds[8]; u32 addImm[8]; u32 nAdds = 0;
        for (u32 i = 1; i <= 24 && nAdds < 8; ++i) {
            const u32 w = ptr[i];
            if (_asm::IsOp(w, _asm::op::AddImm64, _asm::field::Rd, _asm::field::Rn, _asm::field::Imm12) && _asm::Get(w, _asm::field::Rn) == _asm::reg::Sp) {   /* add x?,sp,#imm12 (shift 0) */
                adds[nAdds]   = ptr + i;
                addImm[nAdds] = _asm::Get(w, _asm::field::Imm12);
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
                PATCH_OFFSET(adds[a], _asm::Encode(_asm::op::LdrImm64, {_asm::field::Rt, _asm::Get(*adds[a], _asm::field::Rd)},
                                                            {_asm::field::Rn, descReg},
                                                            {_asm::field::Off8, 0})); /* ldr x?,[x<desc>] */
                ++patched;
            }
        }
        R_UNLESS(patched == 2, ldr::ResultInvalidEmcRateList());

        /* min(maxCount, 32) -> min(maxCount, EmcDvfsTableEntryCount) */
        PATCH_OFFSET(ptr,         _asm::Set(*ptr,            _asm::field::Imm12, EmcDvfsTableEntryCount)); /* cmp  w?,#64 */
        PATCH_OFFSET(ptr + movzI, _asm::Set(*(ptr + movzI), _asm::field::Imm16, EmcDvfsTableEntryCount)); /* movz w?,#64 */
        R_SUCCEED();
    }

    Result SharedClkBusInstallHooks(HookPayloadData *data) {
        R_UNLESS(busInitCache.site != nullptr, ldr::ResultInvalidBusFreqReloc());

        uintptr_t originalFn = 0;
        R_TRY(INSTALL_IMPL_HOOK_ORIG(busInitCache.site, BusFreqExtendImpl, &originalFn));
        data->busData.originalFnCallback = originalFn;
        data->busData.next               = reinterpret_cast<u32 *>(Hooks().ToVa(data->busData.table[0]));

        R_SUCCEED();
    }

}
