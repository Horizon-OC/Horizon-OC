/*
 * Copyright (C) Switch-OC-Suite
 *
 * Copyright (c) 2023 hanai3Bi
 *
 * Copyright (c) B3711
 *
 * Copyright (c) Souldbminer and Horizon OC Contributors
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

#include "../pcv.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    constexpr cvb_entry_t GpuCvbTableDefault[] = {
        // GPUB01_NA_CVB_TABLE
        {   76800, {}, {  610000,                                 } },
        {  153600, {}, {  610000,                                 } },
        {  230400, {}, {  610000,                                 } },
        {  307200, {}, {  610000,                                 } },
        {  384000, {}, {  610000,                                 } },
        {  460800, {}, {  610000,                                 } },
        {  537600, {}, {  801688, -10900, -163, 298, -10599, 162, } },
        {  614400, {}, {  824214,  -5743, -452, 238,  -6325,  81, } },
        {  691200, {}, {  848830,  -3903, -552, 119,  -4030,  -2, } },
        {  768000, {}, {  891575,  -4409, -584,   0,  -2849,  39, } },
        {  844800, {}, {  940071,  -5367, -602, -60,    -63, -93, } },
        {  921600, {}, {  986765,  -6637, -614, -179,  1905, -13, } },
        {  998400, {}, { 1098475, -13529, -497, -179,  3626,   9, } },
        { 1075200, {}, { 1163644, -12688, -648,    0,  1077,  40, } },
        { 1152000, {}, { 1204812,  -9908, -830,    0,  1469, 110, } },
        { 1228800, {}, { 1277303, -11675, -859,    0,  3722, 313, } },
        { 1267200, {}, { 1335531, -12567, -867,    0,  3681, 559, } },
        {                                                           },
    };

    constexpr u32 GpuClkPllMax    = 1300'000'000;
    constexpr u32 GpuClkPllLimit  = 2'600'000;
    constexpr u32 GpuVminOfficial = 610;

    static const u32 gpuDVFSPattern[] = { 1050, 1000, 100, 1000, 10, };
    static const u32 gpuVoltThermalPattern[] = { 800, 1120, 0, 610, 1120, 20000, 610, 1120, 30000, 610, 1120, 50000, 610, 1120, 70000, 610, 1120, 90000, };
    static_assert(sizeof(gpuVoltThermalPattern) == 72, "Invalid gpuVoltThermalPattern");

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
        return asm_compare_no_rd(*ptr32, GpuAsmPattern[0]);
    }

    Result GpuVoltDVFS(u32 *ptr);
    Result GpuVoltThermals(u32 *ptr);
    Result GpuFreqMaxAsm(u32 *ptr32);
    Result GpuFreqPllMax(u32 *ptr);
    Result GpuFreqPllLimit(u32 *ptr);

}
