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
#include "pcv.hpp"
#include "../mtc_timing_value.hpp"
#include "../erista/calculate_timings_erista.hpp"

namespace ams::ldr::hoc::pcv::erista {

    std::vector<u32> newEmcList;
    u32 *nsoStart;
    u32 *nsoEnd;

    Result CpuVoltDvfs(u32 *ptr) {
        if (std::memcmp(ptr + 5, cpuVoltDvfsPattern, sizeof(cpuVoltDvfsPattern))) {
            R_THROW(ldr::ResultInvalidCpuMinVolt());
        }

        if (C.eristaCpuVmin) {
            PATCH_OFFSET(ptr, C.eristaCpuVmin);
        }

        if (C.eristaCpuUV) {
            PATCH_OFFSET(ptr - 2, C.eristaCpuVmin);
        }

        if (C.eristaCpuMaxVolt) {
            PATCH_OFFSET(ptr + 5, C.eristaCpuMaxVolt);
        }

        R_SUCCEED();
    }

    Result CpuVoltThermals(u32 *ptr) {
        if (std::memcmp(ptr - 6, cpuVoltageThermalPattern, sizeof(cpuVoltageThermalPattern))) {
            R_THROW(ldr::ResultInvalidCpuMinVolt());
        }

        if (C.eristaCpuVmin) {
            PATCH_OFFSET(    ptr, C.eristaCpuVmin);
            PATCH_OFFSET(ptr + 3, C.eristaCpuVmin);
            PATCH_OFFSET(ptr + 6, C.eristaCpuVmin);
        }

        if (C.eristaCpuMaxVolt) {
            PATCH_OFFSET(ptr - 2, C.eristaCpuMaxVolt);
            PATCH_OFFSET(ptr + 1, C.eristaCpuMaxVolt);
            PATCH_OFFSET(ptr + 4, C.eristaCpuMaxVolt);
            PATCH_OFFSET(ptr + 7, C.eristaCpuMaxVolt);
        }

        R_SUCCEED();
    }

