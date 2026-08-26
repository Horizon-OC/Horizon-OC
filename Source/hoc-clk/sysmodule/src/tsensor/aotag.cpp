/*
 * drivers/thermal/tegra_aotag.c
 *
 * TEGRA AOTAG (Always-On Thermal Alert Generator) driver.
 *
 * Copyright (c) 2014 - 2017, NVIDIA CORPORATION.  All rights reserved.
 *
 * Copyright (c) 1994, Linus Torvalds
 *
 * Copyright (c) 2026, Souldbminer
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <notification.h>

#include "../bpmp/bpmp.hpp"
#include "../file/file_utils.hpp"
#include "../mapping/mem_map.hpp"
#include "aotag.hpp"
#include "tsensor_common.hpp"


namespace tsensor {
#define PMC_BASE 0x7000E400
#define TEGRA_FUSE_CP_REV_0_3 (3)
#define FUSE_CP_REV 0x190

    namespace {
        u64 fuseVa = 0;
        bool wasInit = false;
    }  // namespace

#define REG_SET(r, mask, value) ((r & ~(mask##_MASK)) | ((value << (mask##_POS_START)) & mask##_MASK))

#define REG_GET(r, mask) ((r & mask##_MASK) >> mask##_POS_START)

#define FUSE_TSENSOR_CALIB_CP_TS_BASE_MASK 0x1fff

    static const TSensorFuse tegra_aotag_fuse = {
        .fuse_base_cp_mask = 0x3ff << 11,
        .fuse_base_cp_shift = 11,
        .fuse_base_ft_mask = (u32)0x7ff << 21,
        .fuse_base_ft_shift = 21,
        .fuse_shift_ft_mask = 0x1f << 6,
        .fuse_shift_ft_shift = 6,
        .fuse_spare_realignment = 0,
    };

    static TSensorConfig tegra_aotag_config = {
        .tall = 76,
        .tiddq_en = 1,
        .ten_count = 16,
        .pdiv = 8,
        .pdiv_ate = 8,
        .tsample = 9,
        .tsample_ate = 39,
    };

    static TSensorConfig tegra210b01_aotag_config = {
        .tall = 76,
        .tiddq_en = 1,
        .ten_count = 16,
        .pdiv = 12,
        .pdiv_ate = 6,
        .tsample = 19,
        .tsample_ate = 39,
    };

    static const FuseCorrCoeff tegra_aotag_coeff = {
        .alpha = 1063200,
        .beta = -6749000,
    };

    static const FuseCorrCoeff tegra210b01_aotag_coeff = {
        .alpha = 991100,
        .beta = 1096200,
    };

    struct aotag_sensor_info_t {
        struct TSensorConfig *config;
        const struct TSensorFuse *fuse;
        const struct FuseCorrCoeff *coeff;
        s32 therm_a;
        s32 therm_b;
    };

    struct aotag_platform_data {
        struct TSensorConfig *config;
        const struct FuseCorrCoeff *coeff;
    };

    static aotag_platform_data tegra210_plat_data = {
        .config = &tegra_aotag_config,
        .coeff = &tegra_aotag_coeff,
    };

    static aotag_platform_data tegra210b01_plat_data = {
        .config = &tegra210b01_aotag_config,
        .coeff = &tegra210b01_aotag_coeff,
    };

    aotag_sensor_info_t aotag_sensor_info = {
        .config = NULL,
        .fuse = NULL,
        .coeff = NULL,
        .therm_a = 0,
        .therm_b = 0,
    };

    aotag_sensor_info_t *info = &aotag_sensor_info;
    aotag_platform_data *pdata = NULL;

    u32 ReadPmcReg(unsigned long offset) {
        SecmonArgs args = {};
        args.X[0] = 0xF0000002;
        args.X[1] = PMC_BASE + offset;
        svcCallSecureMonitor(&args);

        if (args.X[1] == (PMC_BASE + offset)) {  // if param 1 is identical read failed
            return 0;
        }

        return args.X[1];
    }

    void WritePmcReg(u32 value, unsigned long offset) {
        SecmonArgs args = {};
        args.X[0] = 0xF0000002;
        args.X[1] = PMC_BASE + offset;
        args.X[2] = 0xFFFFFFFF;
        args.X[3] = (value);
        svcCallSecureMonitor(&args);
    }

    static inline void SetBit(unsigned long nr, volatile void *addr) {
        int *m = ((int *)addr) + (nr >> 5);
        *m |= 1 << (nr & 31);
    }

    static inline void ClearBit(unsigned long nr, volatile void *addr) {
        int *m = ((int *)addr) + (nr >> 5);
        *m &= ~(1 << (nr & 31));
    }

    void InitializeAotag(bool isMariko) {
        constexpr u64 FusePa = 0x7000F000;
        R_UNLESS(MapAddress(fuseVa, FusePa, "fuse"));

        if (isMariko) {
            // u32 major, minor, rev;
            pdata = &tegra210b01_plat_data;
            info->config = &tegra210b01_aotag_config;
            // tegra_fuse_readl(FUSE_CP_REV, &rev);
            // minor = rev & 0x1f;
            // major = (rev >> 5) & 0x3f;
            // if (major == 0 && minor < TEGRA_FUSE_CP_REV_0_3) {
            //     info->config->tsample_ate -= 1;
            // }
        } else {
            info->config = &tegra_aotag_config;
            pdata = &tegra210_plat_data;
        }

        info->fuse = &tegra_aotag_fuse;
        info->coeff = pdata->coeff;

        aotag_sensor_info_t *ps_info = &aotag_sensor_info;
        TSensorSharedCalib shared_fuses;
        u32 therm_ab;

        CalcSharedCal(ps_info->fuse, &shared_fuses, fuseVa);
        CalcTSensorCalib(ps_info->config, &shared_fuses, ps_info->coeff, &therm_ab, AOTAG_FUSE_ADDR, fuseVa);

        ps_info->therm_a = REG_GET(therm_ab, CONFIG2_THERM_A);
        ps_info->therm_b = REG_GET(therm_ab, CONFIG2_THERM_B);

        WritePmcReg(therm_ab, PMC_TSENSOR_CONFIG2);

        aotag_sensor_info_t *i = info;

        unsigned long r = 0;
        r = REG_SET(r, CONFIG0_TALL, i->config->tall);
        WritePmcReg(r, PMC_TSENSOR_CONFIG0);

        r = 0;
        r = REG_SET(r, CONFIG1_TEN_COUNT, i->config->ten_count);
        r = REG_SET(r, CONFIG1_TIDDQ_EN, i->config->tiddq_en);
        r = REG_SET(r, CONFIG1_TSAMPLE, (i->config->tsample - 1));
        SetBit(CONFIG1_TEMP_ENABLE_POS, &r);
        WritePmcReg(r, PMC_TSENSOR_CONFIG1);

        r = 0;
        r = REG_SET(r, TSENSOR_PDIV, i->config->pdiv);
        WritePmcReg(r, PMC_TSENSOR_PDIV0);

        r = ReadPmcReg(PMC_AOTAG_CFG);
        SetBit(CFG_TAG_EN_POS, &r);
        ClearBit(CFG_DISABLE_CLK_POS, &r);
        WritePmcReg(r, PMC_AOTAG_CFG);
        fileUtils::LogLine("[aotag] Init complete!");
        wasInit = true;
    }

    s32 ReadAotag() {
        if (!wasInit) {
            return -126;
        }

        return bpmp::GetSharedInfo()->tempAO;
    }

    bool IsInitialized() {
        return wasInit;
    }

}  // namespace tsensor
