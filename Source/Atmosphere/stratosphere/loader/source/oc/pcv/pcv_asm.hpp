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

#include <stratosphere.hpp>

namespace ams::ldr::hoc::pcv {

    constexpr u32 NopIns = 0xD503201F;

    template <typename Compare>
    u32 *ScanAssembly(u32 *ptr, u32 scanLimit, u32 pattern, Compare comp) {
        for (u32 i = 0; i < scanLimit; ++i) {
            if (comp(pattern, ptr[i])) {
                return ptr + i;
            }
        }
        return nullptr;
    }

    inline auto asm_compare_no_rd = [](u32 ins1, u32 ins2) {
        return ((ins1 ^ ins2) >> 5) == 0;
    };

    inline auto asm_get_rd = [](u32 ins) {
        return ins & ((1 << 5) - 1);
    };

    /* Rn (bits 9:5) and Rm (bits 20:16) to get registers. */
    inline auto AsmGetRn = [](u32 ins) -> u32 { return (ins >> 5) & 0x1Fu; };
    inline auto AsmGetRm = [](u32 ins) -> u32 { return (ins >> 16) & 0x1Fu; };

    /* Add (shifted register), 64-bit: sf=1 op=0 S=0 01011 shift(2) 0 Rm imm6 Rn Rd. */
    inline auto AsmIsAddShiftedReg64 = [](u32 ins) {
        return (ins & 0xFF200000u) == 0x8B000000u;
    };

    inline auto asm_set_rd = [](u32 ins, u8 rd) {
        return (ins & 0xFFFFFFE0) | (rd & 0x1F);
    };

    inline auto asm_set_imm16 = [](u32 ins, u16 imm) {
        return (ins & 0xFFE0001F) | ((imm & 0xFFFF) << 5);
    };

    inline auto AsmGetImm16 = [](u32 ins) {
        return static_cast<u16>((ins >> 5) & 0xFFFF);
    };

    inline auto AsmCompareBrNoRd = [](u32 ins1, u32 ins2) {
        constexpr u32 RegMask = ~(((1 << 5) - 1) << 5);
        return ((ins1 & RegMask) ^ (ins2 & RegMask)) == 0;
    };

    inline auto AsmCompareAddNoImm12 = [](u32 ins1, u32 ins2) {
        constexpr u32 Imm12Mask = ~(((1 << 12) - 1) << 10);
        return ((ins1 & Imm12Mask) ^ (ins2 & Imm12Mask)) == 0;
    };

    inline auto AsmCompareAdrpNoImm = [](u32 ins1, u32 ins2) {
        constexpr u32 ImmMask = ~((((1 << 2) - 1) << 29) | (((1 << 19) - 1) << 5));
        return ((ins1 & ImmMask) ^ (ins2 & ImmMask)) == 0;
    };

    /* Csel (Conditional Select) */
    /*
        SF | Op | S  |                   | RM             | Cond        | 0  | 0  | Rn        | Rd
        31 | 30 | 29 | 28 27 26 25 24 23 | 20 19 18 17 16 | 15 14 13 12 | 11 | 10 | 9 8 7 6 5 | 4 3 2 1 0
    */
    inline auto AsmCbzCompareOpcodeOnly = [](u32 ins1, u32 ins2) {
        return ((ins1 ^ ins2) >> 24) == 0;
    };

    inline auto AsmBlCompareOpcodeOnly = [](u32 ins1, u32 ins2) {
        return ((ins1 ^ ins2) >> 26) == 0;
    };

    inline auto AsmIsAdrX0 = [](u32 ins) {
        return (ins & 0x9F00001Fu) == 0x10000000u;
    };

    inline auto AsmAdrTarget = [](u32 ins, uintptr_t pc) -> uintptr_t {
        s64 imm = static_cast<s64>((((ins >> 5) & 0x7FFFFu) << 2) | ((ins >> 29) & 0x3u));
        imm = (imm << 43) >> 43;
        return static_cast<uintptr_t>(static_cast<s64>(pc) + imm);
    };

