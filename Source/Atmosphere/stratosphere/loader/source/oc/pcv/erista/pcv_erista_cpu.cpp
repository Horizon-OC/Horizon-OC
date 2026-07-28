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

#include "../pcv.hpp"

namespace ams::ldr::hoc::pcv::erista {

    Result CpuVoltDvfs(u32 *ptr) {
        if (std::memcmp(ptr + 5, cpuVoltDvfsPattern, sizeof(cpuVoltDvfsPattern))) {
            R_THROW(ldr::ResultInvalidCpuMinVolt());
        }

        if (C.eristaCpuVmin) {
            PATCH_OFFSET(ptr, C.eristaCpuVmin);
        }

        if (C.eristaCpuUV) {
            PATCH_OFFSET(ptr - 2, C.eristaCpuVmin);
        }

        if (C.eristaCpuMaxVolt) {
            PATCH_OFFSET(ptr + 5, C.eristaCpuMaxVolt);
        }

        R_SUCCEED();
    }

    Result CpuVoltThermals(u32 *ptr) {
        if (std::memcmp(ptr - 6, cpuVoltageThermalPattern, sizeof(cpuVoltageThermalPattern))) {
            R_THROW(ldr::ResultInvalidCpuMinVolt());
        }

        if (C.eristaCpuVmin) {
            PATCH_OFFSET(    ptr, C.eristaCpuVmin);
            PATCH_OFFSET(ptr + 3, C.eristaCpuVmin);
            PATCH_OFFSET(ptr + 6, C.eristaCpuVmin);
        }

        if (C.eristaCpuMaxVolt) {
            PATCH_OFFSET(ptr - 2, C.eristaCpuMaxVolt);
            PATCH_OFFSET(ptr + 1, C.eristaCpuMaxVolt);
            PATCH_OFFSET(ptr + 4, C.eristaCpuMaxVolt);
            PATCH_OFFSET(ptr + 7, C.eristaCpuMaxVolt);
        }

        R_SUCCEED();
    }

    Result CpuVoltDfll(u32* ptr) {
        CvbCpuDfllData *entry = reinterpret_cast<CvbCpuDfllData *>(ptr);

        R_UNLESS(entry->tune0_low  == 0xFFEAD0FF, ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune0_high == 0x0,        ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune1_low  == 0x0,        ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune1_high == 0x0,        ldr::ResultInvalidCpuVoltDfllEntry());

        if (!C.eristaCpuUV) {
            R_SKIP();
        }

        switch (C.eristaCpuUV) {
            case 1:
                PATCH_OFFSET(&(entry->tune0_high), 0xffff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27007ff);
                break;
            case 2:
                PATCH_OFFSET(&(entry->tune0_high), 0xefff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27407ff);
                break;
            case 3:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27807ff);
                break;
            case 4:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27a07ff);
                break;
            case 5:
                PATCH_OFFSET(&(entry->tune0_high), 0xcfdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x37007ff);
                break;
            default:
                break;
        }

        R_SUCCEED();
    }

}
