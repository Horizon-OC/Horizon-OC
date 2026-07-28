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

    constexpr cvb_entry_t CpuCvbTableDefault[] = {
        // CPU_PLL_CVB_TABLE_ODN
        {  204000,  {721094}, {                         } },
        {  306000,  {754040}, {                         } },
        {  408000,  {786986}, {                         } },
        {  510000,  {819932}, {                         } },
        {  612000,  {852878}, {                         } },
        {  714000,  {885824}, {                         } },
        {  816000,  {918770}, {                         } },
        {  918000,  {951716}, {                         } },
        { 1020000,  {984662}, { -2875621,  358099, -8585} },
        { 1122000, {1017608}, {   -52225,  104159, -2816} },
        { 1224000, {1050554}, {  1076868,    8356,  -727} },
        { 1326000, {1083500}, {  2208191,  -84659,  1240} },
        { 1428000, {1116446}, {  2519460, -105063,  1611} },
        { 1581000, {1130000}, {  2889664, -122173,  1834} },
        { 1683000, {1168000}, {  5100873, -279186,  4747} },
        { 1785000, {1227500}, {  5100873, -279186,  4747} },
        {                                                 },
    };

    constexpr u32 CpuVoltOfficial = 1227;
    constexpr u32 CpuVminOfficial = 825;
    constexpr u32 CpuTune0Low     = 0xFFEAD0FF;

    constexpr u32 CpuVoltL4T = 1257'000;

    static const u32 cpuVoltDvfsPattern[] = { 1227, 1000, 100, 1000, 0 };
    static_assert(sizeof(cpuVoltDvfsPattern) == 0x14, "Invalid cpuVoltDvfsPattern size");

    static const u32 cpuVoltageThermalPattern[] = { 950, 1132, 0, 950, 1227, 0, 825, 1227, 15000, 825, 1170, 60000, 825, 1132, 80000 };
    static_assert(sizeof(cpuVoltageThermalPattern) == 0x3c, "Invalid cpuVoltageThermalPattern size");


    Result CpuVoltDvfs(u32 *ptr);
    Result CpuVoltThermals(u32 *ptr);
    Result CpuVoltDfll(u32* ptr);

}