    /* adrp Rd, target */
    inline auto AsmMakeAdrp = [](uintptr_t pc, uintptr_t target, u32 rd) -> u32 {
        const s64 delta = static_cast<s64>(target & ~static_cast<uintptr_t>(0xFFF))
                        - static_cast<s64>(pc & ~static_cast<uintptr_t>(0xFFF));
        const u32 imm   = static_cast<u32>((delta >> 12) & 0x1FFFFF);   /* 21-bit page */
        const u32 immlo = imm & 0x3;
        const u32 immhi = (imm >> 2) & 0x7FFFF;
        return 0x90000000u | (immlo << 29) | (immhi << 5) | (rd & 0x1Fu);
    };

    inline auto AsmSetAdrTarget = [](u32 ins, uintptr_t pc, uintptr_t target) -> u32 {
        const s64 delta = static_cast<s64>(target) - static_cast<s64>(pc);
        const u32 immlo = static_cast<u32>(delta & 0x3);
        const u32 immhi = static_cast<u32>((delta >> 2) & 0x7FFFF);
        return (ins & ~((0x3u << 29) | (0x7FFFFu << 5))) | (immlo << 29) | (immhi << 5);
    };

    /* adrp: bit31=1, bits 28:24 = 10000. */
    inline auto AsmIsAdrp = [](u32 ins) -> bool {
        return (ins & 0x9F000000u) == 0x90000000u;
    };

    inline auto AsmAdrpPageOffset = [](u32 ins) -> s64 {
        s64 imm = static_cast<s64>((((ins >> 5) & 0x7FFFFu) << 2) | ((ins >> 29) & 0x3u));
        imm = (imm << 43) >> 43;   /* sign-extend the 21-bit immediate */
        return imm << 12;
    };

    /* add (immediate), 64-bit: sf=1 op=0 S=0 100010 sh imm12 Rn Rd. */
    inline auto AsmIsAddImm64 = [](u32 ins) -> bool {
        return (ins & 0xFF800000u) == 0x91000000u;
    };

    inline auto AsmGetImm12 = [](u32 ins) -> u32 {
        return (ins >> 10) & 0xFFFu;
    };

    inline auto AsmMakeAddImm64 = [](u32 rd, u32 rn, u32 imm12) -> u32 {
        return 0x91000000u | ((imm12 & 0xFFFu) << 10) | ((rn & 0x1Fu) << 5) | (rd & 0x1Fu);
    };

    /* movz Wd,#imm16 (no shift). */
    inline auto AsmMakeMovzW = [](u32 rd, u16 imm16) -> u32 {
        return 0x52800000u | (static_cast<u32>(imm16) << 5) | (rd & 0x1Fu);
    };

    /* mov Xd,Xm  ==  orr Xd,XZR,Xm. */
    inline auto AsmMakeMovReg = [](u32 rd, u32 rm) -> u32 {
        return 0xAA0003E0u | ((rm & 0x1Fu) << 16) | (rd & 0x1Fu);
    };

    /* ldr Xt,[Xn,#byteOff] */
    inline auto AsmMakeLdrImm64 = [](u32 rt, u32 rn, u32 byteOff) -> u32 {
        return 0xF9400000u | (((byteOff / 8u) & 0xFFFu) << 10) | ((rn & 0x1Fu) << 5) | (rt & 0x1Fu);
    };

    /* b <target> (PC-relative, +-128MB). */
    inline auto AsmMakeB = [](uintptr_t pc, uintptr_t target) -> u32 {
        const s64 off = (static_cast<s64>(target) - static_cast<s64>(pc)) >> 2;
        return 0x14000000u | (static_cast<u32>(off) & 0x03FFFFFFu);
    };

    /* b.<cond> <target> (cond: LO/CC=0x3, LE=0xD, NE=0x1, ...). */
    inline auto AsmMakeBCond = [](uintptr_t pc, uintptr_t target, u32 cond) -> u32 {
        const s64 off = (static_cast<s64>(target) - static_cast<s64>(pc)) >> 2;
        return 0x54000000u | ((static_cast<u32>(off) & 0x7FFFFu) << 5) | (cond & 0xFu);
    };

    /* sub Xd,Xn,#imm12 (shift 0). */
    inline auto AsmMakeSubImm64 = [](u32 rd, u32 rn, u32 imm12) -> u32 {
        return 0xD1000000u | ((imm12 & 0xFFFu) << 10) | ((rn & 0x1Fu) << 5) | (rd & 0x1Fu);
    };

