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

#include <vector>
#include "pcv.hpp"
#include "../mtc_timing_value.hpp"
#include "../mariko/calculate_timings.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    Result GpuVoltDVFS(u32 *ptr) {
        /* Check for valid pattern. */
        for (size_t i = 0; i < std::size(gpuDVFSPattern); ++i) {
            if (*(ptr + i + 1) != gpuDVFSPattern[i]) {
                R_THROW(ldr::ResultInvalidGpuDvfs());
            }
        }

        /* Default value is 1050mV. */
        if (C.marikoGpuVmax) {
            PATCH_OFFSET(ptr + 1, C.marikoGpuVmax);
        }

        if (C.marikoGpuVmin) {
            PATCH_OFFSET(ptr, C.marikoGpuVmin);
        }

        R_SUCCEED();
    }

    Result GpuVoltThermals(u32 *ptr) {
        if (std::memcmp(ptr - 3, gpuVoltThermalPattern, sizeof(gpuVoltThermalPattern))) {
            R_THROW(ldr::ResultInvalidGpuDvfs());
        }

        // if (C.marikoGpuBootVolt) {
        //     PATCH_OFFSET(ptr - 3, C.marikoGpuBootVolt);
        // }

        if (C.marikoGpuVmin) {
            PATCH_OFFSET(ptr,      C.marikoGpuVmin);
            PATCH_OFFSET(ptr +  3, C.marikoGpuVmin);
            PATCH_OFFSET(ptr +  6, C.marikoGpuVmin);
            PATCH_OFFSET(ptr +  9, C.marikoGpuVmin);
            PATCH_OFFSET(ptr + 12, C.marikoGpuVmin);
        }

        R_SUCCEED();
    }

    u32 CapCpuClock() {
        u32 cpuCap = allowedCpuMaxFrequencies[0];

        for (u32 freq : allowedCpuMaxFrequencies) {
            if (C.marikoCpuMaxClock >= freq) {
                cpuCap = freq;
            } else {
                break;
            }
        }
        return cpuCap;
    }

    Result CpuFreqVdd(u32 *ptr) {
        dvfs_rail *entry = reinterpret_cast<dvfs_rail *>(reinterpret_cast<u8 *>(ptr) - offsetof(dvfs_rail, freq));

        R_UNLESS(entry->id      == 1,        ldr::ResultInvalidCpuFreqVddEntry());
        R_UNLESS(entry->min_mv  == 250'000,  ldr::ResultInvalidCpuFreqVddEntry());
        R_UNLESS(entry->step_mv == 5000,     ldr::ResultInvalidCpuFreqVddEntry());
        R_UNLESS(entry->max_mv  == 1525'000, ldr::ResultInvalidCpuFreqVddEntry());

        if (C.marikoCpuUVHigh) {
            PATCH_OFFSET(ptr, CapCpuClock());
        } else {
            PATCH_OFFSET(ptr, GetDvfsTableLastEntry(C.marikoCpuDvfsTable)->freq);
        }

        R_SUCCEED();
    }

    Result CpuVoltDVFS(u32 *ptr) {
        CvbMeta *cpuCvbMeta = reinterpret_cast<CvbMeta *>(reinterpret_cast<u8 *>(ptr) - offsetof(CvbMeta, vmin));

        R_UNLESS(cpuCvbMeta->highVmin     == CpuHighVminOfficial, ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->unkStepMaybe == 38,                  ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->vmax         == CpuVoltOfficial,     ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->unkScale2    == 1000,                ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->speedoScale  == 100,                 ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->voltageScale == 1000,                ldr::ResultInvalidCpuMinVolt());
        R_UNLESS(cpuCvbMeta->unkZero5     == 0,                   ldr::ResultInvalidCpuMinVolt());

        if (C.marikoCpuLowVmin) {
            PATCH_OFFSET(&(cpuCvbMeta->vmin), C.marikoCpuLowVmin);
        }

        if (C.marikoCpuHighVmin) {
            PATCH_OFFSET(&(cpuCvbMeta->highVmin), C.marikoCpuHighVmin);
        }

        if (C.marikoCpuMaxVolt) {
            PATCH_OFFSET(&(cpuCvbMeta->vmax), C.marikoCpuMaxVolt);
        }

        R_SUCCEED();
    }

    Result CpuVoltThermals(u32 *ptr) {
        if (std::memcmp(ptr, cpuVoltThermalData, sizeof(cpuVoltThermalData))) {
            R_THROW(ldr::ResultInvalidCpuMinVolt());
        }

        if (C.marikoCpuLowVmin) {
            PATCH_OFFSET(ptr,     C.marikoCpuLowVmin);
            PATCH_OFFSET(ptr + 3, C.marikoCpuLowVmin);
        }

        if (C.marikoCpuMaxVolt) {
            PATCH_OFFSET(ptr - 2, C.marikoCpuMaxVolt);
            PATCH_OFFSET(ptr - 5, C.marikoCpuMaxVolt);
            PATCH_OFFSET(ptr + 1, C.marikoCpuMaxVolt);
            PATCH_OFFSET(ptr + 4, C.marikoCpuMaxVolt);
        }

        R_SUCCEED();
    }

    Result CpuVoltDfll(u32 *ptr) {
        CvbCpuDfllData *entry = reinterpret_cast<CvbCpuDfllData *>(ptr);

        R_UNLESS(entry->tune0_low  == 0xFFCF,    ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune0_high == 0x0,       ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune1_low  == 0x12207FF, ldr::ResultInvalidCpuVoltDfllEntry());
        R_UNLESS(entry->tune1_high == 0x3FFF7FF, ldr::ResultInvalidCpuVoltDfllEntry());

        switch (C.marikoCpuUVLow) {
            case 1:
                PATCH_OFFSET(&(entry->tune0_low),  0xffa0);
                PATCH_OFFSET(&(entry->tune0_high), 0xffff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21107ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x0);
                break;
            case 2:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_low),  0x21107ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27207ff);
                break;
            case 3:
                PATCH_OFFSET(&(entry->tune0_low),  0xffdf);
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_low),  0x21107ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27307ff);
                break;
            case 4:
                PATCH_OFFSET(&(entry->tune0_low),  0xffff);
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_low),  0x21107ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27407ff);
                break;
            case 5:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_low),  0x21607ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27707ff);
                break;
            case 6:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_low),  0x21607ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27807ff);
                break;
            case 7:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21607ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27b07ff);
                break;
            case 8:
                PATCH_OFFSET(&(entry->tune0_low),  0xdfff);
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21707ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27b07ff);
                break;
            case 9:
                PATCH_OFFSET(&(entry->tune0_low),  0xdfff);
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21707ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27c07ff);
                break;
            case 10:
                PATCH_OFFSET(&(entry->tune0_low),  0xdfff);
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21707ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27d07ff);
                break;
            case 11:
                PATCH_OFFSET(&(entry->tune0_low),  0xdfff);
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21707ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27e07ff);
                break;
            case 12:
                PATCH_OFFSET(&(entry->tune0_low),  0xdfff);
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_low),  0x21707ff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27f07ff);
                break;
            default:
                break;
        }

        switch (C.marikoCpuUVHigh) {
            case 1:
                PATCH_OFFSET(&(entry->tune1_high), 0x0);
                PATCH_OFFSET(&(entry->tune0_high), 0xffff);
                break;
            case 2:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27207ff);
                break;
            case 3:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27307ff);
                break;
            case 4:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27407ff);
                break;
            case 5:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27707ff);
                break;
            case 6:
                PATCH_OFFSET(&(entry->tune0_high), 0xffdf);
                PATCH_OFFSET(&(entry->tune1_high), 0x27807ff);
                break;
            case 7:
            case 8:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27b07ff);
                break;
            case 9:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27c07ff);
                break;
            case 10:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27d07ff);
                break;
            case 11:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27e07ff);
                break;
            case 12:
                PATCH_OFFSET(&(entry->tune0_high), 0xdfff);
                PATCH_OFFSET(&(entry->tune1_high), 0x27f07ff);
                break;
            default:
                break;
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
        switch (C.marikoGpuUV) {
        case 0:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTable)->freq;
            break;
        case 1:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTableSLT)->freq;
            break;
        case 2:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTableHiOPT)->freq;
            break;
        case 3:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTableHiOPT15)->freq;
            break;
        case 4:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTableHighUV)->freq;
            break;
        default:
            max_clock = GetDvfsTableLastEntry(C.marikoGpuDvfsTableHiOPT)->freq;
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

    Result GpuFreqPllLimit(u32 *ptr) {
        u32 prev_freq = *(ptr - 1);

        if (prev_freq != 128000 && prev_freq != 1300000 && prev_freq != 76800) {
            R_THROW(ldr::ResultInvalidGpuPllEntry());
        }

        PATCH_OFFSET(ptr, 3600000);

        R_SUCCEED();
    }

    void MemMtcTableAutoAdjust(MarikoMtcTable *table) {
        /* Official Tegra X1 TRM, sign up for nvidia developer program (free) to download:
         *     https://developer.nvidia.com/embedded/dlc/tegra-x1-technical-reference-manual
         *     Section 18.11.1:  MC Registers
         *     Section 18.11.2: EMC Registers
         */

        #define WRITE_PARAM_ALL_REG(TABLE, PARAM, VALUE) \
            TABLE->burst_regs.PARAM = VALUE;             \
            TABLE->shadow_regs_ca_train.PARAM   = VALUE; \
            TABLE->shadow_regs_rdwr_train.PARAM = VALUE;

        const double tCK_avg = 1000'000.0 / table->rate_khz;

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
        WRITE_PARAM_ALL_REG(table, emc_trtm, tRTM);
        WRITE_PARAM_ALL_REG(table, emc_tratm, tRATM);
        WRITE_PARAM_ALL_REG(table, emc_w2p, tW2P);
        WRITE_PARAM_ALL_REG(table, emc_w2r, tW2R);
        WRITE_PARAM_ALL_REG(table, emc_twtm, tWTM);
        WRITE_PARAM_ALL_REG(table, emc_twatm, tWATM);
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
        WRITE_PARAM_ALL_REG(table, emc_einput, einput);
        WRITE_PARAM_ALL_REG(table, emc_einput_duration, einput_duration);
        WRITE_PARAM_ALL_REG(table, emc_obdly, obdly);
        WRITE_PARAM_ALL_REG(table, emc_ibdly, ibdly);
        WRITE_PARAM_ALL_REG(table, emc_wdv_mask, wdv);
        WRITE_PARAM_ALL_REG(table, emc_quse_width, quse_width);
        WRITE_PARAM_ALL_REG(table, emc_quse, quse);
        WRITE_PARAM_ALL_REG(table, emc_wdv, wdv);
        WRITE_PARAM_ALL_REG(table, emc_wsv, wsv);
        WRITE_PARAM_ALL_REG(table, emc_wev, wev);
        WRITE_PARAM_ALL_REG(table, emc_qrst, qrst);
        WRITE_PARAM_ALL_REG(table, emc_tr_qrst, qrst);
        WRITE_PARAM_ALL_REG(table, emc_qsafe, qsafe);
        WRITE_PARAM_ALL_REG(table, emc_tr_qsafe, qsafe);
        WRITE_PARAM_ALL_REG(table, emc_tr_qpop, qpop);
        WRITE_PARAM_ALL_REG(table, emc_qpop, qpop);
        WRITE_PARAM_ALL_REG(table, emc_rdv, rdv);
        WRITE_PARAM_ALL_REG(table, emc_tr_rdv_mask, rdv + 2);
        WRITE_PARAM_ALL_REG(table, emc_rdv_early, rdv - 2);
        WRITE_PARAM_ALL_REG(table, emc_rdv_early_mask, rdv);
        WRITE_PARAM_ALL_REG(table, emc_rdv_mask, rdv + 2);
        WRITE_PARAM_ALL_REG(table, emc_tr_rdv, rdv);
        WRITE_PARAM_ALL_REG(table, emc_cmd_brlshft_2, 0x24);
        WRITE_PARAM_ALL_REG(table, emc_cmd_brlshft_3, 0x24);

        /* This needs some clean up. */
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

        /* Two consecutive reads between two different dram modules. */
        /* Only above 1 for 8gb ram. */
        if (table->burst_mc_regs.mc_emem_arb_timing_r2r > 1) {
            table->burst_mc_regs.mc_emem_arb_timing_r2r = CEIL(table->burst_regs.emc_rext / 4) - 1 + MC_ARB_SFA;
        }

        /* Same as r2r but for write. */
        if (table->burst_mc_regs.mc_emem_arb_timing_w2w > 1) {
            table->burst_mc_regs.mc_emem_arb_timing_w2w = CEIL(table->burst_regs.emc_wext / MC_ARB_DIV) - 1 + MC_ARB_SFA;
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

        constexpr u32 AtomsPerDvfsPulse      = 0x7;
        constexpr u32 McEmcSameFreq          = 0x0;
        const u32 expiringSoonSlackThreshold = [&] {
            switch (table->rate_khz) {
                case 2966000:
                case 3100000:
                case 3133000:
                case 3200000:
                    return 0x12u;
                default:
                    return 0x13u;
            }
        }();

        const     u32 priorityInversionIsoThreshold = GET_CYCLE_CEIL(7.5);
        constexpr u32 EmcReqB2bXfer                 = 0x0;
        const     u32 priorityInversionThreshold    = GET_CYCLE_CEIL(22.5);
        const     u32 bc2aaHoldoffThreshold         = table->burst_mc_regs.mc_emem_arb_timing_rc + 1;

        const u32 mc_emem_arb_misc0 = (AtomsPerDvfsPulse << 28) | (McEmcSameFreq << 27) | (expiringSoonSlackThreshold << 21) | (priorityInversionIsoThreshold << 16) | (EmcReqB2bXfer << 15) | (priorityInversionThreshold << 8) | (bc2aaHoldoffThreshold << 0);
        table->burst_mc_regs.mc_emem_arb_misc0 = mc_emem_arb_misc0;

        table->la_scale_regs.mc_mll_mpcorer_ptsa_rate = 0x115;

        if (table->rate_khz >= 2133000) {
            table->la_scale_regs.mc_ftop_ptsa_rate = 0x1F;
        } else {
            table->la_scale_regs.mc_ftop_ptsa_rate = 0x1B;
        }

        table->la_scale_regs.mc_ptsa_grant_decrement = 0x17ff;

        constexpr u32 MaskHigh = 0xFF00FFFF;
        constexpr u32 Mask2    = 0xFFFFFF00;
        constexpr u32 Mask3    = 0xFF00FF00;

        const u32 allowance1 = static_cast<u32>(0x32000 / (table->rate_khz / 1000)) & 0xFF;
        const u32 allowance2 = static_cast<u32>(0x9C40  / (table->rate_khz / 1000)) & 0xFF;
        const u32 allowance3 = static_cast<u32>(0xB540  / (table->rate_khz / 1000)) & 0xFF;
        const u32 allowance4 = static_cast<u32>(0x9600  / (table->rate_khz / 1000)) & 0xFF;
        const u32 allowance5 = static_cast<u32>(0x8980  / (table->rate_khz / 1000)) & 0xFF;

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

        table->dram_timings.rl = RL;
        table->emc_mrw2        = (table->emc_mrw2 & ~0xFFu) | static_cast<u32>(mrw2);
        table->emc_mrw         = (table->emc_mrw  & ~0x70u) | 0x40; /* nWR */
        table->emc_cfg_2       = 0x11083D;
    }

    void MemMtcPllmbDivisor(MarikoMtcTable *table) {
        constexpr u32 PllOscInKHz   = 38400;
        constexpr u32 PllOscHalfKHz = 19200;

        double target_freq_d = static_cast<double>(table->rate_khz);

        s32 divm_candidate_half = static_cast<u8>(table->rate_khz / PllOscHalfKHz);

        bool remainder_check = (table->rate_khz - PllOscInKHz * (table->rate_khz / PllOscInKHz)) > (table->rate_khz - PllOscHalfKHz * divm_candidate_half) && static_cast<int>(((target_freq_d / PllOscHalfKHz - divm_candidate_half - 0.5) * 8192.0)) != 0;

        u32 divm_final    = remainder_check + 1;
        table->pllmb_divm = divm_final;

        double div_step_d = static_cast<double>(PllOscInKHz) / divm_final;
        s32 divn_integer  = static_cast<u8>(table->rate_khz  / div_step_d);
        table->pllmb_divn = divn_integer;

        u32 divn_fraction = static_cast<s32>((target_freq_d / div_step_d - divn_integer - 0.5) * 8192.0);

        u32 actual_freq_khz = static_cast<u32>((divn_integer + 0.5 + divn_fraction * 0.000122070312) * div_step_d);

        if (table->rate_khz - 2366001 < 133999) {
            s32 divn_fraction_ssc = static_cast<s32>((actual_freq_khz * 0.997 / div_step_d - divn_integer - 0.5) * 8192.0);

            double delta_scaled = (0.3 / div_step_d + 0.3 / div_step_d) * (divn_fraction - divn_fraction_ssc);
            s32 delta_int       = static_cast<s32>(delta_scaled);
            double delta_frac   = delta_scaled - delta_int;

            u32 setup_value = 0;
            if (delta_frac <= 0.5) {
                double round_val = (delta_int + ROUND(delta_frac + delta_frac)) ? 0.5 : 0.0;
                setup_value      = ROUND(delta_frac + delta_frac) ? static_cast<u32>(round_val + round_val) | 0x1000 : static_cast<u32>(round_val);
            } else {
                s32 frac_doubled = ROUND(delta_frac - 0.5 + delta_frac - 0.5);
                double round_val = 1.0;
                setup_value      = frac_doubled ? static_cast<u32>(round_val) : static_cast<u32>(round_val + round_val) | 0x1000;
            }

            u32 ctrl1 = static_cast<u16>(divn_fraction_ssc) | (static_cast<u16>(divn_fraction) << 16);
            u32 ctrl2 = static_cast<u16>(divn_fraction)     | (static_cast<u16>(setup_value) << 16);

            table->pllm_ss_ctrl1  = ctrl1;
            table->pllm_ss_ctrl2  = ctrl2;
            table->pllmb_ss_ctrl1 = ctrl1;
            table->pllmb_ss_ctrl2 = ctrl2;
        } else {
            table->pllm_ss_cfg  &= 0xBFFFFFFF;
            table->pllmb_ss_cfg &= 0xBFFFFFFF;

            u64 pair     = (static_cast<u64>(divn_fraction) << 32) | static_cast<u64>(table->rate_khz);
            u32 pll_misc = (table->pllm_ss_ctrl2 & 0xFFFF0000)     | static_cast<u32>((pair - actual_freq_khz) >> 32);

            table->pllm_ss_ctrl2  = pll_misc;
            table->pllmb_ss_ctrl2 = pll_misc;
        }
    }

    namespace {
        std::vector<u32> newEmcList;
        u32 *nsoStart;
        size_t g_nso_size = 0;
        uintptr_t g_cave_cursor = 0;
    }

    static uintptr_t CaveReserve(size_t count) {
        if (g_pcv_cave == 0 || g_cave_cursor == 0) {
            return 0;
        }
        if (g_cave_cursor + count * sizeof(u32) > g_pcv_cave + g_pcv_cave_size) {
            return 0;
        }
        const uintptr_t entry = g_cave_cursor;
        g_cave_cursor += count * sizeof(u32);
        return entry;
    }

    #ifdef HOC_UART_LOG
    /* Redirect pcv's NvLog() calls to UART */
    Result NvLogUartRedirect(u32 *ptr) {
        const uintptr_t mapped_nso     = reinterpret_cast<uintptr_t>(nsoStart);
        const size_t    nso_size       = g_nso_size;
        const uintptr_t textEnd        = g_pcv_cave; /* .text ends where the cave begins */
        const uintptr_t vsnprintf_addr = reinterpret_cast<uintptr_t>(ptr);

        /* NvLog via the VDD_SOC log */
        static const char Fmt[] = "%s(%s): DVFS request VDD_SOC %d mV\n";
        constexpr size_t FmtLen = sizeof(Fmt) - 1;
        uintptr_t strAddr = 0;
        {
            const char *hay = reinterpret_cast<const char *>(mapped_nso);
            for (size_t i = 0; i + FmtLen <= nso_size; ++i) {
                if (std::memcmp(hay + i, Fmt, FmtLen) == 0) { strAddr = mapped_nso + i; break; }
            }
        }
        if (strAddr == 0) {
            LOGGING("NvLogRedirect: fmt string not found (vsnprintf@+%lx)", vsnprintf_addr - mapped_nso);
            R_THROW(ldr::ResultInvalidNvLogRedirect());
        }

        uintptr_t nvlog_addr = 0;
        for (u32 *p = nsoStart; reinterpret_cast<uintptr_t>(p + 2) <= textEnd; ++p) {
            const uintptr_t pc = reinterpret_cast<uintptr_t>(p);
            if (!AsmIsAdrp(p[0])) {
                continue;
            }
            const uintptr_t adrpPage = (pc & ~static_cast<uintptr_t>(0xFFFu)) + static_cast<uintptr_t>(AsmAdrpPageOffset(p[0]));
            if (adrpPage != (strAddr & ~static_cast<uintptr_t>(0xFFFu))) {
                continue;
            }
            const u32 reg = asm_get_rd(p[0]);
            if (!(AsmIsAddImm64(p[1]) && asm_get_rd(p[1]) == reg && AsmGetRn(p[1]) == reg && AsmGetImm12(p[1]) == (strAddr & 0xFFFu))) {
                continue;
            }
            for (u32 k = 2; k <= 12 && (pc + (k + 1) * 4) <= textEnd; ++k) {
                if (AsmIsBl(p[k])) { nvlog_addr = AsmBranchTarget(p[k], pc + k * 4); break; }
            }
            if (nvlog_addr != 0) {
                break;
            }
        }
        if (nvlog_addr == 0 || nvlog_addr < mapped_nso || nvlog_addr >= textEnd) {
            LOGGING("NvLogRedirect: NvLog entry not found (fmt@+%lx)", strAddr - mapped_nso);
            R_THROW(ldr::ResultInvalidNvLogRedirect());
        }

        const uintptr_t helper = CaveReserve(40);
        if (helper == 0) {
            LOGGING("NvLogRedirect: cave unavailable (cave=%lx size=%lx)",
                    static_cast<unsigned long>(g_pcv_cave), static_cast<unsigned long>(g_pcv_cave_size));
            R_THROW(ldr::ResultInvalidNvLogRedirect());
        }
        u32 *t = reinterpret_cast<u32 *>(helper);
        size_t n = 0;
        auto emit = [&](u32 ins) { t[n] = ins; ++n; };

        emit(AsmMakeSubImm64(31, 31, 0x200));
        emit(AsmMakeStpImm64(0, 1, 31, 0x100));
        emit(AsmMakeStpImm64(2, 3, 31, 0x110));
        emit(AsmMakeStpImm64(4, 5, 31, 0x120));
        emit(AsmMakeStpImm64(6, 7, 31, 0x130));
        emit(AsmMakeStpqImm(0, 1, 31, 0x140));
        emit(AsmMakeStpqImm(2, 3, 31, 0x160));
        emit(AsmMakeStpqImm(4, 5, 31, 0x180));
        emit(AsmMakeStpqImm(6, 7, 31, 0x1A0));
        emit(AsmMakeStrImm64(30, 31, 0x1E0));
        emit(AsmMakeAddImm64(9, 31, 0x200)); emit(AsmMakeStrImm64(9, 31, 0x1C0)); /* __stack  */
        emit(AsmMakeAddImm64(9, 31, 0x140)); emit(AsmMakeStrImm64(9, 31, 0x1C8)); /* __gr_top */
        emit(AsmMakeAddImm64(9, 31, 0x1C0)); emit(AsmMakeStrImm64(9, 31, 0x1D0)); /* __vr_top */
        emit(AsmMakeMovnW(9, 0x37)); emit(AsmMakeStrImm32(9, 31, 0x1D8)); /* __gr_offs = -56  */
        emit(AsmMakeMovnW(9, 0x7F)); emit(AsmMakeStrImm32(9, 31, 0x1DC)); /* __vr_offs = -128 */
        emit(AsmMakeAddImm64(0, 31, 0x00)); /* mov x0,sp (buf)  */
        emit(AsmMakeMovzW(1, 0x100)); /* size = 0x100     */
        emit(AsmMakeLdrImm64(2, 31, 0x100)); /* fmt (saved x0)   */
        emit(AsmMakeAddImm64(3, 31, 0x1C0)); /* ap               */
        emit(AsmMakeBl(helper + n * 4, vsnprintf_addr));
        emit(AsmMakeMovReg(1, 0)); /* len = retval     */
        emit(AsmMakeCmpImm32(1, 0x100));
        { const size_t at = n; emit(AsmMakeBCond(helper + at * 4, helper + (at + 2) * 4, 0x3u)); } /* b.lo +2 */
        emit(AsmMakeMovzW(1, 0xFF)); /* clamp len        */
        emit(AsmMakeAddImm64(0, 31, 0x00)); /* mov x0,sp (str)  */
        emit(AsmMakeSvc(0x27)); /* svcOutputDebugString */
        emit(AsmMakeLdrImm64(30, 31, 0x1E0));
        emit(AsmMakeAddImm64(31, 31, 0x200));
        emit(RetIns);

        /* Redirect the call sites as patching the actual function causes crash */
        const uintptr_t roStart = g_pcv_cave + g_pcv_cave_size; /* module .rodata start */
        size_t patchedSites = 0;
        if (HOC_PCV_NVLOG_PATCH) {
            for (u32 *p = nsoStart; reinterpret_cast<uintptr_t>(p + 1) <= textEnd; ++p) {
                if (!AsmIsBl(*p)) {
                    continue;
                }
                const uintptr_t pc = reinterpret_cast<uintptr_t>(p);
                if (AsmBranchTarget(*p, pc) != nvlog_addr) {
                    continue;
                }
                bool isFmtCall = false;
                for (u32 j = 1; j <= 8 && reinterpret_cast<uintptr_t>(p - j) >= reinterpret_cast<uintptr_t>(nsoStart); ++j) {
                    const u32 w = *(p - j);
                    if (AsmIsAdrp(w) && asm_get_rd(w) == 0) { /* adrp x0,<page> */
                        const uintptr_t wpc = pc - j * 4;
                        const uintptr_t tgtPage = (wpc & ~static_cast<uintptr_t>(0xFFFu)) + static_cast<uintptr_t>(AsmAdrpPageOffset(w));
                        if (tgtPage >= (roStart & ~static_cast<uintptr_t>(0xFFFu))) { isFmtCall = true; break; }
                    }
                }
                if (isFmtCall) {
                    PATCH_OFFSET(p, AsmMakeBl(pc, helper));
                    ++patchedSites;
                }
            }
        }

        LOGGING("NvLogRedirect: stub@+%lx vsnprintf@+%lx helper@+%lx instr=%zu sites=%zu",
                nvlog_addr - mapped_nso, vsnprintf_addr - mapped_nso, helper - mapped_nso, n, patchedSites);
        R_SUCCEED();
    }
    #endif

    /* Relocate C2/C3Bus to avoid issues*/
    Result BusFreqReloc(u32 *ptr) {
        const u32 busReg  = AsmGetRn(ptr[0]); /* ldr Xbuf,[Xbus,#0x10] : bus struct pointer */
        const u32 bufReg  = asm_get_rd(ptr[0]);  /*                        : freq-buffer arg    */
        const u32 bufOff  = AsmGetLdStImm64Off(ptr[0]); /*                        : bus->freqBuf offset */
        const u32 cntReg  = asm_get_rd(ptr[1]); /* add Xcnt,Xbus,#0x18   : arg2 (&count)       */
        const u32 railReg = asm_get_rd(ptr[2]); /* str Xrail,[Xbus,#0x50]: arg0 (rail)         */
        u32 *call = ptr + 3; /* the bl to relocate                          */
        const uintptr_t realFn = AsmBranchTarget(*call, reinterpret_cast<uintptr_t>(call));

        /* Pick 3 scratch registers */
        u32 s[3], sc = 0;
        for (u32 r = 9; r <= 15 && sc < 3; ++r) {
            if (r != busReg && r != bufReg && r != cntReg && r != railReg) {
                s[sc++] = r;
            }
        }
        R_UNLESS(sc == 3, ldr::ResultInvalidBusFreqReloc());

        const uintptr_t tramp = CaveReserve(9);
        R_UNLESS(tramp != 0, ldr::ResultInvalidBusFreqReloc());

        const uintptr_t region = g_pcv_scratch + HocBusFreqBufOffset; /* [0]=counter, +0x10 + i*0x400 = bufs */
        u32 *t = reinterpret_cast<u32 *>(tramp);
        size_t n = 0;
        auto emit = [&](u32 ins) { t[n] = ins; ++n; };
        emit(AsmMakeAdrp(tramp + n * 4, region, s[0])); /* adrp s0,<region>            */
        emit(AsmMakeAddImm64(s[0], s[0], region & 0xFFFu)); /* add  s0,s0,#lo              */
        emit(AsmMakeLdrImm32(s[1], s[0], 0x00)); /* s1 = counter               */
        emit(AsmMakeAddImm64(s[2], s[1], 1)); /* s2 = counter+1             */
        emit(AsmMakeStrImm32(s[2], s[0], 0x00)); /* counter++                  */
        emit(AsmMakeAddImm64(s[0], s[0], 0x10)); /* s0 = region+0x10 (buffers) */
        emit(AsmMakeAddShiftedReg64(bufReg, s[0], s[1], 10)); /* Xbuf = s0 + counter*0x400  */
        emit(AsmMakeStrImm64(bufReg, busReg, bufOff)); /* bus[freqBuf] = Xbuf        */
        emit(AsmMakeB(tramp + n * 4, realFn)); /* tail-call the real function */

        PATCH_OFFSET(call, AsmMakeBl(reinterpret_cast<uintptr_t>(call), tramp));
        const uintptr_t base = reinterpret_cast<uintptr_t>(nsoStart);
        (void) base;
        LOGGING("BusFreqReloc: call@+%lx -> tramp@+%lx realfn@+%lx (bus=x%u buf=x%u off=0x%x scratch=x%u,x%u,x%u)",
                reinterpret_cast<uintptr_t>(call) - base, tramp - base, realFn - base, busReg, bufReg, bufOff, s[0], s[1], s[2]);
        R_SUCCEED();
    }

    #ifdef HOC_UART_LOG
    /* Force GetEffectiveVerbosityLevel to return a non-zero level so all NvLog runs. */
    Result ForceVerbosity(u32 *ptr) {
        PATCH_OFFSET(&ptr[0], AsmMakeMovzW(0, static_cast<u16>(HOC_PCV_FORCE_VERBOSITY))); /* movz w0,#level */
        PATCH_OFFSET(&ptr[1], RetIns);                                                     /* ret            */
        R_SUCCEED();
    }
    #endif

    /* Widen InitDram for a >32-entry EMC DVFS list. Freq array can be dropped to free 264 bytes, relocate the Soc LUT to that space */
    Result EmcSocLutReloc(u32 *ptr) {
        constexpr u32 Window = 48;

        u32 *freqStore = ScanAssembly(ptr - Window, Window, EmcSocFreqStoreAsm, asm_compare_no_rd); /* str x?,[x8,#0x18] */
        u32 *voltStore = ScanAssembly(ptr - Window, Window, EmcSocVoltStoreAsm, asm_compare_no_rd); /* str w?,[x8,#0x48] */
        u32 *readLoad  = ScanAssembly(ptr - Window, Window, EmcSocReadLoadAsm,  asm_compare_no_rd); /* ldr w?,[x9,#0x48] */
        R_UNLESS(freqStore && voltStore && readLoad, ldr::ResultInvalidEmcSocLut());

        u32 *voltBase = voltStore - 2;   /* `add Xb,Xsrc,Xi,LSL#2` (a cmn sits between it and store) */
        R_UNLESS(AsmIsAddShiftedReg64(*voltBase) && asm_get_rd(*voltBase) == AsmGetRn(*voltStore),
                 ldr::ResultInvalidEmcSocLut());

        /* adrp Xl ; add Xl,Xl,#off ; ... ; str Xl,[rail,#0x120] */
        const u32 lutReg  = asm_get_rd(ptr[0]);
        const u32 railReg = AsmGetRn(ptr[0]);
        R_UNLESS(AsmIsAdrp(ptr[-3]) && asm_get_rd(ptr[-3]) == lutReg, ldr::ResultInvalidEmcSocLut());
        R_UNLESS(AsmIsAddImm64(ptr[-2]) && asm_get_rd(ptr[-2]) == lutReg && AsmGetRn(ptr[-2]) == lutReg,
                 ldr::ResultInvalidEmcSocLut());

        const u32 srcBase = AsmGetRn(*voltBase);   /* rail ptr at +0x20 */
        const u32 wBase   = asm_get_rd(*voltBase); /* base reg */
        const u32 wIdx    = AsmGetRm(*voltBase);   /* loop index */

        PATCH_OFFSET(freqStore,     NopIns); /* Unneeded */
        PATCH_OFFSET(voltBase,      AsmMakeLdrImm64(wBase, srcBase, 0x20)); /* ldr Xb,[Xsrc,#0x20] (rail) */
        PATCH_OFFSET(voltStore - 1, AsmMakeAddImm64(wBase, wBase, 0x18));  /* add Xb,Xb,#0x18 (was cmn) */
        PATCH_OFFSET(voltStore,     AsmSetLdStRegOffset(*voltStore, wIdx)); /* str Wv,[Xb,Xi,LSL#2] -> rail+0x18+i*4 */
        PATCH_OFFSET(voltStore + 1, NopIns); /* Unneeded */

        /* rail+0x18 as the socMinLut pointer. */
        PATCH_OFFSET(ptr - 3, NopIns);
        PATCH_OFFSET(ptr - 2, AsmMakeAddImm64(lutReg, railReg, 0x18));/* add Xl,rail,#0x18 */

        /* Drop the abort branch in case of a bad read */
        for (u32 i = 1; i <= 4; ++i) {
            if (AsmIsBCond(readLoad[i])) {
                PATCH_OFFSET(&readLoad[i], NopIns);
                break;
            }
        }
        R_SUCCEED();
    }

    Result EmcDvfsCountLimit(u32 *ptr) {
        R_UNLESS(EmcDvfsCountPatternFn(ptr), ldr::ResultInvalidEmcDvfsCount());

        /* cmp w?,#0x21 -> cmp w?,#EmcDvfsTableEntryCount */
        PATCH_OFFSET(ptr, AsmSubsSetImm12(*ptr, static_cast<u16>(EmcDvfsTableEntryCount)));
        R_SUCCEED();
    }

    Result EmcRateListLimit(u32 *ptr) {
        /* ptr = cmp w?,#0x20 ; ptr[1] = csel w?,w?,w?,lt (w? = min(maxCount, 32)) ; ptr[2] = bl */
        R_UNLESS(EmcRateListPatternFn(ptr), ldr::ResultInvalidEmcRateList());

        /* The csel's Rm holds the 32 cap. */
        const u32 capReg = AsmGetRm(ptr[1]);
        const u32 capMov = AsmMakeMovzW(capReg, 0x20); /* movz w<Rm>,#0x20 */

        u32 *movPtr = nullptr;
        for (u32 i = 1; i <= 16; ++i) {
            if (*(ptr - i) == capMov) {
                movPtr = ptr - i;
                break;
            }
        }
        R_UNLESS(movPtr, ldr::ResultInvalidEmcRateList());

        /* min(maxCount, 32) -> min(maxCount, EmcDvfsTableEntryCount). */
        PATCH_OFFSET(ptr, AsmSubsSetImm12(*ptr, static_cast<u16>(EmcDvfsTableEntryCount)));     /* cmp  w?,#64 */
        PATCH_OFFSET(movPtr, asm_set_imm16(*movPtr, static_cast<u16>(EmcDvfsTableEntryCount))); /* movz w?,#64 */
        R_SUCCEED();
    }

    void MtcGenerateJedecTable() {
        const u32 jedecFreqs[] = { 1866000, 1996000, 2133000, 2400000, 2666000, 2933000, 3200000 };
        constexpr u32 JedecFreqCount = std::size(jedecFreqs);

        for (u32 i = 0; i < JedecFreqCount; ++i) {
            if (jedecFreqs[i] <= C.marikoEmcMaxClock) {
                newEmcList.push_back(jedecFreqs[i]);
            } else {
                break;
            }
        }

        if (newEmcList.back() != C.marikoEmcMaxClock) {
            newEmcList.push_back(static_cast<u32>(C.marikoEmcMaxClock));
        }

        newEmcList.resize(std::min(newEmcList.size(), EmcDvfsTableEntryLimit));
    }

    void MtcGenerate133StepTable() {
        const u32 stepFreqs133[] = { 1733000, 1866000, 2000000, 2133000, 2266000, 2400000, 2533000, 2666000, 2800000, 2933000, 3066000, 3200000, 3333000, 3466000, }; // Avoid rounding issues
        constexpr u32 StepFreqs133Size = std::size(stepFreqs133);

        for (u32 i = 0; i < StepFreqs133Size; ++i) {
            if (stepFreqs133[i] <= C.marikoEmcMaxClock) {
                newEmcList.push_back(stepFreqs133[i]);
            } else {
                break;
            }
        }

        if (newEmcList.back() != C.marikoEmcMaxClock) {
            newEmcList.push_back(static_cast<u32>(C.marikoEmcMaxClock));
        }

        newEmcList.resize(std::min(newEmcList.size(), EmcDvfsTableEntryLimit));
    }

    void MtcGenerate33StepTable() {
        /* ~33.33MHz but rounded*/
        const u32 stepFreqs33[] = {
            1633000, 1666000, 1700000, 1733000, 1766000, 1800000, 1833000, 1866000, 1900000, 1933000,
            1966000, 2000000, 2033000, 2066000, 2100000, 2133000, 2166000, 2200000, 2233000, 2266000,
            2300000, 2333000, 2366000, 2400000, 2433000, 2466000, 2500000, 2533000, 2566000, 2600000,
            2633000, 2666000, 2700000, 2733000, 2766000, 2800000, 2833000, 2866000, 2900000, 2933000,
            2966000, 3000000, 3033000, 3066000, 3100000, 3133000, 3166000, 3200000, 3233000, 3266000,
            3300000, 3333000, 3366000, 3400000, 3433000, 3466000, 3500000,
        };
        constexpr u32 StepFreqs33Size = std::size(stepFreqs33);

        for (u32 i = 0; i < StepFreqs33Size; ++i) {
            if (stepFreqs33[i] <= C.marikoEmcMaxClock) {
                newEmcList.push_back(stepFreqs33[i]);
            } else {
                break;
            }
        }

        if (newEmcList.back() != C.marikoEmcMaxClock) {
            newEmcList.push_back(static_cast<u32>(C.marikoEmcMaxClock));
        }

        newEmcList.resize(std::min(newEmcList.size(), EmcDvfsTableEntryLimit));
    }

    void MtcGenerateFreqTables() {
        newEmcList.clear();
        newEmcList.reserve(EmcDvfsTableEntryCount);
        newEmcList.insert(newEmcList.end(), std::begin(EmcListDefault), std::end(EmcListDefault));

        if (C.marikoEmcMaxClock <= EmcClkOSLimit) {
            return;
        }

        u32 stepRate = 0;
        switch (C.stepMode) {
            case StepMode_33MHz:
                MtcGenerate33StepTable();
                return;
            case StepMode_66MHz:
                stepRate = 66667;
                break;
            case StepMode_100MHz:
                stepRate = 100000;
                break;
            case StepMode_Jedec:
                MtcGenerateJedecTable();
                return;
            case StepMode_133MHz:
                MtcGenerate133StepTable();
                return;
            default:
                stepRate = 66667;
                break;
        }

        constexpr u32 RoundHz = 1000;
        for (u32 stepIndex = 1;; ++stepIndex) {
            u32 newFreq = EmcClkOSLimit + stepIndex * stepRate;
            newFreq     = (newFreq / RoundHz) * RoundHz;
            if (newFreq > C.marikoEmcMaxClock) {
                if (newEmcList.back() != C.marikoEmcMaxClock) {
                    newEmcList.push_back(static_cast<u32>(C.marikoEmcMaxClock));
                }
                break;
            }
            newEmcList.push_back(newFreq);
        }

        newEmcList.resize(std::min(newEmcList.size(), EmcDvfsTableEntryLimit));
    }

    Result VerifyMtcTable(MarikoMtcTable *tableStart, u32 expectedFreq) {
        R_UNLESS(tableStart->rate_khz == expectedFreq,  ldr::ResultInvalidMtcTable());
        R_UNLESS(tableStart->rev      == MTC_TABLE_REV, ldr::ResultInvalidMtcTable());

        R_SUCCEED();
    }

    Result MtcValidateAllTables(MarikoMtcTable *tableStart, const u32 *validationList, u32 tableCount) {
        for (u32 i = 0; i < tableCount; ++i) {
            R_TRY(VerifyMtcTable(&tableStart[i], validationList[i]));
        }

        R_SUCCEED();
    }

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
        if (index < T210b0SdevEmcDvfsTableS4gb03) {
            return index * mariko::MtcFullTableSize;
        }

        /* There are 2 erista mtc tables between T210b0SdevEmcDvfsTableS4gb01 and T210b0SdevEmcDvfsTableS4gb03, so we have to do this adjustment. */
        return mariko::MtcFullTableSize * index + (2 * erista::MtcFullTableSize);
    }

    void PrepareMtcMemoryRegion(u8 *firstTable, MarikoMtcTable *usedTable) {
        memmove(firstTable, usedTable, mariko::MtcFullTableSize);

        /* Clear all other tables. */
        /* 1 erista table is excluded because it's always before firstTable. */
        /* We also exclude the used table obviously. */
        constexpr size_t RemainingRegionSize = (mariko::MtcFullTableSize) * (mariko::MtcFullTableCount - 1) + (erista::MtcFullTableSize * (erista::MtcFullTableCount - 1));
        memset(firstTable + mariko::MtcFullTableSize, 0, RemainingRegionSize);
    }

    void MtcExtendTables(MarikoMtcTable *table) {
        for (u32 i = mariko::MtcTableCountDefault; i < newEmcList.size(); ++i) {
            std::memcpy(&table[i], &table[i - 1], sizeof(MarikoMtcTable));
            table[i].rate_khz = newEmcList[i];
        }
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

        /* Offset to dram id specific mtc table. */
        static const u32 mtcOffset = GetMtcOffset(mtcIndex);

        /* Offset from 1600MHz pointer to 204Mhz table start. */
        constexpr u32 StartAdjustment = offsetof(MarikoMtcTable, rate_khz) + sizeof(MarikoMtcTable) * (mariko::MtcTableCountDefault - 1);
        u8 *startPtr = reinterpret_cast<u8 *>(ptr) - StartAdjustment;
        MarikoMtcTable *table = reinterpret_cast<MarikoMtcTable *>(startPtr + mtcOffset);
        R_TRY(MtcValidateAllTables(table, EmcListDefault, EmcListSizeDefault));

        PrepareMtcMemoryRegion(startPtr, table);
        table = reinterpret_cast<MarikoMtcTable *>(startPtr);

        if (R_FAILED(MtcValidateAllTables(table, EmcListDefault, EmcListSizeDefault))) {
            AbortInvalidMtc("Failed mtc validation");
        }

        if (C.marikoEmcMaxClock <= EmcClkOSLimit) {
            R_SKIP();
        }

        MtcExtendTables(table);

        if (R_FAILED(MtcValidateAllTables(table, newEmcList.data(), newEmcList.size()))) {
            AbortInvalidMtc("Failed mtc validation");
        }

        for (u32 i = mariko::MtcTableCountDefault; i < newEmcList.size(); ++i) {
            MemMtcTableAutoAdjust(&table[i]);
            MemMtcPllmbDivisor(&table[i]);
        }

        R_SUCCEED();
    }

    Result MemFreqDvbTable(u32 *ptr) {
        DvbEntry *default_end = reinterpret_cast<DvbEntry *>(ptr);
        DvbEntry *new_start   = default_end + 1;

        // Validate existing table
        void *mem_dvb_table_head = reinterpret_cast<u8 *>(new_start) - sizeof(EmcDvbTableDefault);
        bool validated           = std::memcmp(mem_dvb_table_head, EmcDvbTableDefault, sizeof(EmcDvbTableDefault)) == 0;
        R_UNLESS(validated, ldr::ResultInvalidDvbTable());

        if (C.marikoEmcMaxClock <= EmcClkOSLimit) {
            R_SKIP();
        }

        s32 max0    = 1050;
        s32 max1    = 1025;
        s32 max2    = 1000;

        if (C.marikoSocVmax && C.marikoSocVmax > 1000) {
            max0 = C.marikoSocVmax;
            max1 = C.marikoSocVmax;
            max2 = C.marikoSocVmax;
        }

        constexpr s32 MinVolt = 637;

        auto ClampVolt = [&](s32 value, s32 max, s32 voltAdd) {
            return std::clamp(value + voltAdd, MinVolt, max);
        };

        auto DvbVolt = [&](s32 zero, s32 one, s32 two, u32 index) {
            const s32 overrideVoltage = C.marikoSocVoltArray[index];
            s32 voltAdd = 25 * C.emcDvbShift;

            if (overrideVoltage) {
                zero = one = two = overrideVoltage;
                voltAdd = 0;
            }

            return std::array<s32, 3>{
                ClampVolt(zero, max0, voltAdd),
                ClampVolt(one,  max1, voltAdd),
                ClampVolt(two,  max2, voltAdd)
            };
        };

        #define DVB(v) \
            static_cast<u32>((v)[0]), \
            static_cast<u32>((v)[1]), \
            static_cast<u32>((v)[2])

        #define DVB_OC(one, zero, two, idx) \
            DVB(DvbVolt(one, zero, two, idx))

        DvbEntry emcDvbOcTableBrackets[] = {
            {  204000, {         637,  637,  637,     }, },
            { 1331200, {         650,  637,  637,     }, },
            { 1600000, {         675,  650,  637,     }, },
            { 1866000, { DVB_OC( 700,  675,  650,  0) }, },
            { 2000000, { DVB_OC( 712,  687,  662,  1) }, },
            { 2133000, { DVB_OC( 725,  700,  675,  2) }, },
            { 2200000, { DVB_OC( 737,  712,  687,  3) }, },
            { 2266000, { DVB_OC( 750,  725,  700,  4) }, },
            { 2333000, { DVB_OC( 762,  737,  712,  5) }, },
            { 2400000, { DVB_OC( 775,  750,  725,  6) }, },
            { 2433000, { DVB_OC( 787,  762,  737,  7) }, },
            { 2466000, { DVB_OC( 800,  775,  750,  8) }, },
            { 2533000, { DVB_OC( 812,  787,  762,  9) }, },
            { 2566000, { DVB_OC( 825,  800,  775, 10) }, },
            { 2600000, { DVB_OC( 837,  812,  787, 11) }, },
            { 2666000, { DVB_OC( 850,  825,  800, 12) }, },
            { 2700000, { DVB_OC( 875,  850,  825, 13) }, },
            { 2733000, { DVB_OC( 887,  862,  837, 14) }, },
            { 2766000, { DVB_OC( 912,  887,  862, 15) }, },
            { 2800000, { DVB_OC( 925,  900,  875, 16) }, },
            { 2833000, { DVB_OC( 937,  912,  887, 17) }, },
            { 2900000, { DVB_OC( 950,  925,  900, 18) }, },
            { 2933000, { DVB_OC( 962,  937,  912, 19) }, },
            { 3000000, { DVB_OC( 975,  950,  925, 20) }, },
            { 3033000, { DVB_OC( 987,  962,  937, 21) }, },
            { 3100000, { DVB_OC(1000,  975,  950, 22) }, },
            { 3133000, { DVB_OC(1025, 1000,  975, 23) }, },
            { 3166000, { DVB_OC(1037, 1012,  987, 24) }, },
            { 3200000, { DVB_OC(1050, 1025, 1000, 25) }, },
            { 3266000, { DVB_OC(1075, 1050, 1025, 26) }, },
            { 3333000, { DVB_OC(1100, 1075, 1050, 27) }, },
            {     ~0u, {                              }, },
        };
        #undef DVB
        #undef DVB_OC

        const size_t dvbCount = std::min(newEmcList.size(), DvbTableCapacity);
        DvbEntry emcDvbTableOc[DvbTableCapacity] = {};

        u32 bracketIndex = 0;
        for (size_t i = 0; i < dvbCount; ++i) {
            const u32 freq = (i == dvbCount - 1) ? static_cast<u32>(newEmcList.back()) : newEmcList[i];
            while (freq >= emcDvbOcTableBrackets[bracketIndex + 1].freq) {
                ++bracketIndex;
            }

            emcDvbTableOc[i].freq = freq;
            std::memcpy(emcDvbTableOc[i].volt, emcDvbOcTableBrackets[bracketIndex].volt, sizeof(emcDvbTableOc[i].volt));
        }

        /* Clear the entire 32-entry region */
        std::memset(mem_dvb_table_head, 0, DvbTableCapacity * sizeof(DvbEntry));
        std::memcpy(mem_dvb_table_head, emcDvbTableOc, dvbCount * sizeof(DvbEntry));

        R_SUCCEED();
    }

    Result MemFreqMax(u32 *ptr) {
        if (C.marikoEmcMaxClock <= EmcClkOSLimit) {
            R_SKIP();
        }

        PATCH_OFFSET(ptr, C.marikoEmcMaxClock);
        R_SUCCEED();
    }

    Result I2cSet_U8(I2cDevice dev, u8 reg, u8 val) {
        struct {
            u8 reg;
            u8 val;
        } __attribute__((packed)) cmd;

        I2cSession _session;
        R_TRY(i2cOpenSession(&_session, dev));

        cmd.reg    = reg;
        cmd.val    = val;
        Result res = i2csessionSendAuto(&_session, &cmd, sizeof(cmd), I2cTransactionOption_All);
        i2csessionClose(&_session);
        return res;
    }

    Result EmcVddqVolt(u32 *ptr) {
        regulator *entry = reinterpret_cast<regulator *>(reinterpret_cast<u8 *>(ptr) - offsetof(regulator, type_2_3.default_uv));

        constexpr u32 uv_step = 5'000;
        constexpr u32 uv_min  = 250'000;

        auto validator = [entry]() {
            R_UNLESS(entry->id               == 2,       ldr::ResultInvalidRegulatorEntry());
            R_UNLESS(entry->type             == 3,       ldr::ResultInvalidRegulatorEntry());
            R_UNLESS(entry->type_2_3.step_uv == uv_step, ldr::ResultInvalidRegulatorEntry());
            R_UNLESS(entry->type_2_3.min_uv  == uv_min,  ldr::ResultInvalidRegulatorEntry());
            R_SUCCEED();
        };

        R_TRY(validator());

        u32 emc_uv = C.marikoEmcVddqVolt;

        if (!emc_uv) {
            R_SKIP();
        }

        if (emc_uv % uv_step) {
            emc_uv = (emc_uv + uv_step - 1) / uv_step * uv_step; // rounding
        }

        PATCH_OFFSET(ptr, emc_uv);

        i2cInitialize();
        Result resultI2C = I2cSet_U8(I2cDevice_Max77812_2, 0x25, (emc_uv - uv_min) / uv_step);
        i2cExit();

        return resultI2C;
    }

    Result MemMtcTableAsm(u32 *ptr) {
        constexpr u32 AddpOffset = 1;
        constexpr u32 BrOffset   = 12;
        constexpr u32 MovOffset  = 10;

        /* Ensure we don't dereference memory before nso start. */
        R_UNLESS(ptr - BrOffset >= nsoStart, ldr::ResultInvalidMtcTablePattern());

        u32 adrp = *(ptr - AddpOffset);
        R_UNLESS(AsmCompareAdrpNoImm(adrp, MtcAdrpAsm), ldr::ResultInvalidMtcTablePattern());

        /* We don't check for matching register because both registers must be x0 in order to pass the previous checks. */
        /* The correct instructions will always be x0 since the mtcTable pointer is returned. */

        /* Pray this does not break. */
        u32 br = *(ptr - BrOffset);
        R_UNLESS(AsmCompareBrNoRd(br, MtcBrAsm), ldr::ResultInvalidMtcTablePattern());

        /* Pray this does not break either. */
        u32 mov = *(ptr - MovOffset);
        R_UNLESS(asm_compare_no_rd(mov, MtcMovAsm), ldr::ResultInvalidMtcTablePattern());

        u8  movRd         = asm_get_rd(mov);
        u32 movCountPatch = asm_set_rd(asm_set_imm16(MtcMovAsm, newEmcList.size()), movRd);

        PATCH_OFFSET(ptr - BrOffset,  NopIns);
        PATCH_OFFSET(ptr - MovOffset, movCountPatch);

        R_SUCCEED();
    }

    Result GetSocSpeedo(u32 &socSpeedo) {
        constexpr u64 FusePhysicalAddress = 0x7000F000;
        u64 virtualAddress                = 0;
        constexpr u64 Size                = 0x1000;

        u64 outSize;
        /* TODO: use svc::QueryMemoryMapping instead. */
        R_TRY(svcQueryMemoryMapping(&virtualAddress, &outSize, FusePhysicalAddress, Size));

        constexpr u32 FuseOffset      = 2048;
        constexpr u32 SocSpeedoOffset = 308;
        socSpeedo                     = *reinterpret_cast<u32 *>(virtualAddress + FuseOffset + SocSpeedoOffset);

        R_SUCCEED();
    }

    u32 GetSocProcessId(u32 socSpeedo) {
        if (socSpeedo <= 1597) {
            return 0;
        }

        if (socSpeedo <= 1708) {
            return 1;
        }

        /* >= 1709. */
        return 2;
    }

    Result SocVoltAsm(u32 *compareSpeedos) {
        constexpr u32 VoltageScanLimit = 10;
        /* Might actually be speedo id. */
        u32 *writeProcessId = ScanAssembly(compareSpeedos, VoltageScanLimit, SocVoltWriteProcessIdAsm, asm_compare_no_rd);
        R_UNLESS(writeProcessId != nullptr, ldr::ResultInvalidSocVoltPattern());
        u8 writeProcessIdRd = asm_get_rd(*writeProcessId);

        /* This writes 1050mV. */
        u32 *writeVoltage = ScanAssembly(writeProcessId, VoltageScanLimit, SocVoltWriteVoltageAsm, asm_compare_no_rd);
        R_UNLESS(writeVoltage != nullptr, ldr::ResultInvalidSocVoltPattern());
        u8 writeVoltageRd = asm_get_rd(*writeVoltage);

        /* A csel instruction is used to select the soc voltage limit register. */
        /* We care about its destination register since that is used for verification. */
        constexpr u32 VoltageSelectScanLimit = 24;
        u32 *selectVoltage                   = ScanAssembly(writeVoltage, VoltageSelectScanLimit, SocVoltSelectRegisterAsm, AsmCompareCselNoReg);
        R_UNLESS(selectVoltage != nullptr, ldr::ResultInvalidSocVoltPattern());
        /* Todo: check rm and rn? */
        u8 selectVoltageRd = asm_get_rd(*selectVoltage);

        /* rdCsel is then multiplied by 1000 to convert to uV. */
        /* This is pretty far down the function. */
        constexpr u32 MultiplierScanLimit = 200;
        u32 *multiplier                   = ScanAssembly(selectVoltage, MultiplierScanLimit, SocVoltMultiplyVoltsAsm, AsmCompareMullNoReg);
        R_UNLESS(multiplier != nullptr, ldr::ResultInvalidSocVoltPattern());
        u8 multiplierRn = AsmGetMullRn(*multiplier);
        u8 multiplierRm = AsmGetMullRm(*multiplier);
        /* One of the two registers has to be rdCsel. */
        R_UNLESS((multiplierRn == selectVoltageRd) || (multiplierRm == selectVoltageRd), ldr::ResultInvalidSocVoltPattern());
        u8 multiplierRd = asm_get_rd(*multiplier);

        /* Subs instruction is then used to verify against absolute limit. */
        u32 limitValidationPattern = AsmSubsSetRn(SocVoltValidateLimitAsm, multiplierRd);
        u32 *limitValidation = ScanAssembly(multiplier, VoltageScanLimit, limitValidationPattern, AsmSubsCompareNoReg);
        R_UNLESS(limitValidation != nullptr, ldr::ResultInvalidSocVoltPattern());

        /* There is a b.gt instruction right after (checks for socVoltageCap < socVoltageMax). */
        u32 *branchToAbort = limitValidation + 1;
        R_UNLESS(AsmCompareBrConNoImm19(*branchToAbort, SocVoltBranchToAbortAsm), ldr::ResultInvalidSocVoltPattern());

        if (!C.marikoSocVmax || C.marikoSocVmax <= 1000) {
            R_SKIP();
        }

        /* Adjust 1598 speedo minimum to ensure it always goes down process id 0 branch. */
        /* 2200 should be high enough :D */
        u32 compareSpeedosPatch = AsmSubsSetImm12(*compareSpeedos, 2200);
        PATCH_OFFSET(compareSpeedos, compareSpeedosPatch);

        u32 socSpeedo = 0;
        R_TRY(GetSocSpeedo(socSpeedo));

        /* Adjust processId from 0 to [process id of switch booting this]. */
        /* We're overwriting the orr instruction entirly. */
        u32 processId           = GetSocProcessId(socSpeedo);
        u32 writeProcessIdPatch = asm_set_rd(asm_set_imm16(SocVoltWriteVoltageAsm, processId), writeProcessIdRd);
        PATCH_OFFSET(writeProcessId, writeProcessIdPatch);

        /* Adjust voltage limit. */
        u32 voltageLimitPatch = asm_set_rd(asm_set_imm16(SocVoltWriteVoltageAsm, C.marikoSocVmax), writeVoltageRd);
        PATCH_OFFSET(writeVoltage, voltageLimitPatch);

        /* Branches to an abort if limits are invalid -- we patch the branch instruction with NOP. */
        PATCH_OFFSET(branchToAbort, NopIns);

        R_SUCCEED();
    }

    Result SocVoltLimit(u32 *ptr) {
        R_UNLESS(!std::memcmp(ptr - SocVoltLimitMaxDefaultIndex, socVoltLimitArray, sizeof(socVoltLimitArray)), ldr::ResultInvalidSocVoltLimit());
        if (!C.marikoSocVmax || C.marikoSocVmax <= SocVoltLimitOfficial) {
            R_SKIP();
        }

        constexpr u32 Step = 25;
        u32 maxVolt = C.marikoSocVmax;
        if (maxVolt % Step) {
            maxVolt = maxVolt / Step * Step; /* Round. */
        }

        u32 volt = SocVoltLimitOfficial;
        for (u32 i = 1; i < DvfsTableEntryCount - SocVoltLimitMaxDefaultIndex && volt < maxVolt; ++i) {
            volt += Step;
            PATCH_OFFSET(ptr + i, volt);
        }

        R_SUCCEED();
    }

    Result EmcRateSessLimit(u32 *ptr) {
        u32 movzI = 0;
        R_UNLESS(EmcRateSessFindClamp(ptr, nullptr, nullptr, &movzI), ldr::ResultInvalidEmcRateList());

        /* Reject cmd11 GetDvfsTable. */
        for (u32 i = 1; i <= 24; ++i) {
            const u32 w = ptr[i];
            if (AsmIsSubX29Imm(w) && AsmGetImm12(w) >= 0x20u) {  /* sub x?,x29,#>=0x20 */
                R_THROW(ldr::ResultInvalidEmcRateList());
            }
        }

        /*  mov x<desc>,x2 */
        u32 descReg = 0xFFu;
        for (u32 i = 1; i <= 24; ++i) {
            if (AsmIsMovReg(ptr[i], 2)) { descReg = asm_get_rd(ptr[i]); break; }
        }
        R_UNLESS(descReg != 0xFFu, ldr::ResultInvalidEmcRateList());

        /* Repoint the duplicated-imm pair */
        u32 *adds[8]; u32 addImm[8]; u32 nAdds = 0;
        for (u32 i = 1; i <= 24 && nAdds < 8; ++i) {
            const u32 w = ptr[i];
            if (AsmIsAddSpImm(w)) {   /* add x?,sp,#imm12 (shift 0) */
                adds[nAdds]   = ptr + i;
                addImm[nAdds] = AsmGetImm12(w);
                ++nAdds;
            }
        }
        u32 patched = 0;
        for (u32 a = 0; a < nAdds; ++a) {
            bool dup = false;
            for (u32 b = 0; b < nAdds; ++b) {
                if (a != b && addImm[a] == addImm[b]) { dup = true; break; }
            }
            if (dup) {
                PATCH_OFFSET(adds[a], AsmMakeLdrImm64(asm_get_rd(*adds[a]), descReg, 0)); /* ldr x?,[x<desc>] */
                ++patched;
            }
        }
        R_UNLESS(patched == 2, ldr::ResultInvalidEmcRateList());

        /* min(maxCount, 32) -> min(maxCount, EmcDvfsTableEntryCount) */
        PATCH_OFFSET(ptr,         AsmSubsSetImm12(*ptr, static_cast<u16>(EmcDvfsTableEntryCount)));           /* cmp  w?,#64 */
        PATCH_OFFSET(ptr + movzI, asm_set_imm16(*(ptr + movzI), static_cast<u16>(EmcDvfsTableEntryCount)));  /* movz w?,#64 */
        R_SUCCEED();
    }

    void Patch(uintptr_t mapped_nso, size_t nso_size) {
        nsoStart = reinterpret_cast<u32 *>(mapped_nso);

        g_pcv_scratch = mapped_nso + nso_size - HocPcvScratchSize;
        g_nso_size    = nso_size;
        g_cave_cursor = g_pcv_cave;   /* start the .text-cave bump allocator (0 if unavailable) */

        MtcGenerateFreqTables();

        u32 CpuCvbDefaultMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(CpuCvbTableDefault)->freq);
        u32 GpuCvbDefaultMaxFreq = static_cast<u32>(GetDvfsTableLastEntry(GpuCvbTableDefault)->freq);

        PatcherEntry<u32> patches[] = {
            { "CPU Freq Vdd",      &CpuFreqVdd,            1, nullptr,  CpuClkOSLimit              },
            { "CPU Freq Table",     CpuFreqCvbTable<true>, 1, nullptr,  CpuCvbDefaultMaxFreq       },
            { "CPU Volt DVFS",     &CpuVoltDVFS,           1, nullptr,  CpuVminOfficial            },
            { "CPU Volt Thermals", &CpuVoltThermals,       1, nullptr,  CpuVminOfficial            },
            { "CPU Volt Dfll",     &CpuVoltDfll,           1, nullptr,  CpuTune0Low                },
            { "GPU Volt DVFS",     &GpuVoltDVFS,           1, nullptr,  GpuVminOfficial            },
            { "GPU Volt Thermals", &GpuVoltThermals,       1, nullptr,  GpuVminOfficial            },
            { "GPU Freq Table",     GpuFreqCvbTable<true>, 1, nullptr,  GpuCvbDefaultMaxFreq       },
            { "GPU Freq Asm",      &GpuFreqMaxAsm,         2,          &GpuMaxClockPatternFn       },
            { "GPU PLL Max",       &GpuFreqPllMax,         1, nullptr,  GpuClkPllMax               },
            { "GPU PLL Limit",     &GpuFreqPllLimit,       4, nullptr,  GpuClkPllLimit             },
            { "MEM Freq Mtc",      &MemFreqMtcTable,       1, nullptr,  EmcClkOSLimit              },
            { "MEM Freq Dvb",      &MemFreqDvbTable,       1, nullptr,  EmcClkOSLimit              },
            { "MEM Freq Max",      &MemFreqMax,            0, nullptr,  EmcClkOSLimit              },
            { "MEM Freq PLLM",     &MemFreqPllmLimit,      2, nullptr,  EmcClkPllmLimit            },
            { "MEM Vddq",          &EmcVddqVolt,           2, nullptr,  EmcVddqDefault             },
            { "MEM Vdd2",          &MemVoltHandler,        2, nullptr,  MemVdd2Default             },
            { "MEM Table Asm",     &MemMtcTableAsm,        1,          &MemMtcGetGetTablePatternFn },
            { "EMC DVFS Count",    &EmcDvfsCountLimit,     1,          &EmcDvfsCountPatternFn      },
            { "EMC SoC LUT",       &EmcSocLutReloc,        1,          &EmcSocLutPatternFn         },
            { "EMC Rate List",     &EmcRateListLimit,      0,          &EmcRateListPatternFn       },
            { "EMC Rate Sess",     &EmcRateSessLimit,      1,          &EmcRateSessPatternFn       },
            { "Bus Freq Reloc",    &BusFreqReloc,          1,          &BusFreqRelocPatternFn      },
            { "SOC Volt Asm",      &SocVoltAsm,            1,          &SocVoltPatternFn           },
            { "SOC Volt Limit",    &SocVoltLimit,          1, nullptr,  SocVoltLimitOfficial       },
            /* Debugging patches */
            #ifdef HOC_UART_LOG
            { "NvLog Redirect",    &NvLogUartRedirect,     1,          &NvLogVsnprintfPatternFn,   0, 0, true },
            { "Force Verbosity",   &ForceVerbosity,        3,          &ForceVerbosityPatternFn,   0, 0, true },
            #endif
        };

        for (uintptr_t ptr = mapped_nso; ptr <= mapped_nso + nso_size - sizeof(MarikoMtcTable); ptr += sizeof(u32)) {
            u32 *ptr32 = reinterpret_cast<u32 *>(ptr);
            for (auto &entry : patches) {
                if (R_SUCCEEDED(entry.SearchAndApply(ptr32))) {
                    break;
                }
            }
        }

        for (auto &entry : patches) {
            LOGGING("%s Count: %zu", entry.description, entry.patched_count);
            if (R_FAILED(entry.CheckResult())) {
                panic::SmcError(panic::Patch);

                CRASH(entry.description);
            }
        }
    }

}
