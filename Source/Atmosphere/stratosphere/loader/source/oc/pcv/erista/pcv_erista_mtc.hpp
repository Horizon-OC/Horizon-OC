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
#include "../../mtc_timing_table.hpp"

namespace ams::ldr::hoc::pcv::erista {

    constexpr u32 EmcListDefault[]   = { 40800, 68000, 102000, 204000, 408000, 665600, 800000, 1065600, 1331200, 1600000, };
    constexpr u32 EmcListSizeDefault = std::size(EmcListDefault);
    constexpr u32 EmcListEndDefault  = EmcListSizeDefault - 1;

    constexpr u32 MemVoltHOS      = 1125'000;
    constexpr u32 EmcClkPllmLimit = 1866'000'000;

    constexpr u32 MTC_TABLE_REV        = 7;
    constexpr u32 MtcTableCountDefault = 10;

    constexpr size_t MtcFullTableSize  = sizeof(EristaMtcTable) * MtcTableCountDefault;
    constexpr u32 MtcFullTableCount    = 3;

    /* These dramids were copied from Hekate -- see /bdk/mem/sdram.h */
    enum DramId {
        ICOSA_4GB_SAMSUNG_K4F6E304HB_MGCH        = 0,
        ICOSA_4GB_HYNIX_H9HCNNNBPUMLHR_NLE       = 1,
        ICOSA_4GB_MICRON_MT53B512M32D2NP_062_WTC = 2,
        ICOSA_6GB_SAMSUNG_K4FHE3D4HM_MGCH        = 4,
        ICOSA_8GB_SAMSUNG_K4FBE3D4HM_MGXX        = 7,
    };

    enum MtcTableIndex {
        T210SdevEmcDvfsTableS4gb01 = 0, /* HB-MGCH, WT:C */
        T210SdevEmcDvfsTableS6gb01 = 1, /* HM-MGCH */
        T210SdevEmcDvfsTableH4gb01 = 2, /* HR-NLE  */
        MtcTableIndex_Invalid      = 3,
    };

    struct MtcDramIndex {
        DramId dramId;
        MtcTableIndex index;
    };

    /* TODO: Test 6gb and 8gb. */
    const inline MtcDramIndex mtcIndexTable[] = {
        { ICOSA_4GB_SAMSUNG_K4F6E304HB_MGCH,        T210SdevEmcDvfsTableS4gb01, },
        { ICOSA_4GB_MICRON_MT53B512M32D2NP_062_WTC, T210SdevEmcDvfsTableS4gb01, },
        { ICOSA_6GB_SAMSUNG_K4FHE3D4HM_MGCH,        T210SdevEmcDvfsTableS6gb01, },
        { ICOSA_8GB_SAMSUNG_K4FBE3D4HM_MGXX,        T210SdevEmcDvfsTableS6gb01, },
        { ICOSA_4GB_HYNIX_H9HCNNNBPUMLHR_NLE,       T210SdevEmcDvfsTableH4gb01, },
    };

    constexpr u32 MtcBrAsm   = 0xD61F0140;
    constexpr u32 MtcMovAsm  = 0x52800148;
    constexpr u32 MtcAdrpAsm = 0xD0000081;
    constexpr u32 MtcBlIns = 0x97ffae64;
    constexpr u32 MtcAddAsm = 0x91131821;

    ALWAYS_INLINE bool MemMtcGetGetTablePatternFn(u32 *ptr) {
        /* This builds an address that gets returned, so the register must be x0 by convention. */
        return AsmCompareAddNoImm12(*ptr, MtcAddAsm);
    }

    Result MemFreqMtcTable(u32 *ptr);
    void MtcGenerateFreqTables();
    Result MemFreqMax(u32 *ptr);
    HOOK_PAYLOAD_FN EristaMtcTable *GetEristaMtcTableImpl(u32 *count);
    Result MemMtcTableAsm(u32 *ptr);

    Result MtcInstallHooks(HookPayloadData *data);

}