    /* cmp Wn,#imm12  ==  subs WZR,Wn,#imm12. */
    inline auto AsmMakeCmpImm32 = [](u32 rn, u32 imm12) -> u32 {
        return 0x7100001Fu | ((imm12 & 0xFFFu) << 10) | ((rn & 0x1Fu) << 5);
    };

    /* str Wt,[Xn,#byteOff] (32-bit, unsigned scaled by 4). */
    inline auto AsmMakeStrImm32 = [](u32 rt, u32 rn, u32 byteOff) -> u32 {
        return 0xB9000000u | (((byteOff / 4u) & 0xFFFu) << 10) | ((rn & 0x1Fu) << 5) | (rt & 0x1Fu);
    };

    /* ldr Wt,[Xn,#byteOff] (32-bit, unsigned scaled by 4). */
    inline auto AsmMakeLdrImm32 = [](u32 rt, u32 rn, u32 byteOff) -> u32 {
        return 0xB9400000u | (((byteOff / 4u) & 0xFFFu) << 10) | ((rn & 0x1Fu) << 5) | (rt & 0x1Fu);
    };

    /* add Xd,Xn,Xm,LSL #shift (64-bit shifted register, shift 0-63). */
    inline auto AsmMakeAddShiftedReg64 = [](u32 rd, u32 rn, u32 rm, u32 shift) -> u32 {
        return 0x8B000000u | ((rm & 0x1Fu) << 16) | ((shift & 0x3Fu) << 10) | ((rn & 0x1Fu) << 5) | (rd & 0x1Fu);
    };

    /* stp Xt1,Xt2,[Xn,#imm]  (signed offset, scaled by 8). */
    inline auto AsmMakeStpImm64 = [](u32 rt1, u32 rt2, u32 rn, s32 imm) -> u32 {
        return 0xA9000000u | ((static_cast<u32>(imm / 8) & 0x7Fu) << 15) | ((rt2 & 0x1Fu) << 10) | ((rn & 0x1Fu) << 5) | (rt1 & 0x1Fu);
    };

    /* stp Qt1,Qt2,[Xn,#imm]  (128-bit SIMD, signed offset scaled by 16). */
    inline auto AsmMakeStpqImm = [](u32 qt1, u32 qt2, u32 rn, s32 imm) -> u32 {
        return 0xAD000000u | ((static_cast<u32>(imm / 16) & 0x7Fu) << 15) | ((qt2 & 0x1Fu) << 10) | ((rn & 0x1Fu) << 5) | (qt1 & 0x1Fu);
    };

    /* str Xt,[Xn,#byteOff]  (unsigned scaled by 8). */
    inline auto AsmMakeStrImm64 = [](u32 rt, u32 rn, u32 byteOff) -> u32 {
        return 0xF9000000u | (((byteOff / 8u) & 0xFFFu) << 10) | ((rn & 0x1Fu) << 5) | (rt & 0x1Fu);
    };

    /* movn Wd,#imm16 (no shift): loads ~imm16, e.g. movn Wd,#0x37 == -56. */
    inline auto AsmMakeMovnW = [](u32 rd, u16 imm16) -> u32 {
        return 0x12800000u | (static_cast<u32>(imm16) << 5) | (rd & 0x1Fu);
    };

    /* bl <target>  (PC-relative, +-128MB). */
    inline auto AsmMakeBl = [](uintptr_t pc, uintptr_t target) -> u32 {
        const s64 off = (static_cast<s64>(target) - static_cast<s64>(pc)) >> 2;
        return 0x94000000u | (static_cast<u32>(off) & 0x03FFFFFFu);
    };

    /* svc #imm16. */
    inline auto AsmMakeSvc = [](u16 imm16) -> u32 {
        return 0xD4000001u | (static_cast<u32>(imm16) << 5);
    };

    constexpr u32 RetIns = 0xD65F03C0u; /* ret (x30) */

    /* ldrb Wt,[Xn,#imm] (unsigned byte, scale 1). */
    inline auto AsmMakeLdrbImm = [](u32 rt, u32 rn, u32 byteOff) -> u32 {
        return 0x39400000u | ((byteOff & 0xFFFu) << 10) | ((rn & 0x1Fu) << 5) | (rt & 0x1Fu);
    };

