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

    inline auto AsmSetAdrTarget = [](u32 ins, uintptr_t pc, uintptr_t target) -> u32 {
        const s64 delta = static_cast<s64>(target) - static_cast<s64>(pc);
        const u32 immlo = static_cast<u32>(delta & 0x3);
        const u32 immhi = static_cast<u32>((delta >> 2) & 0x7FFFF);
        return (ins & ~((0x3u << 29) | (0x7FFFFu << 5))) | (immlo << 29) | (immhi << 5);
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

}
