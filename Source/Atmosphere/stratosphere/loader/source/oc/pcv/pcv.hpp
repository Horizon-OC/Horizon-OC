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

#include "../oc_common.hpp"
#include "pcv_common.hpp"
#include "pcv_erista.hpp"
#include "pcv_mariko.hpp"

namespace ams::ldr::hoc::pcv {

    inline auto MatchesPattern = [](u32 *base, const auto &offsets, const auto &values) {
        for (size_t i = 0; i < std::size(values); ++i) {
            if (*(base + offsets[i]) != values[i]) {
                return false;
            }
        }
        return true;
    };

    template <bool isMariko>
    Result CpuFreqCvbTable(u32 *ptr) {
        cvb_entry_t *default_table = isMariko ? (cvb_entry_t *)(&mariko::CpuCvbTableDefault) : (cvb_entry_t *)(&erista::CpuCvbTableDefault);
        cvb_entry_t *customize_table = nullptr;

        if (isMariko) {
            switch (C.tableConf) {
                case TBREAK_1683: {
                    customize_table = const_cast<cvb_entry_t *>(C.marikoCpuDvfsTable1683Tbreak);
                    break;
                }
                case TBREAK_1581: {
                    customize_table = const_cast<cvb_entry_t *>(C.marikoCpuDvfsTable1581Tbreak);
                    break;
                }
                case EXTREME_TABLE: {
                    customize_table = const_cast<cvb_entry_t *>(C.marikoCpuDvfsTableExtreme);
                    break;
                }
                case DEFAULT_TABLE:
                default: {
                    customize_table = default_table;
                    break;
                }
            }
        } else {
            if (C.eristaCpuUV) {
                if (C.eristaCpuUnlock) {
                    customize_table = const_cast<cvb_entry_t *>(C.eristaCpuDvfsTableSLT);
                } else {
                    customize_table = const_cast<cvb_entry_t *>(C.eristaCpuDvfsTable);
                }
            } else {
                customize_table = default_table;
            }
        }

        u32 cpu_max_volt = isMariko ? C.marikoCpuMaxVolt : C.eristaCpuMaxVolt;
        u32 cpu_freq_threshold = 2091'000;

        size_t default_entry_count = GetDvfsTableEntryCount(default_table);
        size_t default_table_size = default_entry_count * sizeof(cvb_entry_t);
        size_t customize_entry_count = GetDvfsTableEntryCount(customize_table);
        size_t customize_table_size = customize_entry_count * sizeof(cvb_entry_t);

        cvb_entry_t *table_free = reinterpret_cast<cvb_entry_t *>(ptr) + 1;
        void *cpu_cvb_table_head = reinterpret_cast<u8 *>(table_free) - default_table_size;
        bool validated = std::memcmp(cpu_cvb_table_head, default_table, default_table_size) == 0;
        R_UNLESS(validated, ldr::ResultInvalidCpuDvfs());

        std::memcpy(cpu_cvb_table_head, static_cast<void *>(customize_table), customize_table_size);

        if (cpu_max_volt) {
            cvb_entry_t *entry = static_cast<cvb_entry_t *>(cpu_cvb_table_head);
            for (size_t i = 0; i < customize_entry_count; i++) {
                if (entry->freq >= cpu_freq_threshold) {
                    if (isMariko) {
                        PATCH_OFFSET(&(entry->cvb_pll_param.c0), cpu_max_volt * 1000);
                    } else {
                        // PATCH_OFFSET(&(entry->cvb_dfll_param.c0), cpu_max_volt * 1000);
                    }
                }
                entry++;
            }
        }
        R_SUCCEED();
    }

    constexpr void ClearCvbPllEntry(cvb_entry_t *entry) {
        PATCH_OFFSET(&(entry->cvb_pll_param.c1), 0);
        PATCH_OFFSET(&(entry->cvb_pll_param.c2), 0);
        PATCH_OFFSET(&(entry->cvb_pll_param.c3), 0);
        PATCH_OFFSET(&(entry->cvb_pll_param.c4), 0);
        PATCH_OFFSET(&(entry->cvb_pll_param.c5), 0);
    }