    /* cbz Wt,<target>. */
    inline auto AsmMakeCbz = [](uintptr_t pc, uintptr_t target, u32 rt) -> u32 {
        const s64 off = (static_cast<s64>(target) - static_cast<s64>(pc)) >> 2;
        return 0x34000000u | ((static_cast<u32>(off) & 0x7FFFFu) << 5) | (rt & 0x1Fu);
    };

    /* strb Wt,[Xn,#imm] (unsigned byte, scale 1). */
    inline auto AsmMakeStrbImm = [](u32 rt, u32 rn, u32 byteOff) -> u32 {
        return 0x39000000u | ((byteOff & 0xFFFu) << 10) | ((rn & 0x1Fu) << 5) | (rt & 0x1Fu);
    };

    /* mov Xd,X<rm>  ==  orr Xd,XZR,X<rm>  (matches any Rd). */
    inline auto AsmIsMovReg = [](u32 ins, u32 rm) -> bool {
        return (ins & 0xFFFFFFE0u) == (0xAA0003E0u | ((rm & 0x1Fu) << 16));
    };

    /* add Xd,sp,#imm12 (shift 0). */
    inline auto AsmIsAddSpImm = [](u32 ins) -> bool {
        return (ins & 0xFFC003E0u) == 0x910003E0u;
    };

    /* sub Xd,x29,#imm12 (shift 0). */
    inline auto AsmIsSubX29Imm = [](u32 ins) -> bool {
        return (ins & 0xFFC003E0u) == 0xD10003A0u;
    };

    inline auto AsmIsB     = [](u32 ins) -> bool { return (ins & 0xFC000000u) == 0x14000000u; }; /* b   */
    inline auto AsmIsBl    = [](u32 ins) -> bool { return (ins & 0xFC000000u) == 0x94000000u; }; /* bl  */
    inline auto AsmIsBCond = [](u32 ins) -> bool { return (ins & 0xFF000010u) == 0x54000000u; }; /* b.c */

    /* ldr/str Xt,[Xn,#imm] (64-bit, unsigned scaled offset). */
    inline auto AsmIsLdrImm64      = [](u32 ins) -> bool { return (ins & 0xFFC00000u) == 0xF9400000u; };
    inline auto AsmIsStrImm64      = [](u32 ins) -> bool { return (ins & 0xFFC00000u) == 0xF9000000u; };
    inline auto AsmGetLdStImm64Off = [](u32 ins) -> u32  { return ((ins >> 10) & 0xFFFu) * 8u; };

    /* Byte target address of a b/bl at pc. */
    inline auto AsmBranchTarget = [](u32 ins, uintptr_t pc) -> uintptr_t {
        s64 off = static_cast<s64>((ins & 0x03FFFFFFu) << 2);
        off = (off << 36) >> 36;   /* sign-extend the 28-bit branch offset */
        return static_cast<uintptr_t>(static_cast<s64>(pc) + off);
    };

    /* Rewrite a scaled immediate-offset load/store (`op Wt,[Xn,#imm]`) into its register-offset form */
    inline auto AsmSetLdStRegOffset = [](u32 ldstImm, u32 rm) -> u32 {
        return (ldstImm & 0xC0C003FFu) | 0x38207800u | ((rm & 0x1Fu) << 16);
    };

    inline auto AsmIsLdpX = [](u32 ins) {
        return (ins & 0xFE400000u) == 0xA8400000u;
    };

    inline bool AsmComparePrologue(u32 ins1, u32 ins2, u32 ins3, u32 cmp1, u32 cmp2, u32 cmp3) {
        constexpr u32 StpImmMask = ~((((1u << 7) - 1u) << 15));

        bool firstMatch = (ins1 & StpImmMask) == (cmp1 & StpImmMask);

        constexpr u32 StpRegsImmMask = ~(((1u << 5) - 1u) |(((1u << 5) - 1u) << 10) | (((1u << 7) - 1u) << 15));

        bool secondMatch = (ins2 & StpRegsImmMask) == (cmp2 & StpRegsImmMask);

        constexpr u32 MovMask = ~((1u << 5) - 1u);

        bool thirdMatch = (ins3 & MovMask) == (cmp3 & MovMask);

        return firstMatch && secondMatch && thirdMatch;
    }

