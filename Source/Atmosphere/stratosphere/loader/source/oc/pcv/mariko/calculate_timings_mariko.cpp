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

namespace ams::ldr::hoc::pcv::mariko {

    namespace {
        const RextPatch gRextTable[] = {
            {1'633'000, 0x17}, {1'666'000, 0x18}, {1'700'000, 0x18},
            {1'733'000, 0x18}, {1'766'000, 0x18}, {1'800'000, 0x1A},
            {1'833'000, 0x1A}, {1'866'000, 0x1A}, {1'900'000, 0x19},
            {1'933'000, 0x19}, {1'966'000, 0x19}, {2'100'000, 0x1A},
            {2'133'000, 0x1A}, {2'166'000, 0x19}, {2'200'000, 0x19},
            {2'233'000, 0x19}, {2'266'000, 0x1A}, {2'300'000, 0x1B},
            {2'333'000, 0x1B}, {2'366'000, 0x1B}, {2'400'000, 0x1B},
            {2'433'000, 0x1B}, {2'466'000, 0x1B}, {2'500'000, 0x1A},
            {2'533'000, 0x1C}, {2'566'000, 0x1B}, {2'600'000, 0x1B},
            {2'633'000, 0x1B}, {2'666'000, 0x1B}, {2'700'000, 0x1C},
            {2'733'000, 0x1C}, {2'766'000, 0x1D}, {2'800'000, 0x1D},
            {2'833'000, 0x1D}, {2'866'000, 0x1D}, {2'900'000, 0x1D},
            {2'933'000, 0x1C}, {2'966'000, 0x1D}, {3'000'000, 0x1D},
            {3'033'000, 0x1D}, {3'066'000, 0x1D}, {3'100'000, 0x1D},
            {3'133'000, 0x1D}, {3'166'000, 0x1C}, {3'200'000, 0x1C},
            {3'233'000, 0x1E}, {3'266'000, 0x1F}, {3'300'000, 0x1E},
            {3'333'000, 0x1F}, {3'366'000, 0x1F}, {3'400'000, 0x1F},
            {3'433'000, 0x1F}, {3'466'000, 0x1F}, {3'500'000, 0x1E},
            {3'533'000, 0x1E}, {3'566'000, 0x1E}, {3'600'000, 0x1E},
            {3'633'000, 0x1E}, {3'666'000, 0x1E}, {3'700'000, 0x1F},
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
            rext = 0x1E;
        }

        void CalculateActivatePrechargeTimings(bool lowFreq) {
            tRCD  = tRCD_values[lowFreq ? C.low_t1_tRCD : C.t1_tRCD];
            tRPpb = tRP_values[lowFreq  ? C.low_t2_tRP  : C.t2_tRP];
            tRAS  = tRAS_values[lowFreq ? C.low_t3_tRAS : C.t3_tRAS];

            tRC   = tRAS  + tRPpb;
            tRPab = tRPpb + 3;
        }

        void CalculateActivateWindowTimings(bool lowFreq) {
            tRRD = tRRD_values[lowFreq ? C.low_t4_tRRD : C.t4_tRRD];
            tFAW = static_cast<u32>(tRRD * 4.0);
        }

        void CalculateReadTimings(u32 freq, double tCK_avg, bool lowFreq) {
            u32 tRTW     = lowFreq ? C.low_t6_tRTW : C.t6_tRTW;
            s32 finetRTW = C.fineTune_t6_tRTW;

            double fullRoundDelay = flyByTime + tDQSCK_max;

            /* Read to precharge delay. */
            tR2P  = CalculateNTRTP();

            /* Read to write turnaround. */
            /* Nvidia seem to use a 0.25 constant here for tK1, I am 99% certain this is tDQSTA. */
            /* This still left a mismatch; after some brute forcing I got GET_CYCLE_FLOOR(1.5) - 8 (using a static latency) as the cleanest result. */
            /* The latency definitely is not WL, but when testing for every latency, it lines up perfectly with wqsOn and wl seems to cancel out? */
            tR2W  = RL + (BL / 2) + CEIL((fullRoundDelay / tCK_avg) + tDQSTA) - GET_CYCLE_FLOOR(1.5) - GetWdqsOn() - (tRTW * 3) + finetRTW;
            tRTM  = FLOOR((10.0 + RL) + (3.502 / tCK_avg)) + GET_CYCLE_FLOOR(7.489);
            tRATM = CEIL((tRTM - 10.0) + (RL * 0.426));

            /* Read data valid: Read command -> data ready to be latched into the registers. */
            rdv = RL + GET_CYCLE_CEIL(fullRoundDelay) + 16;

            /* Read command -> data poppable at the pad macros, 14 cycles ahead of rdv. */
            qpop = rdv - 14;

            constexpr double TrimmerStepPs   = 3.0;
            constexpr double MaxTrimmerSteps = 96.0;

            double tCkAvgPs            = tCK_avg * 1000.0;
            double tckDerated          = tCkAvgPs * 0.85;
            u32 derateStepCount        = MIN(FLOOR(tckDerated / TrimmerStepPs), MaxTrimmerSteps);
            double deratedNsDelay      = (derateStepCount * TrimmerStepPs) / 1000.0;
            /* readEyeEndDerated, nv call this quseExtra. */
            u32 quseExtra              = GET_CYCLE_CEIL(fullRoundDelay - deratedNsDelay);

            /* The amount of deration from the perspective of a full round trip. */
            u32 readEyeEndDerateMargin = GET_CYCLE_CEIL(fullRoundDelay) - quseExtra;

            u32 tQuse        = FLOOR((flyByTime + 0.94) / tCK_avg);
            u32 readEyeStart = tQuse + RL + 1;

            u32 einputMinimal = readEyeStart + readEyeEndDerateMargin - 2;

            /* Data valid window, quse aligns with the start of the read eye (probably). */
            /* DQS needs to be logically and'ed with quse in order to be valid. */
            /* einputMinimal satisfies this. */
            quse = einputMinimal;

            /* input receiver takes time to enable. */
            u32 einputLatency = GET_CYCLE_CEIL(10.0);

            /* Enable input receiver. */
            einput = einputMinimal - einputLatency;

            /* Difference between the two eye edges. */
            quse_width = quseExtra - tQuse + 4;

            /* Duration of input receiver being enabled? */
            einput_duration = einputLatency + quse_width + 4;

            /* qrst_duration is CEIL(a / tCK_avg + b) */
            /* a and b were derived through a brute forcer. */
            u32 qrst_duration = CEIL(1.4 / tCK_avg + 3.5);
            u32 qrstLow       = MAX(static_cast<s32>(einput - qrst_duration - 2), static_cast<s32>(0));

            /* quse reset? */
            qrst = PACK_U32(qrst_duration, qrstLow);

            /* Input buffer delay? */
            ibdly = PACK_U32_NIBBLE_HIGH_BYTE_LOW(1, quse - qrst_duration - 2.0);

            /* Qsafe, safe to reset quse? */
            qsafe = (einput_duration + 3) + MAX(MIN(qrstLow * rdv, qrst_duration + qrst_duration), einput);

            GetRext(freq);
        }

