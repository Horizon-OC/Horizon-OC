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

#include "../../oc_common.hpp"
#include "../../mtc_timing_value.hpp"

namespace ams::ldr::hoc::pcv {

    namespace {
        void SwitchLatency(volatile u32 &latency, u32 index, u32 latencyStep) {
            latency += index * latencyStep;
        }

        void AutoLatency(volatile u32 &latency, u32 freq, u32 latencyStep) {
            if (freq > 1600'000 && freq <= 1866'000) { /* 1866tRWL */
                latency += latencyStep * 2;
            } else { /* 2133tRWL */
                latency += latencyStep * 3;
            }
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

        void HandleLatency(u32 freq, volatile u32 &latency, volatile u32 *latencyArray, u32 indexMax, u32 latencyStep) {
            for (u32 i = 0; i <= indexMax; ++i) {
                if (latencyArray[i] != 0 && freq <= latencyArray[i]) {
                    SwitchLatency(latency, i, latencyStep);
                    return;
                }
            }

            SwitchLatency(latency, indexMax, latencyStep);
        }

        u32 RoundUpToEven(u32 value) {
            return value + (value % 2);
        }


        double GetTckAvg(u32 khz) {
            return 1000'000.0 / static_cast<double>(khz);
        }
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

    u32 GetRlFrequency() {
        switch (RL) {
            case RL_1333:
                return 1333'000;
            case RL_1600:
                return 1600'000;
            case RL_1866:
                return 1866'000;
            case RL_2133:
                return 2133'000;
            default:
                return 2133'000;
        }
    }

    u32 GetWlFrequency() {
        switch (WL) {
            case WL_1333:
                return 1333'000;
            case WL_1600:
                return 1600'000;
            case WL_1866:
                return 1866'000;
            case WL_2133:
                return 2133'000;
            default:
                return 2133'000;
        }
    }

    u32 GetWdqsOn() {
        /* Set A. */
        switch (WL) {
            case WL_1333:
                return 4;
            case WL_1600:
            case WL_1866:
                return 6;
            case WL_2133:
                return 8;
            default:
                return 8;
        }
    }

    u32 CalculateNWr() {
        const u32 latencyFrequency = GetWlFrequency();
        const double tCK           = GetTckAvg(latencyFrequency);
        const u32 nWr              = CEIL(tWR / tCK);

        return RoundUpToEven(nWr);
    }

    u32 CalculateNTRTP() {
        const u32 latencyFrequency = GetRlFrequency();
        const double tCK           = GetTckAvg(latencyFrequency);

        return CEIL(tRTP / tCK);
    }

    u8 CalculateMrw2() {
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
        return static_cast<u8>(((rlIndex & 0x7) | ((wlIndex & 0x7) << 3) | ((0 & 0x1) << 6)));
    }
}
