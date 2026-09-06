/*
 * Copyright (c) CtCaer
 *
 * Copyright (c) Souldbminer
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
 *
 */

#include "dram_mrr.hpp"
#include "../board/board.hpp"

#include <cstring>
#include <switch.h>
#include <unistd.h>

namespace soc::mrr {
    constexpr u64 EmcPhysBase  = 0x7001B000;
    constexpr u64 Emc0PhysBase = 0x7001E000;
    constexpr u64 Emc1PhysBase = 0x7001F000;

    constexpr u32 EmcRegMrr       = 0xEC;
    constexpr u32 EmcRegAdrCfg    = 0x10;
    constexpr u32 EmcRegFbioCfg7  = 0x584;
    constexpr u32 EmcRegEmcStatus = 0x2B4;
    constexpr u32 EmcStatusMrrDivld = (1u << 20);
    constexpr int EmcStatusUpdateTimeout = 100000;

    static bool SmcRead(u64 phys_addr, u32 *out) {
        SecmonArgs args = {};
        args.X[0] = 0xF0000002;
        args.X[1] = phys_addr;
        args.X[2] = 0;
        args.X[3] = 0;
        svcCallSecureMonitor(&args);
        if (args.X[1] == phys_addr) {
            return false;
        }
        *out = static_cast<u32>(args.X[1]);
        return true;
    }

    static bool SmcWrite(u64 phys_addr, u32 value) {
        SecmonArgs args = {};
        args.X[0] = 0xF0000002;
        args.X[1] = phys_addr;
        args.X[2] = 0xFFFFFFFF;
        args.X[3] = value;
        svcCallSecureMonitor(&args);
        return args.X[1] != phys_addr;
    }

    static bool EmcRead(u32 offset, u32 *out)   { return SmcRead(EmcPhysBase + offset, out); }
    static bool EmcWrite(u32 offset, u32 value) { return SmcWrite(EmcPhysBase + offset, value); }
    static bool Emc0Read(u32 offset, u32 *out)  { return SmcRead(Emc0PhysBase + offset, out); }
    static bool Emc1Read(u32 offset, u32 *out)  { return SmcRead(Emc1PhysBase + offset, out); }

    /* 0 = ignore */

    const DramRevisionMap rev[DRAM_COUNT] = {
        /* Erista */
        { .soc = HocClkSocType_Erista, .mfg = MFG_Samsung, .major = 0, .minor = 0, .density = 0, .densityCount = 0 }, /* HB-MGCH */
        { .soc = HocClkSocType_Erista, .mfg = MFG_SKHynix, .major = 0, .minor = 0, .density = 0, .densityCount = 0 }, /* NLE     */
        { .soc = HocClkSocType_Erista, .mfg = MFG_Micron,  .major = 0, .minor = 0, .density = 0, .densityCount = 0 }, /* WT:C    */

        /* Mariko */
        /* Samsung */
        { .soc = HocClkSocType_Mariko, .mfg = MFG_Samsung, .major = 6, .minor = 0x10, .density = 1, .densityCount = 2 }, /* AM-MGCJ (6.10, 4GB) */
        { .soc = HocClkSocType_Mariko, .mfg = MFG_Samsung, .major = 7, .minor = 0,    .density = 1, .densityCount = 2 }, /* AA-MGCL (7.00, 4GB) */
        { .soc = HocClkSocType_Mariko, .mfg = MFG_Samsung, .major = 8, .minor = 0,    .density = 1, .densityCount = 2 }, /* AB-MGCL (8.00, 4GB) */

        /* Micron */
        { .soc = HocClkSocType_Mariko, .mfg = MFG_Micron, .major = 4, .minor = 0x10, .density = 1, .densityCount = 2 }, /* WT:E (4.10, 4GB) */
        { .soc = HocClkSocType_Mariko, .mfg = MFG_Micron, .major = 5, .minor = 0,    .density = 1, .densityCount = 2 }, /* WT:F (5.00, 4GB) */
        { .soc = HocClkSocType_Mariko, .mfg = MFG_Micron, .major = 7, .minor = 0,    .density = 1, .densityCount = 2 }, /* WT:B (7.00, 4GB) */
        { .soc = HocClkSocType_Mariko, .mfg = MFG_Micron, .major = 7, .minor = 0,    .density = 1, .densityCount = 4 }, /* WT:B (7.00, 8GB) */

        /* SK Hynix */
        { .soc = HocClkSocType_Mariko, .mfg = MFG_SKHynix, .major = 4, .minor = 0,  .density = 1, .densityCount = 2 }, /* NME (4.00, 4GB) */
        { .soc = HocClkSocType_Mariko, .mfg = MFG_SKHynix, .major = 6, .minor = 0,  .density = 1, .densityCount = 2 }, /* NEE (6.00, 4GB) */
        { .soc = HocClkSocType_Mariko, .mfg = MFG_SKHynix, .major = 6, .minor = 0,  .density = 1, .densityCount = 4 }, /* NEE (6.00, 8GB) */
        { .soc = HocClkSocType_Mariko, .mfg = MFG_SKHynix, .major = 8, .minor = 0,  .density = 1, .densityCount = 2 }, /* x267 (8.00, 4GB) */
    };

