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

#include <vector>
#include "../pcv.hpp"
#include "../../mtc_timing_value.hpp"
#include "calculate_timings_erista.hpp"

namespace ams::ldr::hoc::pcv::erista {

    namespace {
        std::vector<u32> newEmcList;

        struct {
            u32            *getEristaMtcTableFnSite = nullptr;
            EristaMtcTable *mtcTable                = nullptr;
            bool            foundMtcTablePattern    = false;
        } getMtcTableCache;
    }

    /* Note: This does not have proper timings, so base latency adjustment will not work.             */
    /* However, it may still achieve a slightly higher frequency, but not as much as it could be.     */
    /* I'm certainly not insane enough to attempt this pain again, so this will have to do *for now*. */
    void MemMtcTableAutoAdjust(EristaMtcTable *table) {
        const double tCK_avg = 1000'000.0 / table->rate_khz;

        #define WRITE_PARAM_ALL_REG(TABLE, PARAM, VALUE) \
            TABLE->burst_regs.PARAM = VALUE;             \
            TABLE->shadow_regs_ca_train.PARAM   = VALUE; \
            TABLE->shadow_regs_rdwr_train.PARAM = VALUE;

        #define GET_CYCLE_CEIL(PARAM) u32(CEIL(double(PARAM) / tCK_avg))

        /* Ram power down       */
        /* B31: DRAM_CLKSTOP_PD */
        /* B30: DRAM_CLKSTOP_SR */
        /* B29: DRAM_ACPD       */
        if (C.hpMode) {
            WRITE_PARAM_ALL_REG(table, emc_cfg, 0x13200000);
        } else {
            WRITE_PARAM_ALL_REG(table, emc_cfg, 0xF3200000);
        }

        u32 refresh_raw = 0xFFFF;
        if (C.t8_tREFI != 6) {
            refresh_raw = CEIL(tREFpb_values[C.t8_tREFI] / tCK_avg) - 0x40;
            refresh_raw = MIN(refresh_raw, static_cast<u32>(0xFFFF));
        }

        if (table->rate_khz > 3200000) {
            rext = 30;
        } else if (table->rate_khz >= 2133001) {
            rext = 28;
        } else {
            rext = 26;
        }

        u32 trefbw = refresh_raw + 0x40;
        trefbw     = MIN(trefbw, static_cast<u32>(0x3FFF));

        const u32 dyn_self_ref_control = (static_cast<u32>(7605.0 / tCK_avg) + 260) | (table->burst_regs.emc_dyn_self_ref_control & 0xffff0000);

        CalculateTimings(tCK_avg, table->rate_khz);

        WRITE_PARAM_ALL_REG(table, emc_rd_rcd, GET_CYCLE_CEIL(tRCD));
        WRITE_PARAM_ALL_REG(table, emc_wr_rcd, GET_CYCLE_CEIL(tRCD));
        WRITE_PARAM_ALL_REG(table, emc_rc, MIN(GET_CYCLE_CEIL(tRC), static_cast<u32>(0xB9)));
        WRITE_PARAM_ALL_REG(table, emc_ras, MIN(GET_CYCLE_CEIL(tRAS), static_cast<u32>(0x7F)));
        WRITE_PARAM_ALL_REG(table, emc_rrd, GET_CYCLE_CEIL(tRRD));
        WRITE_PARAM_ALL_REG(table, emc_rfcpb, GET_CYCLE_CEIL(tRFCpb));
        WRITE_PARAM_ALL_REG(table, emc_rfc, GET_CYCLE_CEIL(tRFCab));
        WRITE_PARAM_ALL_REG(table, emc_rp, GET_CYCLE_CEIL(tRPpb));
        WRITE_PARAM_ALL_REG(table, emc_txsr, MIN(GET_CYCLE_CEIL(tXSR), static_cast<u32>(0x3fe)));
        WRITE_PARAM_ALL_REG(table, emc_txsrdll, MIN(GET_CYCLE_CEIL(tXSR), static_cast<u32>(0x3fe)));
        WRITE_PARAM_ALL_REG(table, emc_tfaw, GET_CYCLE_CEIL(tFAW));
        WRITE_PARAM_ALL_REG(table, emc_trpab, MIN(GET_CYCLE_CEIL(tRPab), static_cast<u32>(0x3F)));
        WRITE_PARAM_ALL_REG(table, emc_tckesr, GET_CYCLE_CEIL(tSR));
        WRITE_PARAM_ALL_REG(table, emc_tcke, GET_CYCLE_CEIL(tXP) + MIN(GET_CYCLE_CEIL(1.75) - 2, 2u));
        WRITE_PARAM_ALL_REG(table, emc_tpd, GET_CYCLE_CEIL(tXP));
        WRITE_PARAM_ALL_REG(table, emc_tclkstop, tCLKSTOP);
        WRITE_PARAM_ALL_REG(table, emc_r2p, tR2P);
        WRITE_PARAM_ALL_REG(table, emc_r2w, tR2W);
        WRITE_PARAM_ALL_REG(table, emc_w2p, tW2P);
        WRITE_PARAM_ALL_REG(table, emc_w2r, tW2R);
        WRITE_PARAM_ALL_REG(table, emc_rext, rext);
        WRITE_PARAM_ALL_REG(table, emc_wext, (table->rate_khz >= 2533000) ? 0x19 : 0x16);
        WRITE_PARAM_ALL_REG(table, emc_refresh, refresh_raw);
        WRITE_PARAM_ALL_REG(table, emc_pre_refresh_req_cnt, refresh_raw / 4);
        WRITE_PARAM_ALL_REG(table, emc_trefbw, trefbw);
        WRITE_PARAM_ALL_REG(table, emc_dyn_self_ref_control, dyn_self_ref_control);
        WRITE_PARAM_ALL_REG(table, emc_pdex2wr, pdex2rw);
        WRITE_PARAM_ALL_REG(table, emc_pdex2rd, pdex2rw);
        WRITE_PARAM_ALL_REG(table, emc_pchg2pden, GET_CYCLE_CEIL(1.75));
        WRITE_PARAM_ALL_REG(table, emc_ar2pden, GET_CYCLE_CEIL(1.75));
        WRITE_PARAM_ALL_REG(table, emc_pdex2cke, MAX(GET_CYCLE_CEIL(1.75) - 2, 2u));
        WRITE_PARAM_ALL_REG(table, emc_act2pden, GET_CYCLE_CEIL(14.0));
        WRITE_PARAM_ALL_REG(table, emc_cke2pden, GET_CYCLE_CEIL(8.5));
        WRITE_PARAM_ALL_REG(table, emc_pdex2mrr, GET_CYCLE_CEIL(pdex2mrr));
        WRITE_PARAM_ALL_REG(table, emc_rw2pden, tWTPDEN);

        /* Accept imperfection or prepare for suffering. */
        // WRITE_PARAM_ALL_REG(table, emc_einput, einput);
        // WRITE_PARAM_ALL_REG(table, emc_einput_duration, einput_duration);
        WRITE_PARAM_ALL_REG(table, emc_obdly, obdly);
        // WRITE_PARAM_ALL_REG(table, emc_ibdly, ibdly);
        // WRITE_PARAM_ALL_REG(table, emc_wdv_mask, wdv);
        // WRITE_PARAM_ALL_REG(table, emc_quse_width, quse_width);
        // WRITE_PARAM_ALL_REG(table, emc_quse, quse);
        WRITE_PARAM_ALL_REG(table, emc_wdv, wdv);
        WRITE_PARAM_ALL_REG(table, emc_wsv, wsv);
        WRITE_PARAM_ALL_REG(table, emc_wev, wev);
        // WRITE_PARAM_ALL_REG(table, emc_qrst, qrst);
        // WRITE_PARAM_ALL_REG(table, emc_tr_qrst, qrst);
        // WRITE_PARAM_ALL_REG(table, emc_qsafe, qsafe);
        // WRITE_PARAM_ALL_REG(table, emc_tr_qsafe, qsafe);
        WRITE_PARAM_ALL_REG(table, emc_tr_qpop, qpop);
        WRITE_PARAM_ALL_REG(table, emc_qpop, qpop);
        WRITE_PARAM_ALL_REG(table, emc_rdv, rdv);
        WRITE_PARAM_ALL_REG(table, emc_tr_rdv_mask, rdv + 2);
        WRITE_PARAM_ALL_REG(table, emc_rdv_early, rdv - 2);
        WRITE_PARAM_ALL_REG(table, emc_rdv_early_mask, rdv);
        WRITE_PARAM_ALL_REG(table, emc_rdv_mask, rdv + 2);
        WRITE_PARAM_ALL_REG(table, emc_tr_rdv, rdv);
        table->emc_mrw2 = (table->emc_mrw2 & ~0xFFu) | static_cast<u32>(mrw2);

        constexpr double MC_ARB_DIV = 4.0;
        constexpr u32 MC_ARB_SFA    = 2;

        table->burst_mc_regs.mc_emem_arb_cfg            = table->rate_khz             / (33.3 * 1000) / MC_ARB_DIV;
        table->burst_mc_regs.mc_emem_arb_timing_rcd     = CEIL(GET_CYCLE_CEIL(tRCD)   / MC_ARB_DIV) - 2;
        table->burst_mc_regs.mc_emem_arb_timing_rp      = CEIL(GET_CYCLE_CEIL(tRPpb)  / MC_ARB_DIV) - 1;
        table->burst_mc_regs.mc_emem_arb_timing_rc      = CEIL(GET_CYCLE_CEIL(tRC)    / MC_ARB_DIV) - 1;
        table->burst_mc_regs.mc_emem_arb_timing_ras     = CEIL(GET_CYCLE_CEIL(tRAS)   / MC_ARB_DIV) - 2;
        table->burst_mc_regs.mc_emem_arb_timing_faw     = CEIL(GET_CYCLE_CEIL(tFAW)   / MC_ARB_DIV) - 1;
        table->burst_mc_regs.mc_emem_arb_timing_rrd     = CEIL(GET_CYCLE_CEIL(tRRD)   / MC_ARB_DIV) - 1;
        table->burst_mc_regs.mc_emem_arb_timing_rfcpb   = CEIL(GET_CYCLE_CEIL(tRFCpb) / MC_ARB_DIV) - 1;
        table->burst_mc_regs.mc_emem_arb_timing_rap2pre = CEIL(tR2P / MC_ARB_DIV);
        table->burst_mc_regs.mc_emem_arb_timing_wap2pre = CEIL(tW2P / MC_ARB_DIV) + MC_ARB_SFA;

        if (table->burst_mc_regs.mc_emem_arb_timing_r2r > 1) {
            table->burst_mc_regs.mc_emem_arb_timing_r2r = CEIL(table->burst_regs.emc_rext / 4) - 1 + MC_ARB_SFA;
        }

        table->burst_mc_regs.mc_emem_arb_timing_r2w = CEIL(tR2W / MC_ARB_DIV) - 1 + MC_ARB_SFA;
        table->burst_mc_regs.mc_emem_arb_timing_w2r = CEIL(tW2R / MC_ARB_DIV) - 1 + MC_ARB_SFA;

        u32 da_turns = 0;
        da_turns |= u8(table->burst_mc_regs.mc_emem_arb_timing_r2w / 2) << 16;
        da_turns |= u8(table->burst_mc_regs.mc_emem_arb_timing_w2r / 2) << 24;
        table->burst_mc_regs.mc_emem_arb_da_turns = da_turns;

        u32 da_covers = 0;
        u8 r_cover = (table->burst_mc_regs.mc_emem_arb_timing_rap2pre + table->burst_mc_regs.mc_emem_arb_timing_rp + table->burst_mc_regs.mc_emem_arb_timing_rcd) / 2;
        u8 w_cover = (table->burst_mc_regs.mc_emem_arb_timing_wap2pre + table->burst_mc_regs.mc_emem_arb_timing_rp + table->burst_mc_regs.mc_emem_arb_timing_rcd) / 2;
        da_covers |= (table->burst_mc_regs.mc_emem_arb_timing_rc / 2);
        da_covers |= (r_cover << 8);
        da_covers |= (w_cover << 16);
        table->burst_mc_regs.mc_emem_arb_da_covers = da_covers;

        constexpr u32 AtomsPerDvfsPulse             = 0x7;
        constexpr u32 McEmcSameFreq                 = 0x0;
        constexpr u32 ExpiringSoonSlackThreshold    = 0xC;
        const     u32 priorityInversionIsoThreshold = GET_CYCLE_CEIL(7.5);
        constexpr u32 EmcReqB2bXfer                 = 0x0;
        const     u32 priorityInversionThreshold    = GET_CYCLE_CEIL(22.5);
        const     u32 bc2aaHoldoffThreshold         = table->burst_mc_regs.mc_emem_arb_timing_rc + 1;

        const u32 mc_emem_arb_misc0 = (AtomsPerDvfsPulse << 28) | (McEmcSameFreq << 27) | (ExpiringSoonSlackThreshold << 21) | (priorityInversionIsoThreshold << 16) | (EmcReqB2bXfer << 15) | (priorityInversionThreshold << 8) | (bc2aaHoldoffThreshold << 0);
        table->burst_mc_regs.mc_emem_arb_misc0 = mc_emem_arb_misc0;

        u32 mpcorer_ptsa_rate = MIN(static_cast<u32>(227), (table->rate_khz / 1600000) * 208);
        table->la_scale_regs.mc_mll_mpcorer_ptsa_rate = mpcorer_ptsa_rate;

        u32 ftop_ptsa_rate = MIN(static_cast<u32>(31), (table->rate_khz / 1600000) * 24);
        table->la_scale_regs.mc_ftop_ptsa_rate = ftop_ptsa_rate;

        u32 grant_decrement = MIN(static_cast<u32>(6143), (table->rate_khz / 1600000) * 4611);
        table->la_scale_regs.mc_ptsa_grant_decrement = grant_decrement;

        constexpr u32 MaskHigh = 0xFF00FFFF;
        constexpr u32 Mask2    = 0xFFFFFF00;
        constexpr u32 Mask3    = 0xFF00FF00;

        const u32 allowance1 = static_cast<u32>(0x32000 / (table->rate_khz / 0x3E8)) & 0xFF;
        const u32 allowance2 = static_cast<u32>(0x9C40  / (table->rate_khz / 0x3E8)) & 0xFF;
        const u32 allowance3 = static_cast<u32>(0xB540  / (table->rate_khz / 0x3E8)) & 0xFF;
        const u32 allowance4 = static_cast<u32>(0x9600  / (table->rate_khz / 0x3E8)) & 0xFF;
        const u32 allowance5 = static_cast<u32>(0x8980  / (table->rate_khz / 0x3E8)) & 0xFF;

        table->la_scale_regs.mc_latency_allowance_xusb_0    =              (table->la_scale_regs.mc_latency_allowance_xusb_0    & MaskHigh) | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_xusb_1    =              (table->la_scale_regs.mc_latency_allowance_xusb_1    & MaskHigh) | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_tsec_0    =              (table->la_scale_regs.mc_latency_allowance_tsec_0    & MaskHigh) | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_sdmmcaa_0 =              (table->la_scale_regs.mc_latency_allowance_sdmmcaa_0 & MaskHigh) | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_sdmmcab_0 =              (table->la_scale_regs.mc_latency_allowance_sdmmcab_0 & MaskHigh) | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_sdmmc_0   =              (table->la_scale_regs.mc_latency_allowance_sdmmc_0   & MaskHigh) | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_sdmmca_0  =              (table->la_scale_regs.mc_latency_allowance_sdmmca_0  & MaskHigh) | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_ppcs_1    =              (table->la_scale_regs.mc_latency_allowance_ppcs_1    & MaskHigh) | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_nvdec_0   =              (table->la_scale_regs.mc_latency_allowance_nvdec_0   & MaskHigh) | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_mpcore_0  =              (table->la_scale_regs.mc_latency_allowance_mpcore_0  & MaskHigh) | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_avpc_0    =              (table->la_scale_regs.mc_latency_allowance_avpc_0    & MaskHigh) | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_isp2_1    = allowance1 | (table->la_scale_regs.mc_latency_allowance_isp2_1    & Mask3)    | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_gpu_0     = allowance2 | (table->la_scale_regs.mc_latency_allowance_gpu_0     & Mask3)    | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_gpu2_0    = allowance2 | (table->la_scale_regs.mc_latency_allowance_gpu2_0    & Mask3)    | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_vic_0     = allowance3 | (table->la_scale_regs.mc_latency_allowance_vic_0     & Mask3)    | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_nvenc_0   = allowance4 | (table->la_scale_regs.mc_latency_allowance_nvenc_0   & Mask3)    | (allowance1 << 16);
        table->la_scale_regs.mc_latency_allowance_hc_0      =              (table->la_scale_regs.mc_latency_allowance_hc_0      & Mask2)    |  allowance5;
        table->la_scale_regs.mc_latency_allowance_hc_1      =              (table->la_scale_regs.mc_latency_allowance_hc_1      & Mask2)    |  allowance1;
        table->la_scale_regs.mc_latency_allowance_vi2_0     =              (table->la_scale_regs.mc_latency_allowance_vi2_0     & Mask2)    |  allowance1;

        table->dram_timings.t_rp  = tRP_values[0];
        const u32 tRFCabStock     = tRFC_values[0] * 2;
        table->dram_timings.t_rfc = tRFCabStock;

        table->emc_mrw2        = (table->emc_mrw2 & ~0xFFu) | static_cast<u32>(mrw2);
        table->emc_mrw         = (table->emc_mrw  & ~0x70u) | 0x40; /* nWR */
        table->emc_cfg_2       = 0x11083D;
        table->min_volt        = std::clamp(900 + (C.emcDvbShift * 25), 900, 1050);
    }

