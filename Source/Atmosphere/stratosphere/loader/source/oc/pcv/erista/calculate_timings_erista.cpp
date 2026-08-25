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

#include "../../mtc_timing_value.hpp"

namespace ams::ldr::hoc::pcv::erista {

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
            if (rlMapDBI[i] == 32) {
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
        CalculateMrw2();

        tR2P = CEIL((RL * 0.426) - 2.0);
        tR2W = FLOOR(FLOOR((5.0 / tCK_avg) + ((FLOOR(48.0 / WL) - 0.478) * 3.0)) / 1.501) + RL - (C.t6_tRTW * 3) + finetRTW;

        rdv  = RL + 19 + CEIL((flyByTime + tDQSCK_max) / tCK_avg);
        qpop = rdv - 16;

        quse_width        = CEIL(((4.897 / tCK_avg) - FLOOR(2.538 / tCK_avg)) + 3.782);
        quse              = FLOOR(RL + ((5.082 / tCK_avg) + FLOOR(2.560 / tCK_avg))) - CEIL(4.820 / tCK_avg);
        einput_duration   = FLOOR(9.936 / tCK_avg) + 5.0 + quse_width;
        einput            = quse - CEIL(9.928 / tCK_avg);
        u32 qrst_duration = FLOOR(8.399 - tCK_avg);
        u32 qrstLow       = MAX(static_cast<s32>(einput - qrst_duration - 2), static_cast<s32>(0));
        qrst              = PACK_U32(qrst_duration, qrstLow);
        ibdly             = PACK_U32_NIBBLE_HIGH_BYTE_LOW(1, quse - qrst_duration - 2.0);
        qsafe             = (einput_duration + 3) + MAX(MIN(qrstLow * rdv, qrst_duration + qrst_duration), einput);

        tW2P    = (CEIL(WL * 1.7303) * 2) - 5;
        tWTPDEN = CEIL(((1.803 / tCK_avg) + MAX(RL + (2.694 / tCK_avg), static_cast<double>(tW2P))) + (BL / 2));
        const s32 wlToRl = static_cast<s32>(WL) - static_cast<s32>(RL);
        tW2R    = FLOOR(MAX((5.020 / tCK_avg) + 1.130, WL - MAX(-CEIL(0.258 * wlToRl), 1.964)) * 1.964) + WL - CEIL(tWTR / tCK_avg) + finetWTR;

        wdv = WL;
        wsv = WL - 2;
        wev = 0xA + (WL - 14);

        u32 obdlyHigh = 3 / FLOOR(MIN(static_cast<double>(2), tCK_avg * (WL - 7)));
        u32 obdlyLow  = MAX(WL - FLOOR((126.0 / CEIL(tCK_avg + 8.601))), 0.0);
        obdly         = PACK_U32_NIBBLE_HIGH_BYTE_LOW(obdlyHigh, obdlyLow);

        pdex2rw  = CEIL((CEIL(12.335 - tCK_avg) + (7.430 / tCK_avg) - CEIL(tCK_avg * 11.361)));
        tCLKSTOP = FLOOR(MIN(8.488 / tCK_avg, 23.0)) + 8.0;

        const double tMMRI = tRCD + (tCK_avg * 3);
        pdex2mrr           = tMMRI + 10;
    }

}
