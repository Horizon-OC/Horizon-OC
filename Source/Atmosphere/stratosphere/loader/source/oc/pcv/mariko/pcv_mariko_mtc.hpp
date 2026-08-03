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

#pragma once

#include "../pcv.hpp"

namespace ams::ldr::hoc::pcv::mariko {

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

    void MtcGenerateFreqTables();
    Result MemFreqMtcTable(u32 *ptr);
    Result MemFreqDvbTable(u32 *ptr);
    Result MemFreqMax(u32 *ptr);
    Result MemMtcTableAsm(u32 *ptr);

}
