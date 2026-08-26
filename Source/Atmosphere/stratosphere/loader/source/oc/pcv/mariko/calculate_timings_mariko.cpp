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
            refresh_raw = CEIL(tREFpb_values[tREFI] / tCK_avg) - 0x40;
            refresh_raw = MIN(refresh_raw, static_cast<u32>(0xFFFF));
        }

        tRC    = tRAS + tRPpb;
        tRFCab = tRFCpb * 2;
        tXSR   = static_cast<double>(tRFCab + 7.5);
        tFAW   = static_cast<u32>(tRRD * 4.0);
        tRPab  = tRPpb + 3;

        tR2P  = CEIL((RL * 0.426) - 2.0);
        tR2W  = FLOOR(FLOOR((5.0 / tCK_avg) + ((FLOOR(48.0 / WL) - 0.478) * 3.0)) / 1.501) + RL - (tRTW * 3) + finetRTW;
        tRTM  = FLOOR((10.0 + RL) + (3.502 / tCK_avg)) + FLOOR(7.489 / tCK_avg);
        tRATM = CEIL((tRTM - 10.0) + (RL * 0.426));

        u32 fullRoundDelay = CEIL((flyByTime + tDQSCK_max) / tCK_avg);
        /* Read data valid: Read command -> data ready to be latched into the registers. */
        rdv                = RL + fullRoundDelay + 16;

        /* Read command -> data poppable at the pad macros, 14 cycles ahead of rdv. */
        qpop              = rdv - 14;

        constexpr double TrimmerStepPs   = 3.0;
        constexpr double MaxTrimmerSteps = 96.0;

        double tCkAvgPs       = tCK_avg * 1000.0;
        double tckDerated     = tCkAvgPs * 0.85;
        u32 derateStepCount   = MIN(FLOOR(tckDerated / TrimmerStepPs), MaxTrimmerSteps);
        double deratedNsDelay = (derateStepCount * TrimmerStepPs) / 1000.0;
        u32 fullTripDerating  = fullRoundDelay - CEIL((flyByTime + tDQSCK_max - deratedNsDelay) / tCK_avg);
        u32 readEyeStart      = FLOOR((flyByTime + 0.94) / tCK_avg) + RL;

        /* Data valid window, quse aligns with the start of the read eye (probably). */
        /* DQS needs to be logically and'ed with quse in order to be valid. */
        quse = readEyeStart + fullTripDerating - 2;

        quse_width        = CEIL(((4.897 / tCK_avg) - FLOOR(2.538 / tCK_avg)) + 3.782);
        quse              = FLOOR(RL + ((5.082 / tCK_avg) + FLOOR(2.560 / tCK_avg))) - CEIL(4.820 / tCK_avg);
        einput_duration   = FLOOR(9.936 / tCK_avg) + 5.0 + quse_width;
        einput            = quse - CEIL(9.928 / tCK_avg);
        u32 qrst_duration = FLOOR(8.399 - tCK_avg);
        u32 qrstLow       = MAX(static_cast<s32>(einput - qrst_duration - 2), static_cast<s32>(0));
        qrst              = PACK_U32(qrst_duration, qrstLow);
        ibdly             = PACK_U32_NIBBLE_HIGH_BYTE_LOW(1, quse - qrst_duration - 2.0);
        qsafe             = (einput_duration + 3) + MAX(MIN(qrstLow * rdv, qrst_duration + qrst_duration), einput);
        tW2P              = (CEIL(WL * 1.7303) * 2) - 5;
        tWTPDEN           = CEIL(((1.803 / tCK_avg) + MAX(RL + (2.694 / tCK_avg), static_cast<double>(tW2P))) + (BL / 2));
        const s32 wlToRl  = static_cast<s32>(WL) - static_cast<s32>(RL);
        tW2R              = FLOOR(MAX((5.020 / tCK_avg) + 1.130, WL - MAX(-CEIL(0.258 * wlToRl), 1.964)) * 1.964) + WL - CEIL(tWTR / tCK_avg) + finetWTR;
        tWTM              = CEIL(WL + ((7.570 / tCK_avg) + 8.753));
        tWATM             = (tWTM + (FLOOR(WL / 0.816) * 2.0)) - 4.0;

        wdv = WL;
        wsv = WL - 2;
        wev = 0xA + (WL - 14);

        u32 obdlyHigh = 3 / FLOOR(MIN(static_cast<double>(2), tCK_avg * (WL - 7)));
        u32 obdlyLow  = MAX(WL - FLOOR((126.0 / CEIL(tCK_avg + 8.601))), 0.0);
        obdly         = PACK_U32_NIBBLE_HIGH_BYTE_LOW(obdlyHigh, obdlyLow);

        pdex2rw  = CEIL((CEIL(12.335 - tCK_avg) + (7.430 / tCK_avg) - CEIL(tCK_avg * 11.361)));

        tCLKSTOP = FLOOR(MIN(8.488 / tCK_avg, 23.0)) + 8.0;

        u32 tMMRI = tRCD + (tCK_avg * 3);
        pdex2mrr  = tMMRI + 10;

        CalculateMrw2();
    }

}