    /* TODO: Template this */
    Result VerifyMtcTable(EristaMtcTable *tableStart, u32 expectedFreq) {
        R_UNLESS(tableStart->rate_khz == expectedFreq,  ldr::ResultInvalidMtcTable());
        R_UNLESS(tableStart->rev      == MTC_TABLE_REV, ldr::ResultInvalidMtcTable());

        R_SUCCEED();
    }

    /* TODO: Template this */
    Result MtcValidateAllTables(EristaMtcTable *tableStart, const u32 *validationList, u32 tableCount) {
        for (u32 i = 0; i < tableCount; ++i) {
            R_TRY(VerifyMtcTable(&tableStart[i], validationList[i]));
        }

        R_SUCCEED();
    }

    /* TODO: Put this into common. */
    DramId GetDramId() {
        u64 id64;
        splGetConfig(SplConfigItem_DramId, &id64);
        return static_cast<DramId>(id64);
    }

    MtcTableIndex GetMtcDramIndex(DramId dramId) {
        for (u32 i = 0; i < std::size(mtcIndexTable); ++i) {
            if (mtcIndexTable[i].dramId == dramId) {
                return mtcIndexTable[i].index;
            }
        }

        return MtcTableIndex_Invalid;
    }

    NORETURN void AbortInvalidMtc(const char *crashMsg) {
        panic::SmcError(panic::Emc);
        CRASH(crashMsg);
    }