    /* Ported from hekate */

    bool IsMrrAvailable() {
        u32 val = 0;
        return EmcRead(EmcRegAdrCfg, &val);
    }

    static int _sdram_wait_emc_status(u32 reg_offset, u32 bit_mask, bool updated_state, s32 emc_channel)
    {
        for (s32 i = 0; i < EmcStatusUpdateTimeout; i++)
        {
            u32 val = 0;
            bool ok;
            if (emc_channel)
            {
                if (emc_channel != 1)
                    return 1;

                ok = Emc1Read(reg_offset, &val);
            }
            else
            {
                ok = EmcRead(reg_offset, &val);
            }

            if (ok && (((val & bit_mask) != 0) == updated_state))
                return 0;

            usleep(1);
        }

        return 1;
    }

    static bool _sdram_req_mrr_data(u32 data, bool dual_channel)
    {
        if (!EmcWrite(EmcRegMrr, data))
            return false;
        if (_sdram_wait_emc_status(EmcRegEmcStatus, EmcStatusMrrDivld, true, EMC_CHAN0) != 0)
            return false;
        if (dual_channel && _sdram_wait_emc_status(EmcRegEmcStatus, EmcStatusMrrDivld, true, EMC_CHAN1) != 0)
            return false;
        return true;
    }

    struct EmcTopology {
        u32 ranks;
        u32 channels;
    };

    static bool QueryTopology(EmcTopology *topo) {
        u32 tmp = 0;
        if (!EmcRead(EmcRegAdrCfg, &tmp))
            return false;
        topo->ranks = (tmp & 1) + 1;
        if (!EmcRead(EmcRegFbioCfg7, &tmp))
            return false;
        u32 ch = (tmp >> 1) & 3;
        topo->channels = (ch & 1) + ((ch & 2) >> 1);
        return topo->channels != 0;
    }

    static void ClearMrrLeftovers() {
        u32 tmp = 0;
        for (u32 i = 0; i < 16; i++)
        {
            (void)EmcRead(EmcRegMrr, &tmp);
            usleep(1);
        }
    }

    static void StoreMrrBytes(u32 mrr, u8 *lo, u8 *hi) {
        *lo = mrr & 0xFF;
        *hi = (mrr >> 8) & 0xFF;
    }

