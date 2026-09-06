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
#include "../common/calculate_timings_common.hpp"

namespace ams::ldr::hoc::pcv::erista {

    namespace {
        /* Dumped from bpmp fw mtc table (l4t). */
        const RextPatch gRextTable[] = {
            {1'633'000, 0x17}, {1'728'000, 0x17}, {1'795'200, 0x19},
            {1'862'400, 0x1A}, {1'894'400, 0x1A}, {1'932'800, 0x1A},
            {1'958'400, 0x1A}, {1'996'800, 0x1A}, {2'035'200, 0x1A},
            {2'064'000, 0x19}, {2'099'200, 0x19}, {2'131'200, 0x1A},
            {2'163'200, 0x1A}, {2'188'800, 0x1A}, {2'227'200, 0x1A},
            {2'265'600, 0x1B}, {2'291'200, 0x1B}, {2'329'600, 0x1A},
            {2'361'600, 0x1A},
        };

        const RextPatch *LookupRext(u32 freq) {
            for (u32 i = 0; i < std::size(gRextTable); i++) {
                if (gRextTable[i].freq >= freq) {
                    return &gRextTable[i];
                }
            }
            return nullptr;
        }

        void GetRext(u32 freq) {
            if (auto r = LookupRext(freq)) {
                rext = r->rext;
                return;
            }

            /* Fallback */
            rext = 0x1A;
        }

        void CalculateReadTimings(u32 freq, double tCK_avg) {
            tR2P = CalculateNTRTP();

            double fullRoundDelay = flyByTime + tDQSCK_max;

            /* Read to write turnaround. */
            /* Nvidia seem to use a 0.25 constant here for tK1, I am 99% certain this is tDQSTA. */
            /* This still left a mismatch; after some brute forcing I got GET_CYCLE_FLOOR(1.5) - 8 (using a static latency) as the cleanest result. */
            /* The latency definitely is not WL, but when testing for every latency, it lines up perfectly with wqsOn and wl seems to cancel out? */
            tR2W = RL + (BL / 2) + CEIL((fullRoundDelay / tCK_avg) + tDQSTA) - GET_CYCLE_FLOOR(1.5) - GetWdqsOn() + tWPRE - (C.t6_tRTW * 3) + finetRTW;

            /* Read data valid: Read command -> data ready to be latched into the registers. */
            rdv  = RL + GET_CYCLE_CEIL(fullRoundDelay) + 18;

            /* Read command -> data poppable at the pad macros, 16 cycles ahead of rdv. */
            qpop = rdv - 16;

            constexpr double TrimmerStepPs   = 3.0;
            constexpr double MaxTrimmerSteps = 95.0;

            double tCkAvgPs       = tCK_avg * 1000.0;
            double tckDerated     = tCkAvgPs * 0.85;
            u32 derateStepCount   = MIN(FLOOR(tckDerated / TrimmerStepPs), MaxTrimmerSteps);
            double deratedNsDelay = (derateStepCount * TrimmerStepPs) / 1000.0;

            /* readEyeEndDerated, nv call this quseExtra. */
            u32 quseExtra = GET_CYCLE_CEIL(fullRoundDelay - deratedNsDelay);

            /* The amount of deration from the perspective of a full round trip. */
            u32 readEyeEndDerateMargin = GET_CYCLE_CEIL(fullRoundDelay) - quseExtra;

            u32 tQuse        = FLOOR((flyByTime + 0.94) / tCK_avg);
            u32 readEyeStart = tQuse + RL + 1;

            u32 einputMinimal = readEyeStart + readEyeEndDerateMargin - 2;

            /* Data valid window, quse aligns with the start of the read eye (probably). */
            /* DQS needs to be logically and'ed with quse in order to be valid. */
            /* einputMinimal satisfies this. */
            quse = einputMinimal;

            /* Difference between the two eye edges. */
            quse_width = quseExtra - tQuse + 4;

            /* input receiver takes time to enable. */
            u32 einputLatency = GET_CYCLE_CEIL(10.0);

            /* Enable input receiver. */
            einput = einputMinimal - einputLatency;

            /* Duration of input receiver being enabled? */
            einput_duration = einputLatency + quse_width + 4;

            /* qrst_duration is CEIL(a / tCK_avg + b) */
            /* a and b were derived through a brute forcer. */
            u32 qrst_duration = CEIL(1.4 / tCK_avg + 3.5);
            u32 qrstLow       = MAX(static_cast<s32>(einput - qrst_duration - 2), static_cast<s32>(0));

            /* quse reset? */
            qrst = PACK_U32(qrst_duration, qrstLow);

            /* Qsafe, safe to reset quse? */
            qsafe = (einput_duration + 3) + MAX(MIN(qrstLow * rdv, qrst_duration + qrst_duration), einput);

            /* Input buffer delay? */
            ibdly = PACK_U32_NIBBLE_HIGH_BYTE_LOW(1, quse - qrst_duration - 2.0);

            GetRext(freq);
        }