    Result CpuVoltDfll(u32* ptr) {
        CvbCpuDfllData *entry = reinterpret_cast<CvbCpuDfllData *>(ptr);

        R_UNLESS(entry->tune0_low  == 0xFFEAD0FF, ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune0_high == 0x0,        ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune1_low  == 0x0,        ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune1_high == 0x0,        ldr::ResultInvalidCpuVoltDfllEntry());

        if (!C.eristaCpuUV) {
            R_SKIP();
        }

        switch(C.eristaCpuUV) {
            case 1:
                PATCH_OFFSET(&(entry->tune0_high), 0xffff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27007ff);
                break;
            case 2:
                PATCH_OFFSET(&(entry->tune0_high), 0xefff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27407ff);
                break;
            case 3:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27807ff);
                break;
            case 4:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27a07ff);
                break;
            case 5:
                PATCH_OFFSET(&(entry->tune0_high), 0xcfdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x37007ff);
                break;
            default:
                break;
        }

        R_SUCCEED();
    }

    Result GpuVoltDVFS(u32 *ptr) {
        if (std::memcmp(ptr, gpuVoltDvfsPattern, sizeof(gpuVoltDvfsPattern))) {
            R_THROW(ldr::ResultInvalidGpuDvfs());
        }

        if (C.eristaGpuVmin) {
            PATCH_OFFSET(ptr, C.eristaGpuVmin);
        }

        R_SUCCEED();
    }

    Result GpuVoltThermals(u32 *ptr) {
        if (std::memcmp(ptr - 3, gpuVoltThermalPattern, sizeof(gpuVoltThermalPattern))) {
            R_THROW(ldr::ResultInvalidGpuDvfs());
        }

        if (C.eristaGpuVmin) {
            PATCH_OFFSET(ptr,      C.eristaGpuVmin);
            PATCH_OFFSET(ptr + 3,  C.eristaGpuVmin);
            PATCH_OFFSET(ptr + 6,  C.eristaGpuVmin);
            PATCH_OFFSET(ptr + 9,  C.eristaGpuVmin);
            PATCH_OFFSET(ptr + 12, C.eristaGpuVmin);
        }

        R_SUCCEED();
    }

    Result GpuFreqMaxAsm(u32 *ptr32) {
        // Check if both two instructions match the pattern
        u32 ins1 = *ptr32, ins2 = *(ptr32 + 1);
        if (!(asm_compare_no_rd(ins1, GpuAsmPattern[0]) && asm_compare_no_rd(ins2, GpuAsmPattern[1]))) {
            R_THROW(ldr::ResultInvalidGpuFreqMaxPattern());
        }

        // Both instructions should operate on the same register
        u8 rd = asm_get_rd(ins1);
        if (rd != asm_get_rd(ins2)) {
            R_THROW(ldr::ResultInvalidGpuFreqMaxPattern());
        }

        u32 max_clock;
        switch (C.eristaGpuUV) {
        case 0:
            max_clock = GetDvfsTableLastEntry(C.eristaGpuDvfsTable)->freq;
            break;
        case 1:
            max_clock = GetDvfsTableLastEntry(C.eristaGpuDvfsTableSLT)->freq;
            break;
        case 2:
            max_clock = GetDvfsTableLastEntry(C.eristaGpuDvfsTableHiOPT)->freq;
            break;
        default:
            max_clock = GetDvfsTableLastEntry(C.eristaGpuDvfsTable)->freq;
            break;
        }

        u32 asm_patch[2] = {
            asm_set_rd(asm_set_imm16(GpuAsmPattern[0], max_clock), rd),
            asm_set_rd(asm_set_imm16(GpuAsmPattern[1], max_clock >> 16), rd)
        };

        PATCH_OFFSET(ptr32,     asm_patch[0]);
        PATCH_OFFSET(ptr32 + 1, asm_patch[1]);

        R_SUCCEED();
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
        WRITE_PARAM_ALL_REG(table, emc_tcke, GET_CYCLE_CEIL(7.425) + 2);
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
        WRITE_PARAM_ALL_REG(table, emc_pchg2pden, GET_CYCLE_CEIL(1.763));
        WRITE_PARAM_ALL_REG(table, emc_ar2pden, GET_CYCLE_CEIL(1.75));
        WRITE_PARAM_ALL_REG(table, emc_pdex2cke, GET_CYCLE_CEIL(1.05));
        WRITE_PARAM_ALL_REG(table, emc_act2pden, GET_CYCLE_CEIL(14.0));
        WRITE_PARAM_ALL_REG(table, emc_cke2pden, GET_CYCLE_CEIL(8.499));
        WRITE_PARAM_ALL_REG(table, emc_pdex2mrr, GET_CYCLE_CEIL(pdex2mrr));
        WRITE_PARAM_ALL_REG(table, emc_rw2pden, tWTPDEN);

        /* Accept imperfection or prepare for suffering. */
        // #if defined(AMS_BUILD_FOR_AUDITING) || defined(AMS_BUILD_FOR_DEBUGGING)
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
        // WRITE_PARAM_ALL_REG(table, emc_tr_qpop, qpop);
        // WRITE_PARAM_ALL_REG(table, emc_qpop, qpop);
        // WRITE_PARAM_ALL_REG(table, emc_rdv, rdv);
        // WRITE_PARAM_ALL_REG(table, emc_tr_rdv_mask, rdv + 2);
        // WRITE_PARAM_ALL_REG(table, emc_rdv_early, rdv - 2);
        // WRITE_PARAM_ALL_REG(table, emc_rdv_early_mask, rdv);
        // WRITE_PARAM_ALL_REG(table, emc_rdv_mask, rdv + 2);
        // WRITE_PARAM_ALL_REG(table, emc_tr_rdv, rdv);
        table->emc_mrw2 = (table->emc_mrw2 & ~0xFFu) | static_cast<u32>(mrw2);
        // table->dram_timings.rl = RL;
        // #endif

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

    /* Relocate the table */
    /* Rescanning is simpler than trying to extract a bunch of data from the asm patch, performance impact is negligable */
    /* Also, this is more stable :P */
    u32 RepointEristaEmcTablePtr(uintptr_t fromSlot, uintptr_t toTable) {
        constexpr u32 RetIns = 0xD65F03C0; /* ret */
        u32 patched = 0;

        for (u32 *p = nsoStart; p + 4 < nsoEnd; ++p) {
            const u32 ins = *p;
            if (!AsmIsAdrX0(ins)) {
                continue;
            }

            const uintptr_t pc = reinterpret_cast<uintptr_t>(p);
            if (AsmAdrTarget(ins, pc) != fromSlot) {
                continue;
            }
            if (!(AsmIsLdpX(p[1]) && AsmIsLdpX(p[2]) && p[3] == RetIns)) {
                continue;
            }

            /* adr only reaches +-1MB */
            const s64 delta = static_cast<s64>(toTable) - static_cast<s64>(pc);
            if (delta > 0xFFFFF || delta < -0x100000) {
                continue;
            }

            PATCH_OFFSET(p, AsmSetAdrTarget(ins, pc, toTable));
            ++patched;
        }
        return patched;
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

        const uintptr_t usedSlot = reinterpret_cast<uintptr_t>(startPtr) + mtcOffset;
        EristaMtcTable *table = reinterpret_cast<EristaMtcTable *>(usedSlot);
        R_TRY(MtcValidateAllTables(table, EmcListDefault, EmcListSizeDefault));

        PrepareMtcMemoryRegion(startPtr, table);
        table = reinterpret_cast<EristaMtcTable *>(startPtr);

        /* We must do this as the NLE tables don't have enough space past them for our extended ones */
        if (usedSlot != reinterpret_cast<uintptr_t>(startPtr)) {
            if (RepointEristaEmcTablePtr(usedSlot, reinterpret_cast<uintptr_t>(startPtr)) == 0) {
                AbortInvalidMtc("Failed to repoint emc table");
            }
        }

        if (R_FAILED(MtcValidateAllTables(table, EmcListDefault, EmcListSizeDefault))) {
            AbortInvalidMtc("Failed mtc validation");
        }

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

    Result GpuFreqPllMax(u32 *ptr) {
        clk_pll_param *entry = reinterpret_cast<clk_pll_param *>(ptr);

        // All zero except for freq
        for (size_t i = 1; i < sizeof(clk_pll_param) / sizeof(u32); i++) {
            R_UNLESS(*(ptr + i) == 0, ldr::ResultInvalidGpuPllEntry());
        }

        // Double the max clk simply
        u32 max_clk = entry->freq * 2;
        entry->freq = max_clk;
        R_SUCCEED();
    }

    // patch out 1305MHz limit on erista, don't use this!
    // Result GpuFreqPllLimit(u32 *ptr) {
    //     u32 prev_freq = *(ptr - 1);

    //     if (prev_freq != 128000 && prev_freq != 1300000 && prev_freq != 76800) {
    //         R_THROW(ldr::ResultInvalidGpuPllEntry());
    //     }

    //     PATCH_OFFSET(ptr, 3600000);

    //     R_SUCCEED();
    // }


    Result MemMtcTableAsm(u32 *ptr) {
        /* This is a mess but the compiler made this painful to patch so we must do it this way */
        constexpr s32 GoodAdrpOffset = -1;
        constexpr s32 GoodMovOffset  = -7;
        constexpr s32 GoodBlOffset       = 1;
        constexpr u32 MtcGoodBlOpcode = 0x97fe6cfc;

        constexpr u32 MtcBadBlOpcode0 = 0x97ffae64; // bl nn::pcv::GetHardwareType
        constexpr u32 MtcBadBlOpcode1 = 0x940036d5; // bl strcmp
        constexpr u32 MtcBadAdrpAsm = 0xd00000a1; // adrp x1, s_ModuleResetStatus_

        constexpr s32 MtcBadBlOffset0 = 2;
        constexpr s32 MtcBadBlOffset1 = -1;
        constexpr s32 MtcBadAdrpOffset = 1;

        /* Ensure we don't dereference memory before nso start. */
        R_UNLESS(ptr + GoodMovOffset >= nsoStart, ldr::ResultInvalidMtcTablePattern());

        /* Check for GetHardwareType asm and skip if it is found */
        /* The pattern will match on the first time, but the location is bad, so it must be skipped */
        if(AsmCompareAdrpNoImm(*(ptr + MtcBadAdrpOffset), MtcBadAdrpAsm) && AsmBlCompareOpcodeOnly(*(ptr + MtcBadBlOffset0), MtcBadBlOpcode0) && AsmBlCompareOpcodeOnly(*(ptr + MtcBadBlOffset1), MtcBadBlOpcode1)) {
            R_SKIP();
        }

        /* We don't check for matching register because both registers must be x0 in order to pass the previous checks. */
        /* The correct instructions will always be x0 since the mtcTable pointer is returned. */
        u32 adrp = *(ptr + GoodAdrpOffset);
        R_UNLESS(AsmCompareAdrpNoImm(adrp, MtcAdrpAsm), ldr::ResultInvalidMtcTablePattern());


        /* Check for the branch instruction above the cbz to ensure we are patching the right location*/
        u32 bl = *(ptr + GoodBlOffset);
        R_UNLESS(AsmBlCompareOpcodeOnly(bl, MtcGoodBlOpcode), ldr::ResultInvalidMtcTablePattern());


        /* Check for the mov that actually sets the mtc table count. */
        u32 mov = *(ptr + GoodMovOffset);
        R_UNLESS(asm_compare_no_rd(mov, MtcMovAsm), ldr::ResultInvalidMtcTablePattern());

        /* Patch out the count of the mov to our custom mtc table amount*/
        u32 movCountPatch = asm_set_rd(asm_set_imm16(MtcMovAsm, newEmcList.size()), asm_get_rd(mov));

        PATCH_OFFSET(ptr + GoodMovOffset, movCountPatch);

        R_SUCCEED();
    }

    void Patch(uintptr_t mapped_nso, size_t nso_size) {
        nsoStart = reinterpret_cast<u32 *>(mapped_nso);
        nsoEnd   = reinterpret_cast<u32 *>(mapped_nso + nso_size);
        MtcGenerateFreqTables();

        u32 CpuCvbDefaultMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(CpuCvbTableDefault)->freq);
        u32 GpuCvbDefaultMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(GpuCvbTableDefault)->freq);

        PatcherEntry<u32> patches[] = {
            {"CPU Freq Table",     CpuFreqCvbTable<false>, 1, nullptr,  CpuCvbDefaultMaxFreq },
            {"CPU Volt DVFS",     &CpuVoltDvfs,            1, nullptr,  CpuVminOfficial      },
            {"CPU Volt Thermals", &CpuVoltThermals,        1, nullptr,  CpuVminOfficial      },
            {"CPU Volt Dfll",     &CpuVoltDfll,            1, nullptr,  CpuTune0Low          },
            {"GPU Volt DVFS",     &GpuVoltDVFS,            1, nullptr,  GpuVminOfficial      },
            {"GPU Volt Thermals", &GpuVoltThermals,        1, nullptr,  GpuVminOfficial      },
            {"GPU Freq Table",     GpuFreqCvbTable<false>, 1, nullptr,  GpuCvbDefaultMaxFreq },
            {"GPU Freq Asm",      &GpuFreqMaxAsm,          2,          &GpuMaxClockPatternFn },
            {"GPU PLL Max", &      GpuFreqPllMax,          1, nullptr,  GpuClkPllMax         },
            // {"GPU PLL Limit",  &GpuFreqPllLimit,        4, nullptr,  GpuClkPllLimit       },
            {"MEM Table Asm",     &MemMtcTableAsm,         4,           &MemMtcGetGetTablePatternFn },
            {"MEM Freq Mtc",      &MemFreqMtcTable,        1, nullptr,  EmcClkOSLimit        },
            {"MEM Freq Max",      &MemFreqMax,             0, nullptr,  EmcClkOSLimit        },
            {"MEM Freq PLLM",     &MemFreqPllmLimit,       2, nullptr,  EmcClkPllmLimit      },
            {"MEM Volt",          &MemVoltHandler,         2, nullptr,  MemVoltHOS           },
        };

        for (uintptr_t ptr = mapped_nso; ptr <= mapped_nso + nso_size - sizeof(EristaMtcTable); ptr += sizeof(u32)) {
            u32 *ptr32 = reinterpret_cast<u32 *>(ptr);
            for (auto &entry : patches) {
                if (R_SUCCEEDED(entry.SearchAndApply(ptr32))) {
                    break;
                }
            }
        }

        for (auto &entry : patches) {
            LOGGING("%s Count: %zu\n", entry.description, entry.patched_count);
            if (R_FAILED(entry.CheckResult())) {
                panic::SmcError(panic::Patch);

                CRASH(entry.description);
            }
        }
    }

}
