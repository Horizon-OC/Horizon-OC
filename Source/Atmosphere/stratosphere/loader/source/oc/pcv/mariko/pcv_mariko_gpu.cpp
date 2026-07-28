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
#include "../pcv_asm.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    Result GpuVoltDVFS(u32 *ptr) {
        /* Check for valid pattern. */
        for (size_t i = 0; i < std::size(gpuDVFSPattern); ++i) {
            if (*(ptr + i + 1) != gpuDVFSPattern[i]) {
                R_THROW(ldr::ResultInvalidGpuDvfs());
            }
        }

        /* Default value is 1050mV. */
        if (C.marikoGpuVmax) {
            PATCH_OFFSET(ptr + 1, C.marikoGpuVmax);
        }

        if (C.marikoGpuVmin) {
            PATCH_OFFSET(ptr, C.marikoGpuVmin);
        }

        R_SUCCEED();
    }

    Result GpuVoltThermals(u32 *ptr) {
        if (std::memcmp(ptr - 3, gpuVoltThermalPattern, sizeof(gpuVoltThermalPattern))) {
            R_THROW(ldr::ResultInvalidGpuDvfs());
        }

        // if (C.marikoGpuBootVolt) {
        //     PATCH_OFFSET(ptr - 3, C.marikoGpuBootVolt);
        // }

        if (C.marikoGpuVmin) {
            PATCH_OFFSET(ptr,      C.marikoGpuVmin);
            PATCH_OFFSET(ptr +  3, C.marikoGpuVmin);
            PATCH_OFFSET(ptr +  6, C.marikoGpuVmin);
            PATCH_OFFSET(ptr +  9, C.marikoGpuVmin);
            PATCH_OFFSET(ptr + 12, C.marikoGpuVmin);
        }

        R_SUCCEED();
    }

    Result GpuFreqMaxAsm(u32 *ptr32) {
        // Check if both two instructions match the pattern
        u32 ins1 = *ptr32, ins2 = *(ptr32 + 1);
        if (!(asm_compare_no_rd(ins1, GpuAsmPattern[0]) && asm_compare_no_rd(ins2, GpuAsmPattern[1]))) {
            R_THROW(ldr::ResultInvalidGpuFreqMaxPattern());
        }

        // Both instructions should operate on the same register
        u8 rd = asm_get_rd(ins1);
        if (rd != asm_get_rd(ins2)) {
            R_THROW(ldr::ResultInvalidGpuFreqMaxPattern());
        }

        u32 max_clock;
        switch (C.marikoGpuUV) {
        case 0:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTable)->freq;
            break;
        case 1:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTableSLT)->freq;
            break;
        case 2:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTableHiOPT)->freq;
            break;
        case 3:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTableHiOPT15)->freq;
            break;
        case 4:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTableHighUV)->freq;
            break;
        default:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTableHiOPT)->freq;
            break;
        }

        u32 asm_patch[2] = {
            asm_set_rd(asm_set_imm16(GpuAsmPattern[0], max_clock), rd),
            asm_set_rd(asm_set_imm16(GpuAsmPattern[1], max_clock >> 16), rd)
        };

        PATCH_OFFSET(ptr32,     asm_patch[0]);
        PATCH_OFFSET(ptr32 + 1, asm_patch[1]);

        R_SUCCEED();
    }

    Result GpuFreqPllMax(u32 *ptr) {
        clk_pll_param *entry = reinterpret_cast<clk_pll_param *>(ptr);

        // All zero except for freq
        for (size_t i = 1; i < sizeof(clk_pll_param) / sizeof(u32); i++) {
            R_UNLESS(*(ptr + i) == 0, ldr::ResultInvalidGpuPllEntry());
        }

        // Double the max clk simply
        u32 max_clk = entry->freq * 2;
        entry->freq = max_clk;
        R_SUCCEED();
    }

    Result GpuFreqPllLimit(u32 *ptr) {
        u32 prev_freq = *(ptr - 1);

        if (prev_freq != 128000 && prev_freq != 1300000 && prev_freq != 76800) {
            R_THROW(ldr::ResultInvalidGpuPllEntry());
        }

        PATCH_OFFSET(ptr, 3600000);

        R_SUCCEED();
    }

}
