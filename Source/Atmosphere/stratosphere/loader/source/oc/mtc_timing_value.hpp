/*
 * Copyright (c) 2023 hanai3Bi
 *
 * Copyright (c) 2025 Lightos_
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

#include "oc_common.hpp"

namespace ams::ldr::hoc {
    #define MAX(A, B) std::max(A, B)
    #define MIN(A, B) std::min(A, B)
    #define CEIL(A)   std::ceil(A)
    #define FLOOR(A)  std::floor(A)
    #define ROUND(A)  std::lround(A)

    #define PACK_U32(high, low) ((static_cast<u32>(high) << 16) | (static_cast<u32>(low) & 0xFFFF))
    #define PACK_U32_NIBBLE_HIGH_BYTE_LOW(high, low) ((static_cast<u32>(high & 0xF) << 28) | (static_cast<u32>(low) & 0xFF))

    /* Burst latency, not to be confused with base latency (tWRL). */
    const u32 BL = 16;

    /* Precharge to Precharge Delay. (tCK) */
    const u32 tPPD = 4;

    /* DQS output access time from CK_t/CK_c. */
    const double tDQSCK_max = 3.5;
    /* Write preamble. (tCK) */
    const u32 tWPRE = 2;

    /* Read postamble. (tCK) */
    const double tRPST = 0.5;

    /* Minimum Self-Refresh Time. (Entry to Exit) */
    const double tSR = 15.0;

    /* Exit power down to next valid command delay. */
    const double tXP = 7.5;

    /* Write command to first DQS transition (max) (tCK) */
    const double tDQSS_max = 1.25;

    /* DQ-to-DQS offset(max) (ns) */
    const double tDQS2DQ_max = 0.8;

    /* Write recovery time. */
    const u32 tWR = 18;

    namespace pcv::erista {
        const std::array<u32,       8> tRCD_values    = { 18, 17, 16, 15, 14, 13, 12, 11 };
        const std::array<u32,       8> tRP_values     = { 18, 17, 16, 15, 14, 13, 12, 11 };
        const std::array<u32,      10> tRAS_values    = { 42, 36, 34, 32, 30, 28, 26, 24, 22, 20 };
        const std::array<double,    8>  tRRD_values   = { 10.0, 7.5, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0 };
        const std::array<u32,       6>  tRFC_values   = { 90, 80, 70, 60, 50, 40 };
        const std::array<u32,      10>  tWTR_values   = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
        const std::array<u32,       6>  tREFpb_values = { 3900, 5850, 7800, 11700, 15600, 99999 };

        const u32 tRCD    = tRCD_values[C.t1_tRCD];
        const u32 tRPpb   = tRP_values[C.t2_tRP];
        const u32 tRAS    = tRAS_values[C.t3_tRAS];
        const double tRRD = tRRD_values[C.t4_tRRD];
        const u32 tRFCpb  = tRFC_values[C.t5_tRFC];
        const u32 tWTR    = 10 - tWTR_values[C.t7_tWTR];
        const s32 finetRTW = C.fineTune_t6_tRTW;
        const s32 finetWTR = C.fineTune_t7_tWTR;

        const u32 tRC      = tRAS + tRPpb;
        const u32 tRFCab   = tRFCpb * 2;
        const double tXSR  = static_cast<double>(tRFCab + 7.5);
        const u32 tFAW     = static_cast<u32>(tRRD * 4.0);
        const double tRPab = tRPpb + 3;

        inline u32 RL;
        inline u32 WL;

        inline u32 tR2P;
        inline u32 tR2W;
        inline u32 rext;

        inline u32 tW2P;
        inline u32 tWTPDEN;
        inline u32 tW2R;

        inline u32 wdv;
        inline u32 wsv;
        inline u32 wev;

        inline u32 obdly;

        inline u32 pdex2rw;

        inline u32 tCLKSTOP;

        inline double pdex2mrr;

        inline u8 mrw2;
    }

    namespace pcv::mariko {
        const std::array<u32,       8> tRCD_values    = { 18, 17, 16, 15, 14, 13, 12, 11 };
        const std::array<u32,       8> tRP_values     = { 18, 17, 16, 15, 14, 13, 12, 11 };
        const std::array<u32,      10> tRAS_values    = { 42, 36, 34, 32, 30, 28, 26, 24, 22, 20 };
        const std::array<double,    7>  tRRD_values   = { /*10.0,*/ 7.5, 6.0, 5.0, 4.0, 3.0, 2.0, 1.0 }; /* 10.0 is used for <2133mhz; do we care? 8gb uses 7.5 tRRD on >=1331. */
        const std::array<u32,      11>  tRFC_values   = { 140, 130, 120, 110, 100, 90, 80, 70, 60, 50, 40 };
        const std::array<u32,      10>  tWTR_values   = { 10, 9, 8, 7, 6, 5, 4, 3, 2, 1 };
        const std::array<u32,       6>  tREFpb_values = { 3900, 5850, 7800, 11700, 15600, 99999 };

        inline u32 tRCD;
        inline u32 tRPpb;
        inline u32 tRAS;
        inline double tRRD;
        inline u32 tRFCpb;

        inline u32 tRC;
        inline u32 tRFCab;
        inline double tXSR;
        inline u32 tFAW;
        inline double tRPab;

        inline u32 refresh_raw;

        inline u32 RL;
        inline u32 WL;

        inline u32 tR2P;
        inline u32 tR2W;
        inline u32 tRTM;
        inline u32 tRATM;
        inline u32 rext;

        inline u32 rdv;
        inline u32 qpop;
        inline u32 quse_width;
        inline u32 quse;
        inline u32 einput_duration;
        inline u32 einput;
        inline u32 qrst;
        inline u32 ibdly;
        inline u32 qsafe;

        inline u32 tW2P;
        inline u32 tWTPDEN;
        inline u32 tW2R;
        inline u32 tWTM;
        inline u32 tWATM;

        inline u32 wdv;
        inline u32 wsv;
        inline u32 wev;

        inline u32 obdly;

        inline u32 pdex2rw;

        inline u32 tCLKSTOP;

        inline u32 pdex2mrr;

        inline u8 mrw2;
    }

}