    u32 GetMtcOffset(MtcTableIndex index) {
        if (index < T210SdevEmcDvfsTableS6gb01) {
            return index * erista::MtcFullTableSize;
        }

        /* Account for the weird in between mariko table. */
        return index * erista::MtcFullTableSize + mariko::MtcFullTableSize;
    }

    void PrepareMtcMemoryRegion(u8 *firstTable, EristaMtcTable *usedTable) {
        /* Move the table, this is nessasary for NLE */
        memmove(firstTable, usedTable, erista::MtcFullTableSize);

        /* Clear all other tables. */
        /* The used table is excluded. */
        constexpr size_t RemainingRegionSize = (mariko::MtcFullTableSize) * (mariko::MtcFullTableCount) + (erista::MtcFullTableSize * (erista::MtcFullTableCount - 1));
        memset(firstTable + erista::MtcFullTableSize, 0, RemainingRegionSize);
    }

    void MtcExtendTables(EristaMtcTable *table) {
        for (u32 i = erista::MtcTableCountDefault; i < newEmcList.size(); ++i) {
            std::memcpy(&table[i], &table[i - 1], sizeof(EristaMtcTable));
            table[i].rate_khz = newEmcList[i];
        }
    }

    /* The silicon instructs; the children obey... */
    void MtcGenerateFreqTables() {
        newEmcList.clear();
        newEmcList.reserve(DvfsTableEntryCount);
        newEmcList.insert(newEmcList.end(), std::begin(EmcListDefault), std::end(EmcListDefault));

        if (C.eristaEmcMaxClock <= EmcClkOSLimit) {
            return;
        }

        /* This is scuffed, but Eristas step rate is... weird? */
        /* 1766MHz seems to cause crashes with other freqs near it... why is anyones guess... */
        u32 freqsLow[]                = { 1633000, 1666000, 1700000, 1733000, 1800000, 1833000, 1862400, };
        constexpr size_t freqsLowSize = std::size(freqsLow);

        for (size_t i = 0; i < freqsLowSize; ++i) {
            if (freqsLow[i] <= C.eristaEmcMaxClock) {
                newEmcList.push_back(freqsLow[i]);
            } else {
                break;
            }
        }

        if (C.eristaEmcMaxClock <= freqsLow[freqsLowSize - 1]) {
            return;
        }

        /* High range. */
        constexpr u32 StepRate = 38400;
        while (newEmcList.back() + StepRate < C.eristaEmcMaxClock) {
            newEmcList.push_back(newEmcList.back() + StepRate);
        }

        if (newEmcList.back() != C.eristaEmcMaxClock) {
            newEmcList.push_back(static_cast<u32>(C.eristaEmcMaxClock));
        }

        constexpr u32 PllmToggleFrequency = 19200;

        /* A step of 19.2khz will cause hangs, crashes and other weirdness. */
        /* Why? ¯\_(ツ)_/¯ */
        if (C.eristaEmcMaxClock - newEmcList[newEmcList.size() - 2] <= PllmToggleFrequency) {
            newEmcList.erase(newEmcList.begin() + newEmcList.size() - 2);
        }

        newEmcList.resize(std::min(newEmcList.size(), DvfsTableEntryLimit));
    }

