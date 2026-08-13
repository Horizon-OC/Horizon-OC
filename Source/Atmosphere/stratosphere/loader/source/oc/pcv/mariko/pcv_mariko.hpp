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
        return asm_compare_no_rd(*ptr, SocVoltCompareSpeedoAsm);
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
        return asm_compare_no_rd(*ptr, EmcCountCmpAsm) && AsmCompareBrConNoImm19(*(ptr + 1), 0x54000002) /* b.cs */
               && AsmCbzCompareOpcodeOnly(*(ptr - 1), 0x34000000);                                       /* cbz */
    }

    inline bool EmcSocLutPatternFn(u32 *ptr) {
        return asm_compare_no_rd(*ptr, EmcSocLutPtrStoreAsm)             /* str x?,[x0,#0x120] */
               && asm_compare_no_rd(*(ptr + 1), EmcSocLutCountStoreAsm); /* str w?,[x0,#0x154] */
    }

    /*
        mov  w?,#0x20            ; the 32 cap
        cmp  w?,#0x20
        csel w?,w?,w?,lt         ; w? = min(maxCount, 32)
        bl   TegraGetEmcDvfsFreqTable

        cmp  w?,#0x20            ; (maxCount)
        csel w?,<same>,<cap>,lt  ; min(maxCount, 32)
        bl   <Get*DvfsFreqTable>
    */
    constexpr u32 EmcRateCapCmpAsm = 0x710082FF;  /* cmp  w?,#0x20 */
    constexpr u32 EmcRateCapCselAsm = 0x1A80B000; /* csel w?,w?,w?,lt */

    inline bool EmcRateListPatternFn(u32 *ptr) {
        return AsmSubsCompareNoReg(*ptr, EmcRateCapCmpAsm)           /* cmp w?,#0x20 */
               && AsmCompareCselNoReg(*(ptr + 1), EmcRateCapCselAsm) /* csel w?,w?,w?,lt */
               && (AsmGetRn(*ptr) == AsmGetRn(*(ptr + 1)))           /* min(reg, 0x20) */
               && AsmBlCompareOpcodeOnly(*(ptr + 2), 0x94000000);    /* bl <Get*DvfsFreqTable> */
    }

    constexpr u32 EmcRateSessCmpAsm  = 0x710082FF; /* cmp  w?,#0x20 */
    constexpr u32 EmcRateSessMovAsm  = 0x52800400; /* movz w?,#0x20 */
    constexpr u32 EmcRateSessCselAsm = 0x1A80B000; /* csel w?,w?,w?,lt (opcode + cond) */

    inline bool EmcRateSessFindClamp(u32 *ptr, u32 *out_c, u32 *out_cap, u32 *out_movz_i) {
        if (!AsmSubsCompareNoReg(ptr[0], EmcRateSessCmpAsm)) return false;   /* cmp w<c>,#0x20 */
        const u32 c = AsmGetRn(ptr[0]);
        for (u32 i = 1; i <= 14; ++i) {
            const u32 w = ptr[i];
            if (AsmCompareCselNoReg(w, EmcRateSessCselAsm) && AsmGetRn(w) == c && asm_get_rd(w) == c) {
                const u32 cap = AsmGetRm(w);
                for (u32 j = 1; j < i; ++j) {
                    if ((ptr[j] & 0xFFFFFFE0u) == EmcRateSessMovAsm && asm_get_rd(ptr[j]) == cap) {
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
        if (!(AsmIsLdrImm64(ptr[0]) && AsmGetLdStImm64Off(ptr[0]) == 0x10)) return false; /* ldr Xbuf,[Xbus,#0x10] */
        if (!(AsmIsAddImm64(ptr[1]) && AsmGetImm12(ptr[1])       == 0x18)) return false; /* add Xcnt,Xbus,#0x18   */
        if (!(AsmIsStrImm64(ptr[2]) && AsmGetLdStImm64Off(ptr[2]) == 0x50)) return false; /* str Xrail,[Xbus,#0x50]*/
        if (!AsmIsBl(ptr[3]))                                              return false; /* bl GetDvfsRailUnique  */
        const u32 bus = AsmGetRn(ptr[0]);
        return AsmGetRn(ptr[1]) == bus && AsmGetRn(ptr[2]) == bus;
    }

    inline bool ForceVerbosityPatternFn(u32 *ptr) {
        if (g_pcv_cave == 0) {
            return false;
        }
        if (reinterpret_cast<uintptr_t>(ptr + 11) > g_pcv_cave) {   /* .text only */
            return false;
        }
        if (ptr[0] != 0xA9BE7BFDu || ptr[1] != 0xF9000BF3u || ptr[2] != 0x910003FDu) return false; /* stp/str/mov x29,sp */
        if (!(AsmIsAddImm64(ptr[3]) && asm_get_rd(ptr[3]) == 0  && AsmGetRn(ptr[3]) == 29)) return false; /* add x0,x29,#imm  */
        if (!(AsmIsAddImm64(ptr[4]) && asm_get_rd(ptr[4]) == 19 && AsmGetRn(ptr[4]) == 29)) return false; /* add x19,x29,#imm */
        if (AsmGetImm12(ptr[3]) != AsmGetImm12(ptr[4]) || !AsmIsBl(ptr[5])) return false;
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
