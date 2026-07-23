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

#include "pcv.hpp"

namespace ams::ldr::hoc::pcv {

    Result MemFreqPllmLimit(u32 *ptr) {
        clk_pll_param *entry = reinterpret_cast<clk_pll_param *>(ptr);
        R_UNLESS(entry->freq == entry->vco_max, ldr::ResultInvalidMemPllmEntry());

        // Double the max clk simply
        u32 max_clk = entry->freq * 2;
        entry->freq = max_clk;
        entry->vco_max = max_clk;
        R_SUCCEED();
    }

    Result MemVoltHandler(u32 *ptr) {
        // ptr value might be default_uv or max_uv
        regulator *entries[2] = {
            reinterpret_cast<regulator *>(reinterpret_cast<u8 *>(ptr) - offsetof(regulator, type_1.default_uv)),
            reinterpret_cast<regulator *>(reinterpret_cast<u8 *>(ptr) - offsetof(regulator, type_1.max_uv)),
        };

        constexpr u32 uv_step = 12'500;
        constexpr u32 uv_min = 600'000;

        auto validator = [](regulator *entry) {
            R_UNLESS(entry->id              == 1,       ldr::ResultInvalidRegulatorEntry());
            R_UNLESS(entry->type            == 1,       ldr::ResultInvalidRegulatorEntry());
            R_UNLESS(entry->type_1.volt_reg == 0x17,    ldr::ResultInvalidRegulatorEntry());
            R_UNLESS(entry->type_1.step_uv  == uv_step, ldr::ResultInvalidRegulatorEntry());
            R_UNLESS(entry->type_1.min_uv   == uv_min,  ldr::ResultInvalidRegulatorEntry());
            R_SUCCEED();
        };

        regulator *entry = nullptr;
        for (auto &i : entries) {
            if (R_SUCCEEDED(validator(i))) {
                entry = i;
            }
        }

        R_UNLESS(entry, ldr::ResultInvalidRegulatorEntry());

        u32 emc_uv = C.commonEmcMemVolt;
        if (!emc_uv) {
            R_SKIP();
        }

        if (emc_uv % uv_step) {
            emc_uv = emc_uv / uv_step * uv_step;  // rounding
        }

        PATCH_OFFSET(ptr, emc_uv);

        R_SUCCEED();
    }

    void SafetyCheck() {
        struct Validator {
            volatile u32 value;
            u32 min;
            u32 max;
            u32 panic;
            bool value_required = false;

            Result Check() {
                if (!value_required && !value) {
                    R_SUCCEED();
                }

                if (min && value < min) {
                    R_THROW(ldr::ResultSafetyCheckFailure());
                }

                if (max && value > max) {
                    R_THROW(ldr::ResultSafetyCheckFailure());
                }

                R_SUCCEED();
            }
        };

        u32 eristaCpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.eristaCpuDvfsTable)->freq);
        u32 marikoCpuDvfsMaxFreq;
        if (C.marikoCpuUVHigh) {
            marikoCpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.marikoCpuDvfsTableSLT)->freq);
        } else {
            marikoCpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.marikoCpuDvfsTable)->freq);
        }
        u32 eristaGpuDvfsMaxFreq;
        switch (C.eristaGpuUV) {
            case 0:
                eristaGpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.eristaGpuDvfsTable)->freq);
                break;
            case 1:
                eristaGpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.eristaGpuDvfsTableSLT)->freq);
                break;
            case 2:
                eristaGpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.eristaGpuDvfsTableHiOPT)->freq);
                break;
            default:
                eristaGpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.eristaGpuDvfsTable)->freq);
                break;
        }

        u32 marikoGpuDvfsMaxFreq;
        switch (C.marikoGpuUV) {
            case 0:
                marikoGpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.marikoGpuDvfsTable)->freq);
                break;
            case 1:
                marikoGpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.marikoGpuDvfsTableSLT)->freq);
                break;
            case 2:
                marikoGpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.marikoGpuDvfsTableHiOPT)->freq);
                break;
            case 3:
                marikoGpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.marikoGpuDvfsTableHiOPT15)->freq);
                break;
            case 4:
                marikoGpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.marikoGpuDvfsTableHighUV)->freq);
                break;
            default:
                marikoGpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.marikoGpuDvfsTableHiOPT)->freq);
                break;
        }

        Validator validators[] = {
            { C.eristaCpuBoostClock, 1020'000, 2397'000, panic::Cpu, true },
            { C.marikoCpuBoostClock, 1020'000, 2703'000, panic::Cpu, true },
            { C.eristaCpuMaxVolt,        1000,     1260, panic::Cpu,      },
            { C.marikoCpuMaxVolt,        1000,     1200, panic::Cpu,      },
            { eristaCpuDvfsMaxFreq,  1785'000, 2397'000, panic::Cpu,      },
            { marikoCpuDvfsMaxFreq,  1785'000, 2703'000, panic::Cpu,      },
            { C.commonEmcMemVolt,     912'500, 1350'000, panic::Emc,      }, /* Official vmax for the RAMs is 1400-1500mV */
            { C.eristaEmcMaxClock,   1600'000, 2600'000, panic::Emc,      },
            { C.marikoEmcMaxClock,   1600'000, 3500'000, panic::Emc,      },
            { C.marikoEmcVddqVolt,    400'000,  750'000, panic::Emc,      },
            { C.marikoSocVmax,           1000,     1200, panic::Emc,      },
            { eristaGpuDvfsMaxFreq,   768'000, 1152'000, panic::Gpu,      },
            { marikoGpuDvfsMaxFreq,   768'000, 1536'000, panic::Gpu,      },
            { C.marikoGpuVmax,            800,      960, panic::Gpu,      },
        };

        for (auto &v : validators) {
            if (R_FAILED(v.Check())) {
                panic::SmcError(v.panic);
                CRASH("Validation FAIL");
            }
        }
    }

    void WriteKipLoadToIram() {
        const u32 hocMagic = 0x686F634D;
        constexpr uintptr_t LoadMagicAddress = 0x4003DC00; /* Should be a pretty safe address. */
        R_DISCARD(SmcCopyToIram(LoadMagicAddress, &hocMagic, sizeof(hocMagic)));
    }

    void Patch(uintptr_t mapped_nso, size_t nso_size) {
        SafetyCheck();

        bool isMariko = (spl::GetSocType() == spl::SocType_Mariko);
        if (isMariko) {
            mariko::Patch(mapped_nso, nso_size);
        } else {
            erista::Patch(mapped_nso, nso_size);
        }

        WriteKipLoadToIram();
    }

}