    Result MemFreqMtcTable(u32 *ptr) {
        static const DramId dramId = [] {
            DramId id = GetDramId();
            return id;
        }();

        static const MtcTableIndex mtcIndex = [] {
            MtcTableIndex idx = GetMtcDramIndex(dramId);
            /* If for some reason this happens, there is no chance of recovering this. */
            if (idx == MtcTableIndex_Invalid) {
                AbortInvalidMtc("Invalid dramId");
            }
            return idx;
        }();

        static const u32 mtcOffset = GetMtcOffset(mtcIndex);

        constexpr u32 StartAdjustment = offsetof(EristaMtcTable, rate_khz) + sizeof(EristaMtcTable) * (erista::MtcTableCountDefault - 1);
        u8 *startPtr = reinterpret_cast<u8 *>(ptr) - StartAdjustment;

        EristaMtcTable *table = reinterpret_cast<EristaMtcTable *>(startPtr + mtcOffset);

        R_TRY(MtcValidateAllTables(table, EmcListDefault, EmcListSizeDefault));

        PrepareMtcMemoryRegion(startPtr, table);
        table = reinterpret_cast<EristaMtcTable *>(startPtr);

        if (R_FAILED(MtcValidateAllTables(table, EmcListDefault, EmcListSizeDefault))) {
            AbortInvalidMtc("Failed mtc validation");
        }

        /* Cache the table for hooks. */
        getMtcTableCache.mtcTable = table;

        if (C.eristaEmcMaxClock <= EmcClkOSLimit) {
            R_SKIP();
        }

        MtcExtendTables(table);

        if (R_FAILED(MtcValidateAllTables(table, newEmcList.data(), newEmcList.size()))) {
            AbortInvalidMtc("Failed mtc validation");
        }

        for (u32 i = erista::MtcTableCountDefault; i < newEmcList.size(); ++i) {
            MemMtcTableAutoAdjust(&table[i]);
        }

        R_SUCCEED();
    }

