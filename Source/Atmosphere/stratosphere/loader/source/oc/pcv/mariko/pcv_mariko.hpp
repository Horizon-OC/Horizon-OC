/*
 * Copyright (C) Switch-OC-Suite
 *
 * Copyright (c) 2023 hanai3Bi
 *
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
 */

#pragma once

#include "../../oc_common.hpp"
#include "../pcv_common.hpp"
#include "../pcv_asm.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    constexpr u32 BusFreqTableCount = 2;

    struct HookPayloadData {
        struct {
            u32 *next; /* Save the bytes. */
            uintptr_t originalFnCallback;
            u32 table[BusFreqTableCount][EmcDvfsTableEntryCount]; /* Original bus table size: 32. */
        } busData;
#if HOC_UART_LOG
        u32 verbosityLevel;
#endif
    };
    DECLARE_HOOK_PAYLOAD_PTR(HookPayloadData, m_HookPayloadData);

    extern u32 *nsoStart;

    struct DvbEntry {
        u64 freq;
        u32 volt[4] = {};
    };

    constexpr DvbEntry EmcDvbTableDefault[] = {
        {  204000, { 637, 637, 637, } },
        {  408000, { 637, 637, 637, } },
        {  800000, { 637, 637, 637, } },
        { 1065600, { 637, 637, 637, } },
        { 1331200, { 650, 637, 637, } },
        { 1600000, { 675, 650, 637, } },
    };

    /* Movz */
    /*
        SF | OPC                     | HW    | Imm16                                      | RD
        31 | 30 29 28 27 26 25 24 23 | 22 21 | 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 | 4 3 2 1 0
    */
    constexpr u32 SocVoltCompareSpeedoAsm  = 0x7118FAFF; /* subs imm, compares to >=1598 max speedo and then goes down process id 1 route. */
    constexpr u32 SocVoltWriteProcessIdAsm = 0x2A1F03F4; /* orr, writes id 0. */
    constexpr u32 SocVoltWriteVoltageAsm   = 0x52808358; /* Movz imm, writes 1050mV. */
    constexpr u32 SocVoltSelectRegisterAsm = 0x1A9A3118; /* Csel, selects the voltage -- we need the register of this. */
    constexpr u32 SocVoltMultiplyVoltsAsm  = 0x1B1A7F0B; /* Mul, converts from mV -> uV */
    constexpr u32 SocVoltValidateLimitAsm  = 0x6B0A017F; /* Subs, checks limits */
    constexpr u32 SocVoltBranchToAbortAsm  = 0x540020AC; /* B.ge Branches to abort if limits are invalid. */

    ALWAYS_INLINE bool SocVoltPatternFn(u32 *ptr) {
        return _asm::Ignoring(*ptr, SocVoltCompareSpeedoAsm, _asm::field::Rd);
    }

    constexpr u32 SocVoltLimitOfficial                      = 1050;
    constexpr u32 SocVoltLimitMaxDefaultIndex               = 17;
    static const u32 socVoltLimitArray[DvfsTableEntryCount] = { 637, 650, 675, 700, 725, 750, 775, 800, 825, 850, 875, 900, 925, 950, 975, 1000, 1025, 1050, };

    constexpr u32 EmcCountCmpAsm = 0x7100851F; /* cmp w?,#0x21 (subs wzr,w?,#0x21) */

    /*
        str <lut>,[<rail>,#0x120]   ; volt-array pointer
        str w?,  [<rail>,#0x154]    ; num_freqs
    */
    constexpr u32 EmcSocLutPtrStoreAsm = 0xF9009009;   /* str x?,[x0,#0x120] (anchor) */
    constexpr u32 EmcSocLutCountStoreAsm = 0xB9015408; /* str w?,[x0,#0x154] (anchor) */
    constexpr u32 EmcSocFreqStoreAsm = 0xF9000D00;     /* str x?,[x8,#0x18] */
    constexpr u32 EmcSocVoltStoreAsm = 0xB9004900;     /* str w?,[x8,#0x48] (socMinLut[i]) */
    constexpr u32 EmcSocReadLoadAsm = 0xB9404929;      /* ldr w?,[x9,#0x48] (socMinLut) */

    inline bool EmcDvfsCountPatternFn(u32 *ptr) {
        return _asm::Ignoring(*ptr, EmcCountCmpAsm, _asm::field::Rd);
    }

    inline bool EmcSocLutPatternFn(u32 *ptr) {
        return _asm::Ignoring(*ptr, EmcSocLutPtrStoreAsm, _asm::field::Rt); /* str x?,[x0,#0x120] */
    }

    /*
        mov  w?,#0x20            ; the 32 cap
        cmp  w?,#0x20
        csel w?,w?,w?,lt         ; w? = min(maxCount, 32)
        bl   TegraGetEmcDvfsFreqTable

        cmp  w?,#0x20            ; (maxCount)
        csel w?,<same>,<cap>,lt  ; min(maxCount, 32)
        bl   <_asm::Get*DvfsFreqTable>
    */
    constexpr u32 EmcRateCapCmpAsm = 0x710082FF;  /* cmp  w?,#0x20 */
    constexpr u32 EmcRateCapCselAsm = 0x1A80B000; /* csel w?,w?,w?,lt */

    inline bool EmcRateListPatternFn(u32 *ptr) {
        return _asm::Ignoring(*ptr, EmcRateCapCmpAsm, _asm::field::Rd, _asm::field::Rn);
    }

    constexpr u32 EmcRateSessCmpAsm  = 0x710082FF; /* cmp  w?,#0x20 */
    constexpr u32 EmcRateSessMovAsm  = 0x52800400; /* movz w?,#0x20 */
    constexpr u32 EmcRateSessCselAsm = 0x1A80B000; /* csel w?,w?,w?,lt (opcode + cond) */

    inline bool EmcRateSessFindClamp(u32 *ptr, u32 *out_c, u32 *out_cap, u32 *out_movz_i) {
        const u32 c = _asm::Get(ptr[0], _asm::field::Rn);
        for (u32 i = 1; i <= 14; ++i) {
            const u32 w = ptr[i];
            if (_asm::Ignoring(w, EmcRateSessCselAsm, _asm::field::Rd, _asm::field::Rn, _asm::field::Rm) && _asm::Get(w, _asm::field::Rn) == c && _asm::Get(w, _asm::field::Rd) == c) {
                const u32 cap = _asm::Get(w, _asm::field::Rm);
                for (u32 j = 1; j < i; ++j) {
                    if (_asm::Ignoring(ptr[j], EmcRateSessMovAsm, _asm::field::Rd) && _asm::Get(ptr[j], _asm::field::Rd) == cap) {
                        if (out_c)      *out_c      = c;
                        if (out_cap)    *out_cap    = cap;
                        if (out_movz_i) *out_movz_i = j;
                        return true;
                    }
                }
                return false;
            }
        }
        return false;
    }

    inline bool EmcRateSessPatternFn(u32 *ptr) {
        return _asm::Ignoring(*ptr, EmcRateSessCmpAsm, _asm::field::Rd, _asm::field::Rn);
    }

    inline bool BusFreqRelocPatternFn(u32 *ptr) {
        if (g_pcv_cave == 0) {
            return false;
        }
        if (reinterpret_cast<uintptr_t>(ptr + 4) > g_pcv_cave) {   /* the call site lives in .text */
            return false;
        }
        return _asm::IsOp(*ptr, _asm::op::LdrImm64, _asm::field::Rt, _asm::field::Rn, _asm::field::Off8)
               && _asm::Get(*ptr, _asm::field::Off8) == 0x10; /* ldr Xbuf,[Xbus,#0x10] */
    }

    inline bool ForceVerbosityPatternFn(u32 *ptr) {
        if (g_pcv_cave == 0) {
            return false;
        }
        if (reinterpret_cast<uintptr_t>(ptr + 11) > g_pcv_cave) {   /* .text only */
            return false;
        }
        return _asm::IsFramePushPre(*ptr);
    }

    /*
        sub  sp,  sp, #imm
        stp  x29,x30,[sp,#imm]
        str  x?, [sp,#imm]
        add  x29,sp, #imm
        subs xzr, x1, #0
    */
    constexpr u32 NvLogSubSpAsm    = 0xD10003FFu; /* sub  sp,sp,#0 */
    constexpr u32 NvLogStpFpLrAsm  = 0xA9007BFDu; /* stp  x29,x30,[sp,#0] */
    constexpr u32 NvLogStrSpillAsm = 0xF90003E0u; /* str  x?,[sp,#0] */
    constexpr u32 NvLogMovFpAsm    = 0x910003FDu; /* add  x29,sp,#0 */
    constexpr u32 NvLogCmpSizeAsm  = 0xF100003Fu; /* subs xzr,x1,#0 */

    inline bool NvLogVsnprintfPatternFn(u32 *ptr) {
        if (HOC_UART_LOG == 0 || g_pcv_cave == 0) {
            return false;
        }
        if (reinterpret_cast<uintptr_t>(ptr + 5) > g_pcv_cave) {   /* must sit in .text */
            return false;
        }
        return _asm::Ignoring(*ptr, NvLogSubSpAsm, _asm::field::Imm12);
    }

    void Patch(uintptr_t mapped_nso, size_t nso_size);

}
