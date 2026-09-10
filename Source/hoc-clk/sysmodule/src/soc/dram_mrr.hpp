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
#include <hocclk.h>

namespace soc::mrr {
    typedef enum RealDramModule {
        /* Erista */
        DRAM_HBMGCH = 0,
        DRAM_NLE,
        DRAM_WTC,

        /* Mariko */

        /* Samsung */
        DRAM_AMMGCJ,
        DRAM_AAMGCL,
        DRAM_ABMGCL,

        /* Micron */
        DRAM_WTE,
        DRAM_WTF,
        DRAM_WTB,
        DRAM_WTB_8GB,

        /* SK Hynix */
        DRAM_NME,
        DRAM_NEE,
        DRAM_NEE_8GB,
        DRAM_X267,
        DRAM_COUNT,
    } RealDramModule;

    typedef enum _emc_mr_t
    {
        MR0_FEAT    = 0,
        MR4_TEMP    = 4,
        MR5_MAN_ID  = 5,
        MR6_REV_ID1 = 6,
        MR7_REV_ID2 = 7,
        MR8_DENSITY = 8,
    } emc_mr_t;

    enum
    {
        EMC_CHAN0 = 0,
        EMC_CHAN1 = 1
    };
    
    typedef struct _emc_mr_chip_data_t
    {
        // Device 0.
        u8 rank0_ch0;
        u8 rank0_ch1;

        // Device 1.
        u8 rank1_ch0;
        u8 rank1_ch1;
    } emc_mr_chip_data_t;

    typedef struct _emc_mr_data_t
    {
        emc_mr_chip_data_t chip0;
        emc_mr_chip_data_t chip1;
    } emc_mr_data_t;

    typedef enum DramMfg {
        MFG_Samsung = 1,
        MFG_SKHynix = 6,
        MFG_Micron  = 255,
    } DramMfg;

    typedef struct DramRevisionMap {
        HocClkSocType soc;
        DramMfg mfg;
        u8 major;
        u8 minor;
        u8 density; // GB per die
        u8 densityCount; // number of dies (2 = 4GB, 4 = 8GB)
    } DramRevisionMap;

    extern const DramRevisionMap rev[DRAM_COUNT];

    emc_mr_data_t sdram_read_mrx(emc_mr_t mrx);
    bool IsMrrAvailable();

    bool ReadRamMr4(u8 *mr4);

    u8 IdentifyDramId();
}