    static bool ReadMrxRank(emc_mr_t mrx, bool dual_rank, bool dual_channel, emc_mr_data_t *out) {
        memset(out, 0xFF, sizeof(*out));
        u32 mrr = 0;

        // Get Device 0 (Rank 0) info from both dram chips (channels).
        if (!_sdram_req_mrr_data((2u << 30) | (static_cast<u32>(mrx) << 16), dual_channel))
            return false;

        // Ram module 0 info.
        if (!Emc0Read(EmcRegMrr, &mrr))
            return false;
        StoreMrrBytes(mrr, &out->chip0.rank0_ch0, &out->chip0.rank0_ch1);

        // Ram module 1 info.
        if (dual_channel)
        {
            if (!Emc1Read(EmcRegMrr, &mrr))
                return false;
            StoreMrrBytes(mrr, &out->chip1.rank0_ch0, &out->chip1.rank0_ch1);
        }

        // If Rank 1 exists, get info.
        if (dual_rank)
        {
            // Get Device 1 (Rank 1) info from both dram chips (channels).
            if (!_sdram_req_mrr_data((1u << 30) | (static_cast<u32>(mrx) << 16), dual_channel))
                return false;

            // Ram module 0 info.
            if (!Emc0Read(EmcRegMrr, &mrr))
                return false;
            StoreMrrBytes(mrr, &out->chip0.rank1_ch0, &out->chip0.rank1_ch1);

            // Ram module 1 info.
            if (dual_channel)
            {
                if (!Emc1Read(EmcRegMrr, &mrr))
                    return false;
                StoreMrrBytes(mrr, &out->chip1.rank1_ch0, &out->chip1.rank1_ch1);
            }
        }

        return true;
    }

    static bool ReadMrxSet(emc_mr_data_t *vendor, emc_mr_data_t *rev0, emc_mr_data_t *rev1,
                           emc_mr_data_t *density, EmcTopology *topo) {
        EmcTopology t = {};
        if (!QueryTopology(&t))
            return false;
        const bool dual_rank = t.ranks > 1;
        const bool dual_channel = t.channels > 1;

        ClearMrrLeftovers();

        if (!ReadMrxRank(MR5_MAN_ID, dual_rank, dual_channel, vendor))
            return false;
        if (!ReadMrxRank(MR6_REV_ID1, dual_rank, dual_channel, rev0))
            return false;
        if (!ReadMrxRank(MR7_REV_ID2, dual_rank, dual_channel, rev1))
            return false;
        if (!ReadMrxRank(MR8_DENSITY, dual_rank, dual_channel, density))
            return false;

        *topo = t;
        return true;
    }

    emc_mr_data_t sdram_read_mrx(emc_mr_t mrx)
    {
        emc_mr_data_t data;
        memset(&data, 0xFF, sizeof(emc_mr_data_t));

        EmcTopology topo = {};
        if (!QueryTopology(&topo))
            return data;

        ClearMrrLeftovers();

        (void)ReadMrxRank(mrx, topo.ranks > 1, topo.channels > 1, &data);
        return data;
    }

    bool ReadRamMr4(u8 *mr4) {
        EmcTopology topo = {};
        if (!QueryTopology(&topo))
            return false;

        ClearMrrLeftovers();

        emc_mr_data_t data = {};
        if (!ReadMrxRank(MR4_TEMP, topo.ranks > 1, topo.channels > 1, &data))
            return false;

        *mr4 = data.chip0.rank0_ch0;
        return true;
    }

    /* MR8 density field (bits 5:2) to MB */
    static bool DecodeDensityMb(u8 mr8, u32 *mb) {
        switch ((mr8 & 0x3C) >> 2)
        {
            case 2: *mb = 512; break;
            case 3: *mb = 768; break;
            case 4: *mb = 1024; break;
            case 5: *mb = 1536; break;
            case 6: *mb = 2048; break;
            default: return false;
        }
        return true;
    }

    static bool MatchModule(HocClkSocType soc, DramMfg mfg, u8 major, u8 minor,
                            u32 densityMb, u32 densityCount, RealDramModule *mod) {
        for (int i = 0; i < DRAM_COUNT; i++)
        {
            const DramRevisionMap &e = rev[i];
            if (e.soc != soc || e.mfg != mfg)
                continue;
            if (soc != HocClkSocType_Erista)
            {
                if (e.major != major || e.minor != minor)
                    continue;
                if (e.density * 1024u != densityMb)
                    continue;
                if (e.densityCount != densityCount)
                    continue;
            }
            *mod = static_cast<RealDramModule>(i);
            return true;
        }
        return false;
    }

