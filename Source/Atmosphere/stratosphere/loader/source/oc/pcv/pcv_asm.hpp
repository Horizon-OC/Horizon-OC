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

namespace ams::ldr::hoc::pcv::_asm {

    constexpr u32 NopIns = 0xD503201F;
    constexpr u32 RetIns = 0xD65F03C0;

    constexpr u32 GetField(u32 ins, u8 lsb, u8 width) {
        return (ins >> lsb) & ((1u << width) - 1u);
    }

    constexpr u32 SetField(u32 ins, u8 lsb, u8 width, u32 val) {
        const u32 mask = ((1u << width) - 1u) << lsb;
        return (ins & ~mask) | ((val << lsb) & mask);
    }

    constexpr u32 FieldMask(u8 lsb, u8 width) {
        return ((1u << width) - 1u) << lsb;
    }

    constexpr s64 SignExtend(u64 v, u32 bits) {
        const u32 pad = 64u - bits;
        return static_cast<s64>(v << pad) >> pad;
    }

    namespace field {
        struct Descriptor {
            u8 lsb;
            u8 width;
            u8 shift = 0;
        };

        constexpr Descriptor Rd    { 0,  5};  /* destination register */
        constexpr Descriptor Rn    { 5,  5};  /* first source register */
        constexpr Descriptor Rm    {16,  5};  /* second source register */
        constexpr Descriptor Ra    {10,  5};  /* accumulator register */
        constexpr Descriptor Rt    { 0,  5};  /* load/store transfer register */
        constexpr Descriptor Rt2   {10,  5};  /* second transfer register of a pair */

        constexpr Descriptor Cond  {12,  4};  /* condition code (CSEL) */
        constexpr Descriptor BCond { 0,  4};  /* condition code (B.cond) */
        constexpr Descriptor Hw    {21,  2};  /* MOVZ/MOVK/MOVN shift */

        constexpr Descriptor Sh    {22,  1};
        constexpr Descriptor Shift {22,  2};  /* ADD/SUB (shifted register) shift type */
        constexpr Descriptor Imm6  {10,  6};  /* ADD/SUB (shifted register) shift amount */

        constexpr Descriptor Imm16 { 5, 16};  /* MOVZ/MOVK/MOVN immediate */
        constexpr Descriptor Imm12 {10, 12};  /* ADD/SUB (immediate) */
        constexpr Descriptor Imm19 { 5, 19};  /* B.cond / CBZ / CBNZ immediate */
        constexpr Descriptor Imm26 { 0, 26};  /* B / BL immediate */

        constexpr Descriptor ImmAdrpHi { 5, 19};
        constexpr Descriptor ImmAdrpLo {29,  2};

        constexpr Descriptor Off1  {10, 12, 0};  /* LDRB/STRB */
        constexpr Descriptor Off4  {10, 12, 2};  /* 32-bit LDR/STR */
        constexpr Descriptor Off8  {10, 12, 3};  /* 64-bit LDR/STR */

        constexpr Descriptor PairOff8  {15, 7, 3};  /* X pairs */
        constexpr Descriptor PairOff16 {15, 7, 4};  /* Q pairs */
    }

    /* Regs that read as a name in the encoding. */
    namespace reg {
        constexpr u32 Fp  = 29;
        constexpr u32 Lr  = 30;
        constexpr u32 Sp  = 31;
        constexpr u32 Wzr = 31;
        constexpr u32 Xzr = 31;
    }

    namespace cond {
        constexpr u32 Eq = 0x0, Ne = 0x1, Cs = 0x2, Cc = 0x3,
                      Mi = 0x4, Pl = 0x5, Vs = 0x6, Vc = 0x7,
                      Hi = 0x8, Ls = 0x9, Ge = 0xA, Lt = 0xB,
                      Gt = 0xC, Le = 0xD, Al = 0xE;
        constexpr u32 Hs = Cs, Lo = Cc;
    }

    /* Base opcodes. */
    /* Either pair with Encode or Ignoring. */
    namespace op {
        constexpr u32 AddImm32      = 0x11000000;
        constexpr u32 AddImm64      = 0x91000000;
        constexpr u32 SubImm64      = 0xD1000000;
        constexpr u32 SubsImm32     = 0x71000000;
        constexpr u32 SubsImm64     = 0xF1000000;