    inline auto AsmCompareCselNoReg = [](u32 ins1, u32 ins2) {
        constexpr u32 ClearReg = ~(((1 << 10) - 1) | (((1 << 5) - 1) << 16));
        return ((ins1 & ClearReg) ^ (ins2 & ClearReg)) == 0;
    };

    /* Mul */
    /*
        SF | Op54                 | Op31     | RM             | o0 | RA             | RN        | RD
        31 | 30 29 28 27 26 25 24 | 23 22 21 | 20 19 18 17 16 | 15 | 14 13 12 11 10 | 9 8 7 6 5 | 4 3 2 1 0
    */
    inline auto AsmCompareMullNoReg = [](u32 ins1, u32 ins2) {
        constexpr u32 ClearReg = ~(((1 << 10) - 1) | (((1 << 5) - 1) << 16));
        return ((ins1 & ClearReg) ^ (ins2 & ClearReg)) == 0;
    };

    /* Mul */
    /* MUL W11, W24, W26 */
    /* multiplies by 1000, mV -> uV */
    /*
        SF | Op54                 | Op31     | RM             | o0 | RA             | RN        | RD
        31 | 30 29 28 27 26 25 24 | 23 22 21 | 20 19 18 17 16 | 15 | 14 13 12 11 10 | 9 8 7 6 5 | 4 3 2 1 0
    */
    inline auto AsmGetMullRn = [](u32 ins) {
        constexpr u32 Mask = ((1 << 5) - 1) << 5;
        return (ins & Mask) >> 5;
    };

    inline auto AsmGetMullRm = [](u32 ins) {
        constexpr u32 Mask = ((1 << 5) - 1) << 16;
        return (ins & Mask) >> 16;
    };

    /* Subs (Shifted register) */
    /*
        SF | Op | S  |                | Shift | 0  | RM             | Imm6              | Rn        | Rd
        31 | 30 | 29 | 28 27 26 25 24 | 23 22 | 21 | 20 19 18 17 16 | 15 14 13 12 11 10 | 9 8 7 6 5 | 4 3 2 1 0
    */
    inline auto AsmSubsSetRn = [](u32 ins, u8 rn) {
        constexpr u32 RnMaskClear = ~(((1u << 5) - 1u) << 5);
        constexpr u32 RnMaskSet = (1u << 5) - 1u;

        return (ins & RnMaskClear) | ((static_cast<u32>(rn) & RnMaskSet) << 5);
    };

    /* Subs (Immediate) */

    /*
        SF | Op | S  |                   | Sh | Imm12                               | Rn        | Rd
        31 | 30 | 29 | 28 27 26 25 24 23 | 22 | 21 20 19 18 17 16 15 14 13 12 11 10 | 9 8 7 6 5 | 4 3 2 1 0
    */
    inline auto AsmSubsSetImm12 = [](u32 ins, u16 imm12) {
        constexpr u32 ClearMask    = ~(((1u << 12) - 1) << 10);
        constexpr u32 SetImm12Mask =   ( 1u << 12) - 1;

        return (ins & ClearMask) | ((imm12 & SetImm12Mask) << 10);
    };

    inline auto AsmSubsCompareNoReg = [](u32 ins1, u32 ins2) {
        return ((ins1 ^ ins2) >> 10) == 0;
    };

    inline auto AsmCompareBrConNoImm19 = [](u32 ins1, u32 ins2) {
        constexpr u32 ClearImm19 = ~(((1 << 19) - 1) << 5);
        return (ins1 & ClearImm19) == (ins2 & ClearImm19);
    };

    inline bool AsmIsFramePush(u32 ins) {
        constexpr u32 FramePushMask  = 0xFFC07FFF;
        constexpr u32 FramePushValue = 0xA9807BFD;
        return (ins & FramePushMask) == FramePushValue;
    }

    inline u32 *FindFnPrologue(u32 *ptr, u32 margin, u32 *nsoStart) {
        for (u32 i = 0; i <= margin; ++i) {
            u32 *candidate = ptr - i;
            if (candidate < nsoStart) {
                break;
            }

            if (AsmIsFramePush(*candidate)) {
                return candidate;
            }
        }

        return nullptr;
    }

}
