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

#include "../pcv.hpp"
#include "pcv_mariko_cpu.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    u32 CapCpuClock() {
        u32 cpuCap = allowedCpuMaxFrequencies[0];

        for (u32 freq : allowedCpuMaxFrequencies) {
            if (C.marikoCpuMaxClock >= freq) {
                cpuCap = freq;
            } else {
                break;
            }
        }
        return cpuCap;
    }

    Result CpuFreqVdd(u32 *ptr) {
        dvfs_rail *entry = reinterpret_cast<dvfs_rail *>(reinterpret_cast<u8 *>(ptr) - offsetof(dvfs_rail, freq));

        R_UNLESS(entry->id      == 1,        ldr::ResultInvalidCpuFreqVddEntry());
        R_UNLESS(entry->min_mv  == 250'000,  ldr::ResultInvalidCpuFreqVddEntry());
        R_UNLESS(entry->step_mv == 5000,     ldr::ResultInvalidCpuFreqVddEntry());
        R_UNLESS(entry->max_mv  == 1525'000, ldr::ResultInvalidCpuFreqVddEntry());

        if (C.marikoCpuUVHigh) {
            PATCH_OFFSET(ptr, CapCpuClock());
        } else {
            PATCH_OFFSET(ptr, GetDvfsTableLastEntry(C.marikoCpuDvfsTable)->freq);
        }

        R_SUCCEED();
    }

    Result CpuVoltDVFS(u32 *ptr) {
        CvbMeta *cpuCvbMeta = reinterpret_cast<CvbMeta *>(reinterpret_cast<u8 *>(ptr) - offsetof(CvbMeta, vmin));

        R_UNLESS(cpuCvbMeta->highVmin     == CpuHighVminOfficial, ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->unkStepMaybe == 38,                  ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->vmax         == CpuVoltOfficial,     ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->unkScale2    == 1000,                ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->speedoScale  == 100,                 ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->voltageScale == 1000,                ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->unkZero5     == 0,                   ldr::ResultInvalidCpuMinVolt());

        if (C.marikoCpuLowVmin) {
            PATCH_OFFSET(&(cpuCvbMeta->vmin), C.marikoCpuLowVmin);
        }

        if (C.marikoCpuHighVmin) {
            PATCH_OFFSET(&(cpuCvbMeta->highVmin), C.marikoCpuHighVmin);
        }

        if (C.marikoCpuMaxVolt) {
            PATCH_OFFSET(&(cpuCvbMeta->vmax), C.marikoCpuMaxVolt);
        }

        R_SUCCEED();
    }

    Result CpuVoltThermals(u32 *ptr) {
        if (std::memcmp(ptr, cpuVoltThermalData, sizeof(cpuVoltThermalData))) {
            R_THROW(ldr::ResultInvalidCpuMinVolt());
        }

        if (C.marikoCpuLowVmin) {
            PATCH_OFFSET(ptr,     C.marikoCpuLowVmin);
            PATCH_OFFSET(ptr + 3, C.marikoCpuLowVmin);
        }

        if (C.marikoCpuMaxVolt) {
            PATCH_OFFSET(ptr - 2, C.marikoCpuMaxVolt);
            PATCH_OFFSET(ptr - 5, C.marikoCpuMaxVolt);
            PATCH_OFFSET(ptr + 1, C.marikoCpuMaxVolt);
            PATCH_OFFSET(ptr + 4, C.marikoCpuMaxVolt);
        }

        R_SUCCEED();
    }

    Result CpuVoltDfll(u32 *ptr) {
        CvbCpuDfllData *entry = reinterpret_cast<CvbCpuDfllData *>(ptr);

        R_UNLESS(entry->tune0_low  == 0xFFCF,    ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune0_high == 0x0,       ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune1_low  == 0x12207FF, ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune1_high == 0x3FFF7FF, ldr::ResultInvalidCpuVoltDfllEntry());

        switch (C.marikoCpuUVLow) {
            case 1:
                PATCH_OFFSET(&(entry->tune0_low),  0xffa0);
                PATCH_OFFSET(&(entry->tune0_high), 0xffff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21107ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x0);
                break;
            case 2:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_low),  0x21107ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27207ff);
                break;
            case 3:
                PATCH_OFFSET(&(entry->tune0_low),  0xffdf);
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_low),  0x21107ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27307ff);
                break;
            case 4:
                PATCH_OFFSET(&(entry->tune0_low),  0xffff);
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_low),  0x21107ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27407ff);
                break;
            case 5:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_low),  0x21607ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27707ff);
                break;
            case 6:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_low),  0x21607ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27807ff);
                break;
            case 7:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21607ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27b07ff);
                break;
            case 8:
                PATCH_OFFSET(&(entry->tune0_low),  0xdfff);
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21707ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27b07ff);
                break;
            case 9:
                PATCH_OFFSET(&(entry->tune0_low),  0xdfff);
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21707ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27c07ff);
                break;
            case 10:
                PATCH_OFFSET(&(entry->tune0_low),  0xdfff);
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21707ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27d07ff);
                break;
            case 11:
                PATCH_OFFSET(&(entry->tune0_low),  0xdfff);
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21707ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27e07ff);
                break;
            case 12:
                PATCH_OFFSET(&(entry->tune0_low),  0xdfff);
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21707ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27f07ff);
                break;
            default:
                break;
        }

        switch (C.marikoCpuUVHigh) {
            case 1:
                PATCH_OFFSET(&(entry->tune1_high), 0x0);
                PATCH_OFFSET(&(entry->tune0_high), 0xffff);
                break;
            case 2:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27207ff);
                break;
            case 3:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27307ff);
                break;
            case 4:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27407ff);
                break;
            case 5:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27707ff);
                break;
            case 6:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27807ff);
                break;
            case 7:
            case 8:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27b07ff);
                break;
            case 9:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27c07ff);
                break;
            case 10:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27d07ff);
                break;
            case 11:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27e07ff);
                break;
            case 12:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27f07ff);
                break;
            default:
                break;
        }

        R_SUCCEED();
    }

}