        constexpr u32 MovzW         = 0x52800000;
        constexpr u32 MovkW         = 0x72800000;
        constexpr u32 MovnW         = 0x12800000;

        constexpr u32 AddShifted64  = 0x8B000000;
        constexpr u32 OrrShifted32  = 0x2A000000;
        constexpr u32 OrrShifted64  = 0xAA000000;
        constexpr u32 SubsShifted32 = 0x6B000000;

        constexpr u32 Csel32        = 0x1A800000;
        constexpr u32 Madd32        = 0x1B000000;

        constexpr u32 LdrbImm       = 0x39400000;
        constexpr u32 StrbImm       = 0x39000000;
        constexpr u32 LdrImm32      = 0xB9400000;
        constexpr u32 StrImm32      = 0xB9000000;
        constexpr u32 LdrImm64      = 0xF9400000;
        constexpr u32 StrImm64      = 0xF9000000;

        constexpr u32 StpImm64      = 0xA9000000;
        constexpr u32 StpPreImm64   = 0xA9800000;
        constexpr u32 StpImmQ       = 0xAD000000;

        constexpr u32 B             = 0x14000000;
        constexpr u32 Bl            = 0x94000000;
        constexpr u32 BCond         = 0x54000000;
        constexpr u32 Cbz           = 0x34000000;
        constexpr u32 Br            = 0xD61F0000;

        constexpr u32 Adrp          = 0x90000000;
        constexpr u32 Svc           = 0xD4000001;
    }

    constexpr u32 Get(u32 ins, field::Descriptor f) {
        return GetField(ins, f.lsb, f.width) << f.shift;
    }

    constexpr u32 Set(u32 ins, field::Descriptor f, u64 val) {
        return SetField(ins, f.lsb, f.width, static_cast<u32>(val >> f.shift));
    }

    template <typename... Fs>
    constexpr bool Ignoring(u32 a, u32 b, Fs... fields) {
        const u32 mask = (0u | ... | FieldMask(fields.lsb, fields.width));
        return (a & ~mask) == (b & ~mask);
    }

    inline constexpr auto IgnoringRd = [](u32 a, u32 b) {
        return Ignoring(a, b, field::Rd);
    };

    template <typename... Fs>
    constexpr bool IsOp(u32 ins, u32 baseOpcode, Fs... variableFields) {
        return Ignoring(ins, baseOpcode, variableFields...);
    }

    struct FieldValue {
        field::Descriptor f;
        u64               val;
    };

    constexpr u32 Encode(u32 base, FieldValue a) {
        return Set(base, a.f, a.val);
    }

    constexpr u32 Encode(u32 base, FieldValue a, FieldValue b) {
        return Encode(Encode(base, a), b);
    }

    constexpr u32 Encode(u32 base, FieldValue a, FieldValue b, FieldValue c) {
        return Encode(Encode(base, a, b), c);
    }

    constexpr u32 Encode(u32 base, FieldValue a, FieldValue b, FieldValue c, FieldValue d) {
        return Encode(Encode(base, a, b, c), d);
    }

    constexpr u32 Encode(u32 base, FieldValue a, FieldValue b, FieldValue c, FieldValue d, FieldValue e) {
        return Encode(Encode(base, a, b, c, d), e);
    }

    constexpr uintptr_t Target26(u32 ins, uintptr_t pc) {
        return static_cast<uintptr_t>(static_cast<s64>(pc) + SignExtend(static_cast<u64>(Get(ins, field::Imm26)) << 2, 28));
    }

    constexpr u32 Retarget26(u32 ins, uintptr_t pc, uintptr_t target) {
        return Set(ins, field::Imm26, static_cast<u32>((static_cast<s64>(target) - static_cast<s64>(pc)) >> 2));
    }

    constexpr uintptr_t Target19(u32 ins, uintptr_t pc) {
        return static_cast<uintptr_t>(static_cast<s64>(pc) + SignExtend(static_cast<u64>(Get(ins, field::Imm19)) << 2, 21));
    }

    constexpr u32 Retarget19(u32 ins, uintptr_t pc, uintptr_t target) {
        return Set(ins, field::Imm19, static_cast<u32>((static_cast<s64>(target) - static_cast<s64>(pc)) >> 2));
    }

