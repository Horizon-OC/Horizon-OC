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
#include "pcv_asm.hpp"

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

    struct DvbEntry {
        u64 freq;
        u32 volt[4] = {};
    };

    constexpr DvbEntry EmcDvbTableDefault[] = {
        {  204000, { 637, 637, 637, } },
        {  408000, { 637, 637, 637, } },
        {  800000, { 637, 637, 637, } },
        { 1065600, { 637, 637, 637, } },
        { 1331200, { 650, 637, 637, } },
        { 1600000, { 675, 650, 637, } },
    };

    /* Movz */
    /*
        SF | OPC                     | HW    | Imm16                                      | RD
        31 | 30 29 28 27 26 25 24 23 | 22 21 | 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 | 4 3 2 1 0
    */
    constexpr u32 SocVoltCompareSpeedoAsm  = 0x7118FAFF; /* subs imm, compares to >=1598 max speedo and then goes down process id 1 route. */
    constexpr u32 SocVoltWriteProcessIdAsm = 0x2A1F03F4; /* orr, writes id 0. */
    constexpr u32 SocVoltWriteVoltageAsm   = 0x52808358; /* Movz imm, writes 1050mV. */
    constexpr u32 SocVoltSelectRegisterAsm = 0x1A9A3118; /* Csel, selects the voltage -- we need the register of this. */
    constexpr u32 SocVoltMultiplyVoltsAsm  = 0x1B1A7F0B; /* Mul, converts from mV -> uV */
    constexpr u32 SocVoltValidateLimitAsm  = 0x6B0A017F; /* Subs, checks limits */
    constexpr u32 SocVoltBranchToAbortAsm  = 0x540020AC; /* B.ge Branches to abort if limits are invalid. */

    ALWAYS_INLINE bool SocVoltPatternFn(u32 *ptr) {
        return asm_compare_no_rd(*ptr, SocVoltCompareSpeedoAsm);
    }

    constexpr u32 SocVoltLimitOfficial                      = 1050;
    constexpr u32 SocVoltLimitMaxDefaultIndex               = 17;
    static const u32 socVoltLimitArray[DvfsTableEntryCount] = { 637, 650, 675, 700, 725, 750, 775, 800, 825, 850, 875, 900, 925, 950, 975, 1000, 1025, 1050, };

    constexpr u32 EmcListDefault[]   = { 204000, 1331200, 1600000, };
    constexpr u32 EmcListSizeDefault = std::size(EmcListDefault);
    constexpr u32 EmcListEndDefault  = EmcListSizeDefault - 1;

    constexpr u32 EmcClkOSAlt     = 1331'200;
    constexpr u32 EmcClkPllmLimit = 2133'000'000;
    constexpr u32 EmcVddqDefault  = 600'000;
    constexpr u32 MemVdd2Default  = 1100'000;

    constexpr u32 MTC_TABLE_REV        = 3;
    constexpr u32 MtcTableCountDefault = 3;

    constexpr size_t MtcFullTableSize  = sizeof(MarikoMtcTable) * MtcTableCountDefault;
    constexpr u32 MtcFullTableCount    = 17;

    /* These dramids were copied from Hekate -- see /bdk/mem/sdram.h */
    enum DramId : u64 {
        HOAG_4GB_HYNIX_H9HCNNNBKMMLXR_NEE       = 3,
        AULA_4GB_HYNIX_H9HCNNNBKMMLXR_NEE       = 5,
        IOWA_4GB_HYNIX_H9HCNNNBKMMLXR_NEE       = 6,

        IOWA_4GB_SAMSUNG_K4U6E3S4AM_MGCJ        = 8,
        IOWA_8GB_SAMSUNG_K4UBE3D4AM_MGCJ        = 9,
        IOWA_4GB_HYNIX_H9HCNNNBKMMLHR_NME       = 10,
        IOWA_4GB_MICRON_MT53E512M32D2NP_046_WTE = 11,

        HOAG_4GB_SAMSUNG_K4U6E3S4AM_MGCJ        = 12,
        HOAG_8GB_SAMSUNG_K4UBE3D4AM_MGCJ        = 13,
        HOAG_4GB_HYNIX_H9HCNNNBKMMLHR_NME       = 14,
        HOAG_4GB_MICRON_MT53E512M32D2NP_046_WTE = 15,

        IOWA_4GB_SAMSUNG_K4U6E3S4AA_MGCL        = 17,
        IOWA_8GB_SAMSUNG_K4UBE3D4AA_MGCL        = 18,
        HOAG_4GB_SAMSUNG_K4U6E3S4AA_MGCL        = 19,

        IOWA_4GB_SAMSUNG_K4U6E3S4AB_MGCL        = 20,
        HOAG_4GB_SAMSUNG_K4U6E3S4AB_MGCL        = 21,
        AULA_4GB_SAMSUNG_K4U6E3S4AB_MGCL        = 22,

        HOAG_8GB_SAMSUNG_K4UBE3D4AA_MGCL        = 23,
        AULA_4GB_SAMSUNG_K4U6E3S4AA_MGCL        = 24,

        IOWA_4GB_MICRON_MT53E512M32D2NP_046_WTF = 25,
        HOAG_4GB_MICRON_MT53E512M32D2NP_046_WTF = 26,
        AULA_4GB_MICRON_MT53E512M32D2NP_046_WTF = 27,

        AULA_8GB_SAMSUNG_K4UBE3D4AA_MGCL        = 28,

        IOWA_4GB_HYNIX_H54G46CYRBX267           = 29,
        HOAG_4GB_HYNIX_H54G46CYRBX267           = 30,
        AULA_4GB_HYNIX_H54G46CYRBX267           = 31,

        IOWA_4GB_MICRON_MT53E512M32D1NP_046_WTB = 32,
        HOAG_4GB_MICRON_MT53E512M32D1NP_046_WTB = 33,
        AULA_4GB_MICRON_MT53E512M32D1NP_046_WTB = 34,
    };

    enum MtcTableIndex {
        T210b0SdevEmcDvfsTableS4gb01    =   0, /* (Unused) Samsung 4Gb */
        T210b0SdevEmcDvfsTableS4gb03    =   1, /* Samsung AM-MGCJ  4Gb */
        T210b0SdevEmcDvfsTableS8gb03    =   2, /* (Unused) Samsung 4Gb */
        T210b0SdevEmcDvfsTableH4gb03    =   3, /* Hynix NME        4Gb */
        T210b0SdevEmcDvfsTableM4gb03    =   4, /* Micron WT:F      4Gb */
        T210b0SdevEmcDvfsTableS4gbY01   =   5, /* (Unused) Samsung 4Gb */
        T210b0SdevEmcDvfsTableS1y4gbY01 =   6, /* (Unused) Samsung 4Gb */
        T210b0SdevEmcDvfsTableS1y8gbY01 =   7, /* (Unused) Samsung 4Gb */
        T210b0SdevEmcDvfsTableS1y4gbX03 =   8, /* Samsung AA-MGCL  4Gb */
        T210b0SdevEmcDvfsTableS1y8gbX03 =   9, /* Samsung AA-MGCL  8Gb */
        T210b0SdevEmcDvfsTableS1y4gb01  =  10, /* (Unused) Samsung 4Gb */
        T210b0SdevEmcDvfsTableM1y4gb01  =  11, /* Micron WT:E      4Gb */
        T210b0SdevEmcDvfsTableH1y4gb01  =  12, /* Hynix NEE        4Gb */
        T210b0SdevEmcDvfsTableS1y8gb04  =  13, /* Samsung AM-MGCJ  8Gb */
        T210b0SdevEmcDvfsTableS1z4gb01  =  14, /* Samsung AB-MGCL  4Gb */
        T210b0SdevEmcDvfsTableH1a4gb01  =  15, /* Hynix x267       4Gb */
        T210b0SdevEmcDvfsTableM1a4gb01  =  16, /* Micron WT:B      8Gb */
        MtcTableIndex_Invalid           =  17,
    };

    struct MtcDramIndex {
        DramId dramId;
        MtcTableIndex index;
    };

    const inline MtcDramIndex mtcIndexTable[] = {
        { HOAG_4GB_HYNIX_H9HCNNNBKMMLXR_NEE,       T210b0SdevEmcDvfsTableH1y4gb01,  },
        { AULA_4GB_HYNIX_H9HCNNNBKMMLXR_NEE,       T210b0SdevEmcDvfsTableH1y4gb01,  },
        { IOWA_4GB_HYNIX_H9HCNNNBKMMLXR_NEE,       T210b0SdevEmcDvfsTableH1y4gb01,  },
        { IOWA_4GB_SAMSUNG_K4U6E3S4AM_MGCJ,        T210b0SdevEmcDvfsTableS4gb03,    },
        { IOWA_8GB_SAMSUNG_K4UBE3D4AM_MGCJ,        T210b0SdevEmcDvfsTableS1y8gb04,  },
        { IOWA_4GB_HYNIX_H9HCNNNBKMMLHR_NME,       T210b0SdevEmcDvfsTableH4gb03,    },
        { IOWA_4GB_MICRON_MT53E512M32D2NP_046_WTE, T210b0SdevEmcDvfsTableM1y4gb01,  },
        { HOAG_4GB_SAMSUNG_K4U6E3S4AM_MGCJ,        T210b0SdevEmcDvfsTableS4gb03,    },
        { HOAG_8GB_SAMSUNG_K4UBE3D4AM_MGCJ,        T210b0SdevEmcDvfsTableS1y8gb04,  },
        { HOAG_4GB_HYNIX_H9HCNNNBKMMLHR_NME,       T210b0SdevEmcDvfsTableH4gb03,    },
        { HOAG_4GB_MICRON_MT53E512M32D2NP_046_WTE, T210b0SdevEmcDvfsTableM1y4gb01,  },
        { IOWA_4GB_SAMSUNG_K4U6E3S4AA_MGCL,        T210b0SdevEmcDvfsTableS1y4gbX03, },
        { IOWA_8GB_SAMSUNG_K4UBE3D4AA_MGCL,        T210b0SdevEmcDvfsTableS1y8gbX03, },
        { HOAG_4GB_SAMSUNG_K4U6E3S4AA_MGCL,        T210b0SdevEmcDvfsTableS1y4gbX03, },
        { IOWA_4GB_SAMSUNG_K4U6E3S4AB_MGCL,        T210b0SdevEmcDvfsTableS1z4gb01,  },
        { HOAG_4GB_SAMSUNG_K4U6E3S4AB_MGCL,        T210b0SdevEmcDvfsTableS1y8gb04,  },
        { AULA_4GB_SAMSUNG_K4U6E3S4AB_MGCL,        T210b0SdevEmcDvfsTableS1y8gb04,  },
        { HOAG_8GB_SAMSUNG_K4UBE3D4AA_MGCL,        T210b0SdevEmcDvfsTableS1y8gbX03, },
        { AULA_4GB_SAMSUNG_K4U6E3S4AA_MGCL,        T210b0SdevEmcDvfsTableS1y4gbX03, },
        { IOWA_4GB_MICRON_MT53E512M32D2NP_046_WTF, T210b0SdevEmcDvfsTableM4gb03,    },
        { HOAG_4GB_MICRON_MT53E512M32D2NP_046_WTF, T210b0SdevEmcDvfsTableM4gb03,    },
        { AULA_4GB_MICRON_MT53E512M32D2NP_046_WTF, T210b0SdevEmcDvfsTableM4gb03,    },
        { AULA_8GB_SAMSUNG_K4UBE3D4AA_MGCL,        T210b0SdevEmcDvfsTableS1y8gbX03, },
        { IOWA_4GB_HYNIX_H54G46CYRBX267,           T210b0SdevEmcDvfsTableH1a4gb01,  },
        { HOAG_4GB_HYNIX_H54G46CYRBX267,           T210b0SdevEmcDvfsTableH1a4gb01,  },
        { AULA_4GB_HYNIX_H54G46CYRBX267,           T210b0SdevEmcDvfsTableH1a4gb01,  },
        { IOWA_4GB_MICRON_MT53E512M32D1NP_046_WTB, T210b0SdevEmcDvfsTableM1a4gb01,  },
        { HOAG_4GB_MICRON_MT53E512M32D1NP_046_WTB, T210b0SdevEmcDvfsTableM1a4gb01,  },
        { AULA_4GB_MICRON_MT53E512M32D1NP_046_WTB, T210b0SdevEmcDvfsTableM1a4gb01,  },
    };

    /*
        710006abfc 40 01 1f d6     br         x10
    */

    /*
        710006ac28 a0 03 00 90     adrp       x0,0x71000de000
        710006ac2c 00 80 16 91     add        x0=>SdevEmcDvfsTableS4gb01,x0,#0x5a0
    */

    /* Br */
    /*
                             | Z     | OP | Fixed                         | A | M  | RN        | RM
        31 30 29 28 27 26 25 | 24 23 | 22 | 21 20 19 18 17 16 15 14 13 12 |11 | 10 | 9 8 7 6 5 | 4 3 2 1 0
        1 1 0 1 0 1 1 0 0 0 0 1 1 1 1 1 0 0 0 0 0 0 Rn 0 0 0 0 0
        Z  op   A M  Rm
    */

    /* Adrp */
    /*
        OP | ImmLow |                | ImmHigh                                             | RD
        31 | 30 29  | 28 27 26 25 24 | 23 22 21 20 19 18 17 16 15 14 13 12 11 10 9 8 7 6 5 | 4 3 2 1 0
    */

    /* ADD (immediate) */
    /*
        SF | OP | S  | Fixed value       | Sh | Imm12                               | RN        | RD
        31 | 30 | 29 | 28 27 26 25 24 23 | 22 | 21 20 19 18 17 16 15 14 13 12 11 10 | 9 8 7 6 5 | 4 3 2 1 0
    */

    constexpr u32 MtcBrAsm   = 0xD61F0140;
    constexpr u32 MtcMovAsm  = 0x52800068;
    constexpr u32 MtcAdrpAsm = 0x900003A0;
    constexpr u32 MtcAddAsm  = 0x91168000;

    ALWAYS_INLINE bool MemMtcGetGetTablePatternFn(u32 *ptr) {
        /* This builds an address that gets returned, so the register must be x0 by convention. */
        return AsmCompareAddNoImm12(*ptr, MtcAddAsm);
    }

    void Patch(uintptr_t mapped_nso, size_t nso_size);

}