    Result MemFreqMax(u32 *ptr) {
        if (C.eristaEmcMaxClock <= EmcClkOSLimit) {
            R_SKIP();
        }

        PATCH_OFFSET(ptr, C.eristaEmcMaxClock);

        R_SUCCEED();
    }

    HOOK_PAYLOAD_FN EristaMtcTable *GetEristaMtcTableImpl(u32 *count) {
        const HookPayloadData *data = HOOK_PAYLOAD_PTR(HookPayloadData, e_HookPayloadData);

        *count = data->mtcTableAsm.mtcCount;
        return data->mtcTableAsm.mtcTable;
    }

    Result MtcInstallHooks(HookPayloadData *data) {
        R_UNLESS(getMtcTableCache.getEristaMtcTableFnSite != nullptr && getMtcTableCache.mtcTable != nullptr, ldr::ResultInvalidMtcTablePattern());

        /* Copy the data to the payload. */
        data->mtcTableAsm.mtcTable = reinterpret_cast<EristaMtcTable *>(Hooks().ToVa(getMtcTableCache.mtcTable));
        data->mtcTableAsm.mtcCount = newEmcList.size();

        R_TRY(INSTALL_IMPL_HOOK(getMtcTableCache.getEristaMtcTableFnSite, GetEristaMtcTableImpl));

        R_SUCCEED();
    }

