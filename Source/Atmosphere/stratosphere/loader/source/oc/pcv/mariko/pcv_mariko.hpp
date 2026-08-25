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
    constexpr u32 EmcSocReadLoadAsm = 0xB9404929;      /* ldr w?,[x9,#0x48] (socMinLut readback) */

    inline bool EmcDvfsCountPatternFn(u32 *ptr) {
        /* Local context: cbz w?,<skip> ; cmp w?,#0x21 ; b.cs <abort> */
        return _asm::Ignoring(*ptr, EmcCountCmpAsm, _asm::field::Rd)
               && _asm::Ignoring(*(ptr + 1), _asm::Encode(_asm::op::BCond, {_asm::field::BCond, _asm::cond::Cs}), _asm::field::Imm19) /* b.cs */
               && _asm::IsOp(*(ptr - 1), _asm::op::Cbz, _asm::field::Imm19, _asm::field::Rt);                             /* cbz */
    }

    inline bool EmcSocLutPatternFn(u32 *ptr) {
        return _asm::Ignoring(*ptr, EmcSocLutPtrStoreAsm, _asm::field::Rt)             /* str x?,[x0,#0x120] */
               && _asm::Ignoring(*(ptr + 1), EmcSocLutCountStoreAsm, _asm::field::Rt); /* str w?,[x0,#0x154] */
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
        return _asm::Ignoring(*ptr, EmcRateCapCmpAsm, _asm::field::Rd, _asm::field::Rn)                            /* cmp w?,#0x20 */
               && _asm::Ignoring(*(ptr + 1), EmcRateCapCselAsm, _asm::field::Rd, _asm::field::Rn, _asm::field::Rm)       /* csel w?,w?,w?,lt */
               && (_asm::Get(*ptr, _asm::field::Rn) == _asm::Get(*(ptr + 1), _asm::field::Rn))                           /* min(reg, 0x20) */
               && _asm::IsOp(*(ptr + 2), _asm::op::Bl, _asm::field::Imm26);                                        /* bl <_asm::Get*DvfsFreqTable> */
    }

    constexpr u32 EmcRateSessCmpAsm  = 0x710082FF; /* cmp  w?,#0x20 */
    constexpr u32 EmcRateSessMovAsm  = 0x52800400; /* movz w?,#0x20 */
    constexpr u32 EmcRateSessCselAsm = 0x1A80B000; /* csel w?,w?,w?,lt (opcode + cond) */

    inline bool EmcRateSessFindClamp(u32 *ptr, u32 *out_c, u32 *out_cap, u32 *out_movz_i) {
        if (!_asm::Ignoring(ptr[0], EmcRateSessCmpAsm, _asm::field::Rd, _asm::field::Rn)) return false;   /* cmp w<c>,#0x20 */
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
        return EmcRateSessFindClamp(ptr, nullptr, nullptr, nullptr);
    }

    inline bool BusFreqRelocPatternFn(u32 *ptr) {
        if (g_pcv_scratch == 0 || g_pcv_cave == 0) {
            return false;
        }
        if (reinterpret_cast<uintptr_t>(ptr + 4) > g_pcv_cave) {   /* the call site lives in .text */
            return false;
        }
        if (!(_asm::IsOp(ptr[0], _asm::op::LdrImm64, _asm::field::Rt, _asm::field::Rn, _asm::field::Off8)  && _asm::Get(ptr[0], _asm::field::Off8)  == 0x10)) return false; /* ldr Xbuf,[Xbus,#0x10] */
        if (!(_asm::IsOp(ptr[1], _asm::op::AddImm64, _asm::field::Rd, _asm::field::Rn, _asm::field::Imm12) && _asm::Get(ptr[1], _asm::field::Imm12) == 0x18)) return false; /* add Xcnt,Xbus,#0x18   */
        if (!(_asm::IsOp(ptr[2], _asm::op::StrImm64, _asm::field::Rt, _asm::field::Rn, _asm::field::Off8)  && _asm::Get(ptr[2], _asm::field::Off8)  == 0x50)) return false; /* str Xrail,[Xbus,#0x50]*/
        if (!_asm::IsOp(ptr[3], _asm::op::Bl, _asm::field::Imm26))                                                                    return false; /* bl GetDvfsRailUnique  */
        const u32 bus = _asm::Get(ptr[0], _asm::field::Rn);
        return _asm::Get(ptr[1], _asm::field::Rn) == bus && _asm::Get(ptr[2], _asm::field::Rn) == bus;
    }

    inline bool ForceVerbosityPatternFn(u32 *ptr) {
        if (g_pcv_cave == 0) {
            return false;
        }
        if (reinterpret_cast<uintptr_t>(ptr + 11) > g_pcv_cave) {   /* .text only */
            return false;
        }
        if (ptr[0] != 0xA9BE7BFDu || ptr[1] != 0xF9000BF3u || ptr[2] != 0x910003FDu) return false; /* stp/str/mov x29,sp */
        if (!(_asm::IsOp(ptr[3], _asm::op::AddImm64, _asm::field::Rd, _asm::field::Rn, _asm::field::Imm12) && _asm::Get(ptr[3], _asm::field::Rd) == 0  && _asm::Get(ptr[3], _asm::field::Rn) == _asm::reg::Fp)) return false; /* add x0,x29,#imm  */
        if (!(_asm::IsOp(ptr[4], _asm::op::AddImm64, _asm::field::Rd, _asm::field::Rn, _asm::field::Imm12) && _asm::Get(ptr[4], _asm::field::Rd) == 19 && _asm::Get(ptr[4], _asm::field::Rn) == _asm::reg::Fp)) return false; /* add x19,x29,#imm */
        if (_asm::Get(ptr[3], _asm::field::Imm12) != _asm::Get(ptr[4], _asm::field::Imm12) || !_asm::IsOp(ptr[5], _asm::op::Bl, _asm::field::Imm26)) return false;
        for (u32 j = 6; j <= 10; ++j) {
            if (ptr[j] == 0x7100001Fu) { /* cmp w0,#0 */
                return true;
            }
        }
        return false;
    }

    /* vsnprintf(buf,size,fmt,va_list) prologue */
    inline constexpr u32 NvLogVsnSig[] = { 0xD10483FFu, 0xA9107BFDu, 0xF9008BFCu, 0x910403FDu, 0xF100003Fu };

    inline bool NvLogVsnprintfPatternFn(u32 *ptr) {
        if (HOC_UART_LOG == 0 || g_pcv_cave == 0) {
            return false;
        }
        if (reinterpret_cast<uintptr_t>(ptr + std::size(NvLogVsnSig)) > g_pcv_cave) {   /* must sit in .text */
            return false;
        }
        for (size_t k = 0; k < std::size(NvLogVsnSig); ++k) {
            if (ptr[k] != NvLogVsnSig[k]) {
                return false;
            }
        }
        return true;
    }

    void Patch(uintptr_t mapped_nso, size_t nso_size);

}
