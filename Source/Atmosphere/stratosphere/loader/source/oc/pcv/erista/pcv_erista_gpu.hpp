/*
 * Copyright (C) Switch-OC-Suite
 *
 * Copyright (c) 2023 hanai3Bi
 *
 * Copyright (c) B3711
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

#include "pcv_erista.hpp"

namespace ams::ldr::hoc::pcv::erista {

    constexpr u32 GpuClkPllLimit  = 2'600'000;
    constexpr u32 GpuClkPllMax    = 921'600'000;
    constexpr u32 GpuVminOfficial = 810;

    static const u32 gpuVoltDvfsPattern[] = { 810, 1150, 1000, 100, 1000, 10, };
    static_assert(sizeof(gpuVoltDvfsPattern) == (sizeof(u32) * 6), "Invalid gpuVoltDvfsPattern");

    static const u32 gpuVoltThermalPattern[] = { 950, 1132, 0, 810, 1132, 15000, 810, 1132, 30000, 810, 1132, 50000, 810, 1132, 70000, 810, 1132, 105000 };
    static_assert(sizeof(gpuVoltThermalPattern) == 0x48, "Invalid gpuVoltageThermalPattern size");

    /* GPU Max Clock asm Pattern:
        *
        * MOV W11, #0x1000 MOV (wide immediate)                0x1000                              0xB (11)
        *  sf | opc |                 | hw  |                   imm16                        |      Rd
        * #31 |30 29|28 27 26 25 24 23|22 21|20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5 |4  3  2  1  0
        *   0 | 1 0 | 1  0  0  1  0  1| 0  0| 0  0  0  1  0  0  0  0  0  0  0  0  0  0  0  0 |0  1  0  1  1
        *
        * MOVK W11, #0xE, LSL#16     <shift>16                    0xE                              0xB (11)
        *  sf | opc |                 | hw  |                   imm16                        |      Rd
        * #31 |30 29|28 27 26 25 24 23|22 21|20 19 18 17 16 15 14 13 12 11 10  9  8  7  6  5 |4  3  2  1  0
        *   0 | 1 1 | 1  0  0  1  0  1| 0  1| 0  0  0  0  0  0  0  0  0  0  0  0  1  1  1  0 |0  1  0  1  1
        */
    inline constexpr u32 GpuAsmPattern[] = { 0x52820000, 0x72A001C0 };

    inline bool GpuMaxClockPatternFn(u32 *ptr32) {
        return _asm::Ignoring(*ptr32, GpuAsmPattern[0], _asm::field::Rd);
    };

    constexpr cvb_entry_t GpuCvbTableDefault[] = {
        // NA_FREQ_CVB_TABLE
        {  76800, {}, {  814294, 8144, -940, 808, -21583, 226, } },
        { 153600, {}, {  856185, 8144, -940, 808, -21583, 226, } },
        { 230400, {}, {  898077, 8144, -940, 808, -21583, 226, } },
        { 307200, {}, {  939968, 8144, -940, 808, -21583, 226, } },
        { 384000, {}, {  981860, 8144, -940, 808, -21583, 226, } },
        { 460800, {}, { 1023751, 8144, -940, 808, -21583, 226, } },
        { 537600, {}, { 1065642, 8144, -940, 808, -21583, 226, } },
        { 614400, {}, { 1107534, 8144, -940, 808, -21583, 226, } },
        { 691200, {}, { 1149425, 8144, -940, 808, -21583, 226, } },
        { 768000, {}, { 1191317, 8144, -940, 808, -21583, 226, } },
        { 844800, {}, { 1233208, 8144, -940, 808, -21583, 226, } },
        { 921600, {}, { 1275100, 8144, -940, 808, -21583, 226, } },
        {                                                        },
    };

    Result GpuVoltDVFS(u32 *ptr);
    Result GpuVoltThermals(u32 *ptr);
    Result GpuFreqMaxAsm(u32 *ptr32);
    Result GpuFreqPllMax(u32 *ptr);

    // patch out 1305MHz limit on erista, don't use this!
    // Result GpuFreqPllLimit(u32 *ptr);

}