    static u8 FuseIdForModule(RealDramModule mod, HocClkConsoleType console, u8 fuseFallback) {
        switch (mod)
        {
            case DRAM_HBMGCH:
            case DRAM_NLE:
            case DRAM_WTC:
                return fuseFallback;
            case DRAM_AMMGCJ:
                if (console == HocClkConsoleType_Hoag) return 12;
                if (console == HocClkConsoleType_Iowa) return 8;
                return fuseFallback;
            case DRAM_AAMGCL:
                if (console == HocClkConsoleType_Iowa) return 17;
                if (console == HocClkConsoleType_Hoag) return 19;
                if (console == HocClkConsoleType_Aula) return 24;
                return fuseFallback;
            case DRAM_ABMGCL:
                if (console == HocClkConsoleType_Iowa) return 20;
                if (console == HocClkConsoleType_Hoag) return 21;
                if (console == HocClkConsoleType_Aula) return 22;
                return fuseFallback;
            case DRAM_WTE:
                if (console == HocClkConsoleType_Iowa) return 11;
                if (console == HocClkConsoleType_Hoag) return 15;
                return fuseFallback;
            case DRAM_WTF:
                if (console == HocClkConsoleType_Iowa) return 25;
                if (console == HocClkConsoleType_Hoag) return 26;
                if (console == HocClkConsoleType_Aula) return 27;
                return fuseFallback;
            case DRAM_WTB:
                if (console == HocClkConsoleType_Iowa) return 32;
                if (console == HocClkConsoleType_Hoag) return 33;
                if (console == HocClkConsoleType_Aula) return 34;
                return fuseFallback;
            case DRAM_WTB_8GB:
            case DRAM_NEE_8GB:
                /* Used in atmosphere and commonly burnt for 8gb upgrade */
                return 28;
            case DRAM_NME:
                if (console == HocClkConsoleType_Iowa) return 10;
                if (console == HocClkConsoleType_Hoag) return 14;
                return fuseFallback;
            case DRAM_NEE:
                if (console == HocClkConsoleType_Iowa) return 6;
                if (console == HocClkConsoleType_Hoag) return 3;
                if (console == HocClkConsoleType_Aula) return 5;
                return fuseFallback;
            case DRAM_X267:
                if (console == HocClkConsoleType_Iowa) return 29;
                if (console == HocClkConsoleType_Hoag) return 30;
                if (console == HocClkConsoleType_Aula) return 31;
                return fuseFallback;
            default:
                return fuseFallback;
        }
    }

    u8 IdentifyDramId() {
        const u8 fuseId = board::GetFuseDramId();
        const HocClkSocType soc = board::GetSocType();

        if (soc == HocClkSocType_Erista)
            return fuseId;

        if (!IsMrrAvailable())
            return fuseId;

        emc_mr_data_t vendor = {}, rev0 = {}, rev1 = {}, density = {};
        EmcTopology topo = {};
        if (!ReadMrxSet(&vendor, &rev0, &rev1, &density, &topo))
            return fuseId;

        DramMfg mfg = static_cast<DramMfg>(vendor.chip0.rank0_ch0);

        u32 densityMb = 0;
        if (!DecodeDensityMb(density.chip0.rank0_ch0, &densityMb))
            return fuseId;

        RealDramModule mod = DRAM_COUNT;
        if (!MatchModule(soc, mfg, rev0.chip0.rank0_ch0, rev1.chip0.rank0_ch0,
                         densityMb, topo.ranks * topo.channels, &mod))
            return fuseId;

        return FuseIdForModule(mod, board::GetConsoleType(), fuseId);
    }


}