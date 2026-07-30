/*
 * Copyright (c) Lightos_
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

#include <stratosphere.hpp>
#include "../pcv.hpp"
#include "../../mtc_timing_value.hpp"
#include "timing_tables.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    void GetRext(u32 freq) {
        if (auto r = FindRext(freq)) {
            rext = r->rext;
            return;
        }

        /* Fallback */
        rext = 0x1E;
    }

    void SwitchLatency(volatile u32 &latency, u32 index, u32 latencyStep) {
        latency += index * latencyStep;
    }

    static s32 GetMaxLatencyIndex(volatile u32 *latencyArray, u32 latencySize) {
        s32 maxIndex = -1;
        for (u32 i = 0; i < latencySize; ++i) {
            if (latencyArray[i]) {
                maxIndex = i;
            }
        }

        return maxIndex;
    }

    void AutoLatency(volatile u32 &latency, u32 freq, u32 latencyStep) {
        if (freq > 1600'000 && freq <= 1866'000) { /* 1866tRWL */
            latency += latencyStep * 2;
        } else { /* 2133tRWL */
            // Note: JEDEC/Datasheet mandates 2133 for >1866, even if <2133
            latency += latencyStep * 3;
        }
    }

    void HandleLatency(u32 freq, volatile u32 &latency, volatile u32 *latencyArray, u32 indexMax, u32 latencyStep) {
        for (u32 i = 0; i <= indexMax; ++i) {
            if (latencyArray[i] != 0 && freq <= latencyArray[i]) {
                SwitchLatency(latency, i, latencyStep);
                return;
            }
        }

        SwitchLatency(latency, indexMax, latencyStep);
    }

    void HandleLatency(u32 freq) {
        static s32 rlIndexMax = GetMaxLatencyIndex(C.readLatency, std::size(C.readLatency));
        static s32 wlIndexMax = GetMaxLatencyIndex(C.writeLatency, std::size(C.writeLatency));
        constexpr u32 ReadLatencyStep  = 4;
        constexpr u32 WriteLatencyStep = 2;
        bool autoLatencyRead = false, autoLatencyWrite = false;

        if (rlIndexMax == -1) {
            AutoLatency(RL, freq, ReadLatencyStep);
            autoLatencyRead = true;
        }

        if (wlIndexMax == -1) {
            AutoLatency(WL, freq, WriteLatencyStep);
            autoLatencyWrite = true;
        }

        if (autoLatencyRead && autoLatencyWrite) {
            return;
        }

        if (!autoLatencyRead) {
            HandleLatency(freq, RL, C.readLatency, rlIndexMax, ReadLatencyStep);
        }

        if (!autoLatencyWrite) {
            HandleLatency(freq, WL, C.writeLatency, wlIndexMax, WriteLatencyStep);
        }
    }

    void CalculateMrw2() {
        static const u8 rlMapDBI[8] = {
            6, 12, 16, 22, 28, 32, 36, 40
        };

        static const u8 wlMapSetA[8] = {
            4, 6, 8, 10, 12, 14, 16, 18
        };

        u32 rlIndex = 0;
        u32 wlIndex = 0;

        for (u32 i = 0; i < std::size(rlMapDBI); ++i) {
            if (rlMapDBI[i] == RL) {
                rlIndex = i;
                break;
            }
        }

        for (u32 i = 0; i < std::size(wlMapSetA); ++i) {
            if (wlMapSetA[i] == WL) {
                wlIndex = i;
                break;
            }
        }

        /* DBI is always enabled. */
        mrw2 = static_cast<u8>(((rlIndex & 0x7) | ((wlIndex & 0x7) << 3) | ((0 & 0x1) << 6)));
    }

    void CalculateTimings(double tCK_avg, u32 freq) {
        RL = RL_1331;
        WL = WL_1331;

        HandleLatency(freq);

        GetRext(freq);

        /* At 1333WL, for some reason (incorrect ram timing config in mtc table?), tRP causes crashes at high reductions - 2 seems to be the most common limit. */
        /* This is a lazy workaround until I find the issue... */
        const bool lowFreq = freq < C.timingEmcTbreak;

        tRCD   = tRCD_values[lowFreq ? C.low_t1_tRCD : C.t1_tRCD];
        tRPpb  = tRP_values[lowFreq  ? C.low_t2_tRP  : C.t2_tRP];
        tRAS   = tRAS_values[lowFreq ? C.low_t3_tRAS : C.t3_tRAS];
        tRRD   = tRRD_values[lowFreq ? C.low_t4_tRRD : C.t4_tRRD];
        tRFCpb = tRFC_values[lowFreq ? C.low_t5_tRFC : C.t5_tRFC];

        u32 tRTW = lowFreq ? C.low_t6_tRTW : C.t6_tRTW;
        u32 tWTR = 10 - tWTR_values[lowFreq ? C.low_t7_tWTR : C.t7_tWTR];

        s32 finetRTW = C.fineTune_t6_tRTW;
        s32 finetWTR = C.fineTune_t7_tWTR;

        u32 tREFI = lowFreq ? C.low_t8_tREFI : C.t8_tREFI;
        refresh_raw = 0xFFFF;
        if (tREFI != 6) {
            refresh_raw = ROUND(tREFpb_values[tREFI] / tCK_avg) - 0x40;
            refresh_raw = MIN(refresh_raw, static_cast<u32>(0xFFFF));
        }

        tRC    = tRAS + tRPpb;
        tRFCab = tRFCpb * 2;
        tXSR   = static_cast<double>(tRFCab + 7.5);
        tFAW   = static_cast<u32>(tRRD * 4.0);
        tRPab  = tRPpb + 3;

        tR2P  = CEIL((RL * 0.426) - 2.0);
        tR2W  = RL + 2 * (FLOOR(48.0 / WL) - 2) + CEIL((5.036 / tCK_avg) + 0.418) - FLOOR((1.476 / tCK_avg) + 0.05) - (tRTW * 3) + finetRTW;
        tRTM  = FLOOR((10.0 + RL) + (3.496 / tCK_avg)) + FLOOR(7.430 / tCK_avg);
        tRATM = CEIL((tRTM - 10.0) + (RL * 0.426));

        rdv               = RL + FLOOR((5.109 / tCK_avg) + 17.003);
        qpop              = rdv - 14;
        u32 quseSum       = RL + FLOOR(5.110 / tCK_avg) + 4;
        quse_width        = CEIL(MAX((4.7980 / tCK_avg) + 4.046, (5.0680 / tCK_avg) + 3.276)) - FLOOR(2.55 / tCK_avg);
        quse              = quseSum - quse_width;
        einput_duration   = FLOOR(9.998 / tCK_avg) + 5.0 + quse_width;
        einput            = quse - (FLOOR(9.998 / tCK_avg) + 1);
        u32 qrst_duration = FLOOR((1.398 / tCK_avg) + 4.5);
        u32 qrstLow       = MAX(static_cast<s32>(einput - qrst_duration - 2), static_cast<s32>(0));
        qrst              = PACK_U32(qrst_duration, qrstLow);
        ibdly             = PACK_U32_NIBBLE_HIGH_BYTE_LOW(1, quse - qrst_duration - 2.0);
        qsafe             = (einput_duration + 3) + MAX(MIN(qrstLow * rdv, qrst_duration + qrst_duration), einput);
        tW2P              = (CEIL(WL * 1.7303) * 2) - 5;
        tWTPDEN           = CEIL(((1.800 / tCK_avg) + MAX(RL + (2.550 / tCK_avg), static_cast<double>(tW2P))) + (BL / 2));
        tW2R              = FLOOR(MAX((5.087 / tCK_avg) + 1.030, WL - MAX(-CEIL(0.258 * (WL - RL)), 1.964)) * 1.964) + WL - CEIL(tWTR / tCK_avg) + finetWTR;
        tWTM              = CEIL(WL + ((7.386 / tCK_avg) + 9.150));
        tWATM             = (tWTM + (FLOOR(WL / 0.816) * 2.0)) - 4.0;

        wdv = WL;
        wsv = WL - 2;
        wev = 0xA + (WL - 14);

        u32 obdlyHigh = 3 / FLOOR(MIN(static_cast<double>(2), tCK_avg * (WL - 7)));
        u32 obdlyLow  = MAX(WL - FLOOR((126.0 / CEIL(tCK_avg + 8.601))), 0.0);
        obdly         = PACK_U32_NIBBLE_HIGH_BYTE_LOW(obdlyHigh, obdlyLow);

        pdex2rw  = CEIL((7.5 / tCK_avg) + 0.998) + FLOOR(1.75 / tCK_avg) + FLOOR(1.0 / tCK_avg);

        tCLKSTOP = FLOOR(MIN(8.4996 / tCK_avg, 23.0)) + 7.0 + MIN(CEIL(1.75 / tCK_avg) - 3.0, 1.0);

        u32 tMMRI = tRCD + (tCK_avg * 3);
        pdex2mrr  = tMMRI + 10;

        CalculateMrw2();
    }

}