        void CalculateWriteTimings(double tCK_avg, bool lowFreq) {
            u32 tWTR     = 10 - tWTR_values[lowFreq ? C.low_t7_tWTR : C.t7_tWTR];
            s32 finetWTR = C.fineTune_t7_tWTR;

            /* Write to read turnaround. */
            /* According to jedec specifications: nWR >= tWTR + nRTP */
            /* Therefore tWTR >= nWR - tRTP. */
            const u32 nTWTR          = GET_CYCLE_CEIL(tWTRStock);
            const u32 nWr            = CalculateNWr();
            u32 autoPrecharcetWTRMin = nWr - tR2P;

            /* tWTR reduction is applied in a weird way to not break existing setups. */
            tW2R  = WL + 1 + MAX(autoPrecharcetWTRMin, nTWTR) - GET_CYCLE_CEIL(tWTR) + finetWTR;
            tW2P  = (CEIL(WL * 1.7303) * 2) - 5;
            tWTM  = CEIL(WL + ((7.570 / tCK_avg) + 8.753));
            tWATM = (tWTM + (FLOOR(WL / 0.816) * 2.0)) - 4.0;

            wdv = WL;
            wsv = WL - 2;
            wev = WL - 4;

            /* Output buffer delay? */
            u32 obdlyHigh = 3 / FLOOR(MIN(static_cast<double>(2), tCK_avg * (WL - 7)));
            u32 obdlyLow  = MAX(WL - FLOOR((126.0 / CEIL(tCK_avg + 8.601))), 0.0);
            obdly         = PACK_U32_NIBBLE_HIGH_BYTE_LOW(obdlyHigh, obdlyLow);
        }

        void CalculateRefreshTimings(double tCK_avg, bool lowFreq) {
            tRFCpb = tRFC_values[lowFreq ? C.low_t5_tRFC : C.t5_tRFC];
            tRFCab = tRFCpb * 2;
            tXSR = static_cast<double>(tRFCab + 7.5);

            u32 tREFI  = lowFreq ? C.low_t8_tREFI : C.t8_tREFI;
            refresh_raw = 0xFFFF;
            if (tREFI != 6) {
                refresh_raw = GET_CYCLE_CEIL(tREFpb_values[tREFI]) - 0x40;
                refresh_raw = MIN(refresh_raw, static_cast<u32>(0xFFFF));
            }
        }

        void CalculatePowerDownTimings(double tCK_avg) {
            tWTPDEN = CEIL(((1.8 / tCK_avg) + MAX(RL + (2.7 / tCK_avg), static_cast<double>(tW2P))) + (BL / 2));
            pdex2rw = CEIL((CEIL(12.335 - tCK_avg) + (7.430 / tCK_avg) - CEIL(tCK_avg * 11.361)));

            tCLKSTOP = FLOOR(MIN(8.488 / tCK_avg, 23.0)) + 8.0;

            double tMMRI = tRCD + (tCK_avg * 3);
            pdex2mrr     = tMMRI + 10;
        }
    }

    void CalculateTimings(double tCK_avg, u32 freq) {
        RL = RL_1333;
        WL = WL_1333;

        HandleLatency(freq);

        const bool lowFreq = freq < C.timingEmcTbreak;

        CalculateActivatePrechargeTimings(lowFreq);
        CalculateActivateWindowTimings(lowFreq);
        CalculateReadTimings(freq, tCK_avg, lowFreq);
        CalculateWriteTimings(tCK_avg, lowFreq);
        CalculateRefreshTimings(tCK_avg, lowFreq);
        CalculatePowerDownTimings(tCK_avg);
        mrw2 = CalculateMrw2();
    }

}
