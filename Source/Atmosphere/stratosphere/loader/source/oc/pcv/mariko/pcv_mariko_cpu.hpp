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

#pragma once

#include "../pcv.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    constexpr cvb_entry_t CpuCvbTableDefault[] = {
        {  204000, {  721589, -12695, 27 }, {         } },
        {  306000, {  747134, -14195, 27 }, {         } },
        {  408000, {  776324, -15705, 27 }, {         } },
        {  510000, {  809160, -17205, 27 }, {         } },
        {  612000, {  845641, -18715, 27 }, {         } },
        {  714000, {  885768, -20215, 27 }, {         } },
        {  816000, {  929540, -21725, 27 }, {         } },
        {  918000, {  976958, -23225, 27 }, {         } },
        { 1020000, { 1028021, -24725, 27 }, { 1120000 } },
        { 1122000, { 1082730, -26235, 27 }, { 1120000 } },
        { 1224000, { 1141084, -27735, 27 }, { 1120000 } },
        { 1326000, { 1203084, -29245, 27 }, { 1120000 } },
        { 1428000, { 1268729, -30745, 27 }, { 1120000 } },
        { 1581000, { 1374032, -33005, 27 }, { 1120000 } },
        { 1683000, { 1448791, -34505, 27 }, { 1120000 } },
        { 1785000, { 1527196, -36015, 27 }, { 1120000 } },
        { 1887000, { 1609246, -37515, 27 }, { 1120000 } },
        { 1963500, { 1675751, -38635, 27 }, { 1120000 } },
        {                                               },
    };

    constexpr u32 CpuClkOfficial      = 1963'500;
    constexpr u32 CpuVoltOfficial     = 1120;
    constexpr u32 CpuHighVminOfficial = 850;
    constexpr u32 CpuVminOfficial     = 620;
    constexpr u32 CpuTune0Low         = 0xFFCF;

    static const u32 cpuVoltagePatchValues[]  = { 850, 38, 1120, 1000, 100, 1000, 0 };
    static const s32 cpuVoltagePatchOffsets[] = {  -2, -1,    5,    6,   7,    8, 9 };
    static_assert(sizeof(cpuVoltagePatchValues) == sizeof(cpuVoltagePatchOffsets), "Invalid cpuVoltagePatch size");

    static const u32 cpuVoltThermalData[] = { 620, 1120, 20000, 620, 1120, 70000, 950, 1132, 0, 950, 1227, 0 };

    static const u32 allowedCpuMaxFrequencies[] = { 1'963'500, 2'091'000, 2'193'000, 2'295'000, 2'397'000, 2'499'000, 2'601'000, 2'703'000, };


    Result CpuFreqVdd(u32 *ptr);
    Result CpuVoltDVFS(u32 *ptr);
    Result CpuVoltThermals(u32 *ptr);
    Result CpuVoltDfll(u32 *ptr);

}