    template <bool isMariko>
    Result GpuFreqCvbTable(u32 *ptr) {
        cvb_entry_t *default_table = isMariko ? (cvb_entry_t *)(&mariko::GpuCvbTableDefault) : (cvb_entry_t *)(&erista::GpuCvbTableDefault);
        cvb_entry_t *customize_table;
        if (isMariko) {
            switch (C.marikoGpuUV) {
                case 0:
                    customize_table = const_cast<cvb_entry_t *>(C.marikoGpuDvfsTable);
                    break;
                case 1:
                    customize_table = const_cast<cvb_entry_t *>(C.marikoGpuDvfsTableSLT);
                    break;
                case 2:
                    customize_table = const_cast<cvb_entry_t *>(C.marikoGpuDvfsTableHiOPT);
                    break;
                case 3:
                    customize_table = const_cast<cvb_entry_t *>(C.marikoGpuDvfsTableHiOPT15);
                    break;
                case 4:
                    customize_table = const_cast<cvb_entry_t *>(C.marikoGpuDvfsTableHighUV);
                    break;
                default:
                    customize_table = const_cast<cvb_entry_t *>(C.marikoGpuDvfsTableHiOPT);
                    break;
                }
        } else {
            switch (C.eristaGpuUV) {
                case 0:
                    customize_table = const_cast<cvb_entry_t *>(C.eristaGpuDvfsTable);
                    break;
                case 1:
                    customize_table = const_cast<cvb_entry_t *>(C.eristaGpuDvfsTableSLT);
                    break;
                case 2:
                    customize_table = const_cast<cvb_entry_t *>(C.eristaGpuDvfsTableHiOPT);
                    break;
                default:
                    customize_table = const_cast<cvb_entry_t *>(C.eristaGpuDvfsTable);
                    break;
                }
        }

        size_t default_entry_count = GetDvfsTableEntryCount(default_table);
        size_t default_table_size = default_entry_count * sizeof(cvb_entry_t);
        size_t customize_entry_count = GetDvfsTableEntryCount(customize_table);
        size_t customize_table_size = customize_entry_count * sizeof(cvb_entry_t);

        // Validate existing table
        cvb_entry_t *table_free = reinterpret_cast<cvb_entry_t *>(ptr) + 1;
        void *gpu_cvb_table_head = reinterpret_cast<u8 *>(table_free) - default_table_size;
        bool validated = std::memcmp(gpu_cvb_table_head, default_table, default_table_size) == 0;
        R_UNLESS(validated, ldr::ResultInvalidGpuDvfs());

        std::memcpy(gpu_cvb_table_head, (void *)customize_table, customize_table_size);

        // Patch GPU volt
        cvb_entry_t *entry = static_cast<cvb_entry_t *>(gpu_cvb_table_head);
        for (size_t i = 0; i < customize_entry_count; ++i) {
            if (isMariko) {
                if (C.marikoGpuVoltArray[i] != 0) {
                    PATCH_OFFSET(&(entry->cvb_pll_param.c0), (C.marikoGpuVoltArray[i] * 1000));
                    ClearCvbPllEntry(entry);
                } else {
                    PATCH_OFFSET(&(entry->cvb_pll_param.c0), (u32)((s32)entry->cvb_pll_param.c0 + C.commonGpuVoltOffset * 1000));
                }
            } else {
                if (C.eristaGpuVoltArray[i] != 0) {
                    PATCH_OFFSET(&(entry->cvb_pll_param.c0), (C.eristaGpuVoltArray[i] * 1000));
                    ClearCvbPllEntry(entry);
                } else {
                    PATCH_OFFSET(&(entry->cvb_pll_param.c0), (u32)((s32)entry->cvb_pll_param.c0 + C.commonGpuVoltOffset * 1000));
                }
            }
            ++entry;
        }
        if (C.commonGpuVoltOffset && !(isMariko ? C.marikoGpuUV : C.eristaGpuUV)) {
            cvb_entry_t *entry = static_cast<cvb_entry_t *>(gpu_cvb_table_head);
            for (size_t i = 0; i < customize_entry_count; ++i) {
                PATCH_OFFSET(&(entry->cvb_pll_param.c0), (u32)((s32)entry->cvb_pll_param.c0 + C.commonGpuVoltOffset * 1000));
                ++entry;
            }
        }

        R_SUCCEED();
    };

    Result MemFreqPllmLimit(u32 *ptr);
    Result MemVoltHandler(u32 *ptr); // Used for Erista MEM Vdd2 + EMC Vddq or Mariko MEM Vdd2

    void SafetyCheck();
    void Patch(uintptr_t mapped_nso, size_t nso_size);

}