    constexpr uintptr_t TargetAdrp(u32 ins, uintptr_t pc) {
        const u64 imm21 = (static_cast<u64>(Get(ins, field::ImmAdrpHi)) << 2) | Get(ins, field::ImmAdrpLo);
        return static_cast<uintptr_t>(static_cast<s64>(pc & ~static_cast<uintptr_t>(0xFFF)) + SignExtend(imm21 << 12, 33));
    }

    constexpr u32 RetargetAdrp(u32 ins, uintptr_t pc, uintptr_t target) {
        const s64 delta = static_cast<s64>(target & ~static_cast<uintptr_t>(0xFFF)) - static_cast<s64>(pc & ~static_cast<uintptr_t>(0xFFF));
        const u32 imm21 = static_cast<u32>((delta >> 12) & 0x1FFFFF);
        return Set(Set(ins, field::ImmAdrpHi, imm21 >> 2), field::ImmAdrpLo, imm21 & 0x3u);
    }

    constexpr u32 MovReg64(u32 rd, u32 rm) {
        return Encode(op::OrrShifted64, {field::Rd, rd}, {field::Rn, reg::Xzr}, {field::Rm, rm});
    }

    constexpr u32 CmpImm32(u32 rn, u32 imm12) {
        return Encode(op::SubsImm32, {field::Rd, reg::Wzr}, {field::Rn, rn}, {field::Imm12, imm12});
    }

    constexpr u32 ToRegOffset(u32 ldstImm, u32 rm) {
        constexpr u32 Keep = 0xC0C00000u | FieldMask(field::Rt.lsb, field::Rt.width) | FieldMask(field::Rn.lsb, field::Rn.width);
        return ((ldstImm & Keep) | 0x38207800u) | ((rm & 0x1Fu) << field::Rm.lsb);
    }

    template <typename Compare> requires (!std::is_same_v<std::remove_cvref_t<Compare>, field::Descriptor>)
    u32 *ScanAssembly(u32 *ptr, u32 scanLimit, u32 pattern, Compare comp) {
        for (u32 i = 0; i < scanLimit; ++i) {
            if (comp(pattern, ptr[i])) {
                return ptr + i;
            }
        }
        return nullptr;
    }

    template <typename... Fs>
    u32 *ScanAssembly(u32 *ptr, u32 scanLimit, u32 pattern, Fs... ignoreFields) {
        return ScanAssembly(ptr, scanLimit, pattern, [=](u32 a, u32 b) ALWAYS_INLINE_LAMBDA { return Ignoring(a, b, ignoreFields...); });
    }

    constexpr bool IsFramePushPre(u32 ins) {
        constexpr u32 Pattern = Encode(op::StpPreImm64, {field::Rt, reg::Fp}, {field::Rt2, reg::Lr}, {field::Rn, reg::Sp});
        return Ignoring(ins, Pattern, field::PairOff8);
    }

    constexpr bool IsFramePushOffset(u32 ins) {
        constexpr u32 Pattern = Encode(op::StpImm64, {field::Rt, reg::Fp}, {field::Rt2, reg::Lr}, {field::Rn, reg::Sp});
        return Ignoring(ins, Pattern, field::PairOff8);
    }

    constexpr bool IsSpSubImm(u32 ins) {
        constexpr u32 Pattern = Encode(op::SubImm64, {field::Rd, reg::Sp}, {field::Rn, reg::Sp});
        return Ignoring(ins, Pattern, field::Imm12);
    }

    constexpr bool IsFramePush(u32 ins) {
        return IsFramePushPre(ins) || IsFramePushOffset(ins);
    }

    inline u32 *FindFnPrologue(u32 *ptr, u32 margin, u32 *nsoStart) {
        for (u32 i = 0; i <= margin; ++i) {
            u32 *candidate = ptr - i;
            if (candidate < nsoStart) {
                break;
            }

            if (IsFramePushPre(*candidate)) {
                return candidate;
            }

            if (IsFramePushOffset(*candidate) && candidate - 1 >= nsoStart && IsSpSubImm(candidate[-1])) {
                return candidate - 1;
            }
        }

        return nullptr;
    }

}
