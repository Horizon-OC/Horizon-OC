/*
 * Copyright (C) hanai3bi (meha) and Souldbminer
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

namespace ams::ldr::oc::pcv {

Result MemFreqPllmLimit(u32* ptr) {
    clk_pll_param* entry = reinterpret_cast<clk_pll_param *>(ptr);
    R_UNLESS(entry->freq == entry->vco_max, ldr::ResultInvalidMemPllmEntry());

    // Double the max clk simply
    u32 max_clk = entry->freq * 2;
    entry->freq = max_clk;
    entry->vco_max = max_clk;
    R_SUCCEED();
}

Result MemVoltHandler(u32* ptr) {
    // ptr value might be default_uv or max_uv
    regulator* entries[2] = {
        reinterpret_cast<regulator *>(reinterpret_cast<u8 *>(ptr) - offsetof(regulator, type_1.default_uv)),
        reinterpret_cast<regulator *>(reinterpret_cast<u8 *>(ptr) - offsetof(regulator, type_1.max_uv)),
    };

    constexpr u32 uv_step = 12'500;
    constexpr u32 uv_min  = 600'000;

    auto validator = [](regulator* entry) {
        R_UNLESS(entry->id == 1,                    ldr::ResultInvalidRegulatorEntry());
        R_UNLESS(entry->type == 1,                  ldr::ResultInvalidRegulatorEntry());
        R_UNLESS(entry->type_1.volt_reg == 0x17,    ldr::ResultInvalidRegulatorEntry());
        R_UNLESS(entry->type_1.step_uv == uv_step,  ldr::ResultInvalidRegulatorEntry());
        R_UNLESS(entry->type_1.min_uv == uv_min,    ldr::ResultInvalidRegulatorEntry());
        R_SUCCEED();
    };

    regulator* entry = nullptr;
    for (auto& i : entries) {
        if (R_SUCCEEDED(validator(i)))
            entry = i;
    }

    R_UNLESS(entry, ldr::ResultInvalidRegulatorEntry());

    u32 emc_uv = C.commonEmcMemVolt;
    if (!emc_uv)
        R_SKIP();

    if (emc_uv % uv_step)
        emc_uv = emc_uv / uv_step * uv_step; // rounding

    PATCH_OFFSET(ptr, emc_uv);

    R_SUCCEED();
}

void SafetyCheck() {
    if (C.custRev != CUST_REV)
        CRASH("Triggered");

    struct sValidator {
        volatile u32 value;
        u32 min;
        u32 max;
        bool value_required = false;

        Result check() {
            if (!value_required && !value)
                R_SUCCEED();

            if (min && value < min)
                R_THROW(ldr::ResultSafetyCheckFailure());
            if (max && value > max)
                R_THROW(ldr::ResultSafetyCheckFailure());

            R_SUCCEED();
        }
    };

    u32 eristaCpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.eristaCpuDvfsTable)->freq);
    u32 marikoCpuDvfsMaxFreq = static_cast<u32>(C.marikoCpuUV ? GetDvfsTableLastEntry(C.marikoCpuDvfsTableSLT)->freq : GetDvfsTableLastEntry(C.marikoCpuDvfsTable)->freq);
    u32 eristaGpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.eristaGpuDvfsTable)->freq);
    u32 marikoGpuDvfsMaxFreq;
    u32 temporaryMaxFreqForCalculation = C.gpuMaxFreq;
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
            marikoGpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.marikoGpuDvfsTableUv3)->freq);
            temporaryMaxFreqForCalculation = 1536'000;
            break;
        default: 
            marikoGpuDvfsMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(C.marikoGpuDvfsTable)->freq);
            break;
    }
    
    sValidator validators[] = {
        { C.commonCpuBoostClock, 1020'000, C.cpuMaxFreq + 1, true },
        { C.commonEmcMemVolt,    1000'000, C.MemVltMax + 1 },
        { C.eristaCpuMaxVolt,        1100,     C.eristaCPUvMax + 1 },
        { C.eristaEmcMaxClock,   1600'000, 2428'800 },
        { C.marikoCpuMaxVolt,         800,     C.marikoCPUvMax + 1 },
        { C.marikoEmcMaxClock,   1600'000, 3504'000 },
        { C.marikoEmcVddqVolt,    550'000,  C.marikoVDDQMax + 1 },
        { eristaCpuDvfsMaxFreq,  1785'000, C.cpuMaxFreq + 1 },
        { marikoCpuDvfsMaxFreq,  1785'000, C.cpuMaxFreq + 1 },
        { eristaGpuDvfsMaxFreq,   76'800, C.gpuMaxFreq + 1 },
        { marikoGpuDvfsMaxFreq,   76'800, temporaryMaxFreqForCalculation + 1 },
    };

    for (auto& i : validators) {
        if (R_FAILED(i.check()))
            CRASH("Triggered");
    }
}

void Patch(uintptr_t mapped_nso, size_t nso_size) {
    #ifdef ATMOSPHERE_IS_STRATOSPHERE
    SafetyCheck();
    bool isMariko = (spl::GetSocType() == spl::SocType_Mariko);
    if (isMariko)
        mariko::Patch(mapped_nso, nso_size);
    else
        erista::Patch(mapped_nso, nso_size);
    #endif
}

}