    Result MemMtcTableAsm(u32 *ptr) {
        /* Return if the pattern was already found. */
        /* This pattern happens multiple times in this function., we only need to find it once. */
        R_UNLESS(!getMtcTableCache.foundMtcTablePattern, ldr::ResultInvalidMtcTablePattern());

        /* This is a mess but the compiler made this painful to patch so we must do it this way */
        constexpr u32 AddrpOffset  = 1;
        constexpr u32 MovOffset    = 7;
        constexpr u32 BlOffset     = 5;
        constexpr u32 MovOffsetOld = 8;

        /* Ensure we don't dereference memory before nso start. */
        R_UNLESS(ptr - MovOffset >= nsoStart, ldr::ResultInvalidMtcTablePattern());

        u32 adrp = *(ptr - AddrpOffset);
        R_UNLESS(AsmCompareAdrpNoImm(adrp, MtcAdrpAsm), ldr::ResultInvalidMtcTablePattern());

        /* Check for the branch instruction above the cbz to ensure we are patching the right location*/
        u32 bl = *(ptr - BlOffset);
        R_UNLESS(AsmBlCompareOpcodeOnly(bl, MtcBlIns), ldr::ResultInvalidMtcTablePattern());

        /* Check for the mov that actually sets the mtc table count. */
        u32 mov = *(ptr - MovOffset);
        bool foundMov = false;
        foundMov = asm_compare_no_rd(mov, MtcMovAsm);

        if (!foundMov) {
            mov = *(ptr + MovOffsetOld);
            /* Check old firmware offset. */
            foundMov = asm_compare_no_rd(mov, MtcMovAsm);
        }

        R_UNLESS(foundMov, ldr::ResultInvalidMtcTablePattern());

        constexpr u32 PrologueWindow = 140;
        u32 *functionPrologue = FindFnPrologue(ptr, PrologueWindow, nsoStart);
        R_UNLESS(functionPrologue != nullptr, ldr::ResultInvalidMtcTablePattern());

        getMtcTableCache.getEristaMtcTableFnSite = functionPrologue;
        getMtcTableCache.foundMtcTablePattern = true;

        R_SUCCEED();
    }

}