        void CalculateWriteTimings(double tCK_avg) {
            /* Write to read turnaround. */
            /* According to jedec specifications: nWR >= tWTR + nRTP */
            /* Therefore tWTR >= nWR - tRTP. */
            const u32 nTWTR          = GET_CYCLE_CEIL(tWTRStock);
            const u32 nWr            = CalculateNWr();
            u32 autoPrecharcetWTRMin = nWr - tR2P;

            /* tWTR reduction is applied in a weird way to not break existing setups. */
            tW2R    = WL + 1 + MAX(autoPrecharcetWTRMin, nTWTR) - GET_CYCLE_CEIL(tWTR) + finetWTR;
            tW2P    = (CEIL(WL * 1.7303) * 2) - 5;
            tWTPDEN = CEIL(((1.8 / tCK_avg) + MAX(RL + (2.7 / tCK_avg), static_cast<double>(tW2P))) + (BL / 2));

            wdv = WL;
            wsv = WL - 2;
            wev = WL - 4;

            /* Output buffer delay? */
            u32 obdlyHigh = 3 / FLOOR(MIN(static_cast<double>(2), tCK_avg * (WL - 7)));
            u32 obdlyLow  = MAX(WL - FLOOR((126.0 / CEIL(tCK_avg + 8.601))), 0.0);
            obdly         = PACK_U32_NIBBLE_HIGH_BYTE_LOW(obdlyHigh, obdlyLow);
        }

        void CalculatePowerDownTimings(double tCK_avg) {
            pdex2rw  = CEIL((CEIL(12.335 - tCK_avg) + (7.430 / tCK_avg) - CEIL(tCK_avg * 11.361)));
            tCLKSTOP = FLOOR(MIN(8.488 / tCK_avg, 23.0)) + 8.0;

            const double tMMRI = tRCD + (tCK_avg * 3);
            pdex2mrr           = tMMRI + 10;
        }
    }

    u8 GetNWrIndex() {
        static const u8 nWrMap[8] = {
            6, 10, 16, 20, 24, 30, 34, 40
        };

        const u32 nWr = CalculateNWr();

        for (u8 i = 0; i < std::size(nWrMap); ++i) {
            if (nWrMap[i] >= nWr) {
                return i;
            }
        }

        return static_cast<u8>(std::size(nWrMap) - 1);
    }

    void CalculateTimings(double tCK_avg, u32 freq) {
        RL = RL_1333;
        WL = WL_1333;

        /* I don't understand why, I don't want to know why, but for some fucking reason erista doesn't handle RL 40. */
        /* This should not be required but for these fuck damn reasons any type of dram oc will crash instantly with RL 40. */
        /* But it works fine in l4t? Even when copying the tables it still doesn't work??? WTF */
        readLatency[std::size(readLatency) - 1] = 0;
        HandleLatency(freq);
        RL = std::min(static_cast<u32>(RL_1866), RL);

        CalculateReadTimings(freq, tCK_avg);
        CalculateWriteTimings(tCK_avg);
        CalculatePowerDownTimings(tCK_avg);
        mrw2 = CalculateMrw2();
    }

}
