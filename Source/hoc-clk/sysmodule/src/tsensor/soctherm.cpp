/*
 * Copyright (c) 2014 - 2019, NVIDIA CORPORATION.  All rights reserved.
 *
 * Author:
 *	Mikko Perttunen <mperttunen@nvidia.com>
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
 *
 */

#include <hocclk.h>
#include <switch.h>

#include "../board/board.hpp"
#include "../bpmp/bpmp.hpp"
#include "../file/file_utils.hpp"
#include "../mapping/mem_map.hpp"
#include "soctherm.hpp"
#include "tsensor_common.hpp"


namespace tsensor {

    namespace {

#define CAR_CLK_SOURCE_TSENSOR 0x3B8
#define CAR_CLK_OUT_ENB_V 0x360

#define CLK_RST_CONTROLLER_RST_DEVICES 0xC
#define SWR_SOC_THERM_RST 1 << 14

#define CLK_RST_CONTROLLER_CLK_OUT_ENB 0x18
#define CLK_ENB_SOC_THERM 1 << 14

#define CAR_CLK_SOURCE_TSENSOR_VAL 0x8000005E

#define THERMCTL_LEVEL0_GROUP_CPU 0x0
#define THERMCTL_LEVEL0_GROUP_GPU 0x4
#define THERMCTL_LEVEL0_GROUP_MEM 0x8
#define THERMCTL_LEVEL0_GROUP_TSENSE 0xc

#define TEGRA124_SOCTHERM_SENSOR_CPU 0
#define TEGRA124_SOCTHERM_SENSOR_MEM 1
#define TEGRA124_SOCTHERM_SENSOR_GPU 2
#define TEGRA124_SOCTHERM_SENSOR_PLLX 3
#define TEGRA124_SOCTHERM_SENSOR_NUM 4

#define TEGRA_SOCTHERM_THROT_LEVEL_NONE 0
#define TEGRA_SOCTHERM_THROT_LEVEL_LOW 1
#define TEGRA_SOCTHERM_THROT_LEVEL_MED 2
#define TEGRA_SOCTHERM_THROT_LEVEL_HIGH 3

#define SENSOR_CONFIG2 8
#define SENSOR_CONFIG2_THERMA_MASK (0xffffu << 16)
#define SENSOR_CONFIG2_THERMB_MASK 0xffff

#define THERMCTL_THERMTRIP_CTL 0x80

#define SENSOR_PDIV 0x1c0
#define SENSOR_PDIV_CPU_MASK (0xf << 12)
#define SENSOR_PDIV_GPU_MASK (0xf << 8)
#define SENSOR_PDIV_MEM_MASK (0xf << 4)
#define SENSOR_PDIV_PLLX_MASK (0xf << 0)

#define SENSOR_HOTSPOT_OFF 0x1c4
#define SENSOR_HOTSPOT_CPU_MASK (0xff << 16)
#define SENSOR_HOTSPOT_GPU_MASK (0xff << 8)
#define SENSOR_HOTSPOT_MEM_MASK (0xff << 0)

#define SENSOR_HW_PLLX_OFFSET_EN 0x1e4
#define SENSOR_HW_PLLX_OFFSET_MEM_EN_MASK BIT(2)
#define SENSOR_HW_PLLX_OFFSET_CPU_EN_MASK BIT(1)
#define SENSOR_HW_PLLX_OFFSET_GPU_EN_MASK BIT(0)

#define SENSOR_HW_PLLX_OFFSET_MIN 0x1e8
#define SENSOR_HW_PLLX_OFFSET_MAX 0x1ec
#define SENSOR_HW_PLLX_OFFSET_MEM_MASK (0xff << 16)
#define SENSOR_HW_PLLX_OFFSET_GPU_MASK (0xff << 8)
#define SENSOR_HW_PLLX_OFFSET_CPU_MASK (0xff << 0)

#define SENSOR_TEMP1 0x1c8
#define SENSOR_TEMP1_CPU_TEMP_MASK (0xffffu << 16)
#define SENSOR_TEMP1_GPU_TEMP_MASK 0xffff
#define SENSOR_TEMP2 0x1cc
#define SENSOR_TEMP2_MEM_TEMP_MASK (0xffffu << 16)
#define SENSOR_TEMP2_PLLX_TEMP_MASK 0xffff

#define SENSOR_VALID 0x1e0
#define SENSOR_GPU_VALID_MASK BIT(9)
#define SENSOR_CPU_VALID_MASK 0xf
#define SENSOR_MEM_VALID_MASK (0x3 << 10)

#define TEGRA210_THERMTRIP_ANY_EN_MASK (0x1u << 31)
#define TEGRA210_THERMTRIP_MEM_EN_MASK (0x1 << 30)
#define TEGRA210_THERMTRIP_GPU_EN_MASK (0x1 << 29)
#define TEGRA210_THERMTRIP_CPU_EN_MASK (0x1 << 28)
#define TEGRA210_THERMTRIP_TSENSE_EN_MASK (0x1 << 27)
#define TEGRA210_THERMTRIP_GPUMEM_THRESH_MASK (0x1ff << 18)
#define TEGRA210_THERMTRIP_CPU_THRESH_MASK (0x1ff << 9)
#define TEGRA210_THERMTRIP_TSENSE_THRESH_MASK 0x1ff

#define TEGRA210_THERM_IRQ_MEM_MASK (0x3 << 24)
#define TEGRA210_THERM_IRQ_GPU_MASK (0x3 << 16)
#define TEGRA210_THERM_IRQ_CPU_MASK (0x3 << 8)
#define TEGRA210_THERM_IRQ_TSENSE_MASK (0x3 << 0)

#define TEGRA210_THERMCTL_LVL0_UP_THRESH_MASK (0x1ff << 18)
#define TEGRA210_THERMCTL_LVL0_DN_THRESH_MASK (0x1ff << 9)

#define TEGRA210_THRESH_GRAIN 500
#define TEGRA210_BPTT 9

#define FUSE_TSENSOR_CALIB_CP_TS_BASE_MASK 0x1fff

#define SENSOR_CONFIG0 0
#define SENSOR_CONFIG0_STOP BIT(0)
#define SENSOR_CONFIG0_CPTR_OVER BIT(2)
#define SENSOR_CONFIG0_OVER BIT(3)
#define SENSOR_CONFIG0_TCALC_OVER BIT(4)
#define SENSOR_CONFIG0_TALL_MASK (0xfffff << 8)
#define SENSOR_CONFIG0_TALL_SHIFT 8

#define PDIV_RATE_T210B0 0xCC0C
#define PDIV_RATE_T210 0x8888
#define HOTSPOT_VAL 0xA0500

#define SENSOR_CONFIG1 4
#define SENSOR_CONFIG1_TSAMPLE_MASK 0x3ff
#define SENSOR_CONFIG1_TSAMPLE_SHIFT 0
#define SENSOR_CONFIG1_TIDDQ_EN_MASK (0x3f << 15)
#define SENSOR_CONFIG1_TIDDQ_EN_SHIFT 15
#define SENSOR_CONFIG1_TEN_COUNT_MASK (0x3f << 24)
#define SENSOR_CONFIG1_TEN_COUNT_SHIFT 24
#define SENSOR_CONFIG1_TEMP_ENABLE BIT(31)

#define READBACK_VALUE_MASK 0xff00
#define READBACK_VALUE_SHIFT 8
#define READBACK_ADD_HALF BIT(7)
#define READBACK_NEGATE BIT(0)

#define PDIV_MASK_T210B0 0xFFFF00F0
#define HOTSPOT_MASK_T210B0 0xFF0000FF

#define PDIV_MASK_T210 0xFFFF0000
#define HOTSPOT_MASK_T210 0xFF000000

#define TSENSOR_TSENSOR_CLKEN 0x1DC
#define TSENSOR_TSENSOR_ENABLE 225

        const TSensorFuse tfuse = {
            .fuse_base_cp_mask = 0x3ff << 11,
            .fuse_base_cp_shift = 11,
            .fuse_base_ft_mask = 0x7ffu << 21,
            .fuse_base_ft_shift = 21,
            .fuse_shift_ft_mask = 0x1f << 6,
            .fuse_shift_ft_shift = 6,
            .fuse_spare_realignment = 0,
        };

        const TSensorConfig eristaConf = {
            .tall = 16300,
            .tiddq_en = 1,
            .ten_count = 1,
            .pdiv = 8,
            .pdiv_ate = 8,
            .tsample = 120,
            .tsample_ate = 480,
        };

        const TSensorConfig marikoConf = {
            .tall = 16300,
            .tiddq_en = 1,
            .ten_count = 1,
            .pdiv = 12,
            .pdiv_ate = 6,
            .tsample = 240,
            .tsample_ate = 480,
        };

        const struct TSensorGroup tSensorGroupCpu = {
            .name = "cpu",
            .id = TEGRA124_SOCTHERM_SENSOR_CPU,
            .sensor_temp_offset = SENSOR_TEMP1,
            .sensor_temp_mask = SENSOR_TEMP1_CPU_TEMP_MASK,
            .pdiv_mask = SENSOR_PDIV_CPU_MASK,
            .pllx_hotspot_diff = 10,
            .pllx_hotspot_mask = SENSOR_HOTSPOT_CPU_MASK,
            .hw_pllx_offset_mask = SENSOR_HW_PLLX_OFFSET_CPU_MASK,
            .hw_pllx_offset_en_mask = SENSOR_HW_PLLX_OFFSET_CPU_EN_MASK,
            .thermtrip_enable_mask = TEGRA210_THERMTRIP_CPU_EN_MASK,
            .thermtrip_any_en_mask = TEGRA210_THERMTRIP_ANY_EN_MASK,
            .thermtrip_threshold_mask = TEGRA210_THERMTRIP_CPU_THRESH_MASK,
            .thermctl_lvl0_offset = THERMCTL_LEVEL0_GROUP_CPU,
            .thermctl_isr_mask = TEGRA210_THERM_IRQ_CPU_MASK,
            .thermctl_lvl0_up_thresh_mask = TEGRA210_THERMCTL_LVL0_UP_THRESH_MASK,
            .thermctl_lvl0_dn_thresh_mask = TEGRA210_THERMCTL_LVL0_DN_THRESH_MASK,
        };

        const struct TSensorGroup tSensorGroupGpu = {
            .name = "gpu",
            .id = TEGRA124_SOCTHERM_SENSOR_GPU,
            .sensor_temp_offset = SENSOR_TEMP1,
            .sensor_temp_mask = SENSOR_TEMP1_GPU_TEMP_MASK,
            .pdiv_mask = SENSOR_PDIV_GPU_MASK,
            .pllx_hotspot_diff = 5,
            .pllx_hotspot_mask = SENSOR_HOTSPOT_GPU_MASK,
            .hw_pllx_offset_mask = SENSOR_HW_PLLX_OFFSET_GPU_MASK,
            .hw_pllx_offset_en_mask = SENSOR_HW_PLLX_OFFSET_GPU_EN_MASK,
            .thermtrip_enable_mask = TEGRA210_THERMTRIP_GPU_EN_MASK,
            .thermtrip_any_en_mask = TEGRA210_THERMTRIP_ANY_EN_MASK,
            .thermtrip_threshold_mask = TEGRA210_THERMTRIP_GPUMEM_THRESH_MASK,
            .thermctl_lvl0_offset = THERMCTL_LEVEL0_GROUP_GPU,
            .thermctl_isr_mask = TEGRA210_THERM_IRQ_GPU_MASK,
            .thermctl_lvl0_up_thresh_mask = TEGRA210_THERMCTL_LVL0_UP_THRESH_MASK,
            .thermctl_lvl0_dn_thresh_mask = TEGRA210_THERMCTL_LVL0_DN_THRESH_MASK,
        };

        const struct TSensorGroup tSensorGroupPll = {
            .name = "pll",
            .id = TEGRA124_SOCTHERM_SENSOR_PLLX,
            .sensor_temp_offset = SENSOR_TEMP2,
            .sensor_temp_mask = SENSOR_TEMP2_PLLX_TEMP_MASK,
            .pdiv_mask = SENSOR_PDIV_PLLX_MASK,
            .thermtrip_enable_mask = TEGRA210_THERMTRIP_TSENSE_EN_MASK,
            .thermtrip_any_en_mask = TEGRA210_THERMTRIP_ANY_EN_MASK,
            .thermtrip_threshold_mask = TEGRA210_THERMTRIP_TSENSE_THRESH_MASK,
            .thermctl_lvl0_offset = THERMCTL_LEVEL0_GROUP_TSENSE,
            .thermctl_isr_mask = TEGRA210_THERM_IRQ_TSENSE_MASK,
            .thermctl_lvl0_up_thresh_mask = TEGRA210_THERMCTL_LVL0_UP_THRESH_MASK,
            .thermctl_lvl0_dn_thresh_mask = TEGRA210_THERMCTL_LVL0_DN_THRESH_MASK,
        };

        const struct TSensorGroup eristaTSensorGroupMem = {
            .name = "mem",
            .id = TEGRA124_SOCTHERM_SENSOR_MEM,
            .sensor_temp_offset = SENSOR_TEMP2,
            .sensor_temp_mask = SENSOR_TEMP2_MEM_TEMP_MASK,
            .pdiv_mask = SENSOR_PDIV_MEM_MASK,
            .pllx_hotspot_diff = 0,
            .pllx_hotspot_mask = SENSOR_HOTSPOT_MEM_MASK,
            .hw_pllx_offset_mask = SENSOR_HW_PLLX_OFFSET_MEM_MASK,
            .hw_pllx_offset_en_mask = SENSOR_HW_PLLX_OFFSET_MEM_EN_MASK,
            .thermtrip_enable_mask = TEGRA210_THERMTRIP_MEM_EN_MASK,
            .thermtrip_any_en_mask = TEGRA210_THERMTRIP_ANY_EN_MASK,
            .thermtrip_threshold_mask = TEGRA210_THERMTRIP_GPUMEM_THRESH_MASK,
            .thermctl_lvl0_offset = THERMCTL_LEVEL0_GROUP_MEM,
            .thermctl_isr_mask = TEGRA210_THERM_IRQ_MEM_MASK,
            .thermctl_lvl0_up_thresh_mask = TEGRA210_THERMCTL_LVL0_UP_THRESH_MASK,
            .thermctl_lvl0_dn_thresh_mask = TEGRA210_THERMCTL_LVL0_DN_THRESH_MASK,
        };

        const TSensor eristaTSensors[] = {
            {
                .name = "cpu0",
                .base = 0xc0,
                .config = &eristaConf,
                .calib_fuse_offset = 0x198,
                .fuse_corr = {
                    .alpha = 1085000,
                    .beta = 3244200,
                },
                .group = &tSensorGroupCpu,
            }, {
                .name = "cpu1",
                .base = 0xe0,
                .config = &eristaConf,
                .calib_fuse_offset = 0x184,
                .fuse_corr = {
                    .alpha = 1126200,
                    .beta = -67500,
                },
                .group = &tSensorGroupCpu,
            }, {
                .name = "cpu2",
                .base = 0x100,
                .config = &eristaConf,
                .calib_fuse_offset = 0x188,
                .fuse_corr = {
                    .alpha = 1098400,
                    .beta = 2251100,
                },
                .group = &tSensorGroupCpu,
            }, {
                .name = "cpu3",
                .base = 0x120,
                .config = &eristaConf,
                .calib_fuse_offset = 0x22c,
                .fuse_corr = {
                    .alpha = 1108000,
                    .beta = 602700,
                },
                .group = &tSensorGroupCpu,
            }, {
                .name = "gpu",
                .base = 0x180,
                .config = &eristaConf,
                .calib_fuse_offset = 0x254,
                .fuse_corr = {
                    .alpha = 1074300,
                    .beta = 2734900,
                },
                .group = &tSensorGroupGpu,
            }, {
                .name = "pllx",
                .base = 0x1a0,
                .config = &eristaConf,
                .calib_fuse_offset = 0x260,
                .fuse_corr = {
                    .alpha = 1039700,
                    .beta = 6829100,
                },
                .group = &tSensorGroupPll,
            }, {
                .name = "mem0",
                .base = 0x140,
                .config = &eristaConf,
                .calib_fuse_offset = 0x258,
                .fuse_corr = {
                    .alpha = 1069200,
                    .beta = 3549900,
                },
                .group = &eristaTSensorGroupMem,
            }, {
                .name = "mem1",
                .base = 0x160,
                .config = &eristaConf,
                .calib_fuse_offset = 0x25c,
                .fuse_corr = {
                    .alpha = 1173700,
                    .beta = -6263600,
                },
                .group = &eristaTSensorGroupMem,
            },
        };

        const TSensor marikoTSensors[] = {
            {
                .name = "cpu0",
                .base = 0xc0,
                .config = &marikoConf,
                .calib_fuse_offset = 0x198,
                .fuse_corr = {
                    .alpha = 1085000,
                    .beta = 3244200,
                },
                .group = &tSensorGroupCpu,
            }, {
                .name = "cpu1",
                .base = 0xe0,
                .config = &marikoConf,
                .calib_fuse_offset = 0x184,
                .fuse_corr = {
                    .alpha = 1126200,
                    .beta = -67500,
                },
                .group = &tSensorGroupCpu,
            }, {
                .name = "cpu2",
                .base = 0x100,
                .config = &marikoConf,
                .calib_fuse_offset = 0x188,
                .fuse_corr = {
                    .alpha = 1098400,
                    .beta = 2251100,
                },
                .group = &tSensorGroupCpu,
            }, {
                .name = "cpu3",
                .base = 0x120,
                .config = &marikoConf,
                .calib_fuse_offset = 0x22c,
                .fuse_corr = {
                    .alpha = 1108000,
                    .beta = 602700,
                },
                .group = &tSensorGroupCpu,
            }, {
                .name = "gpu",
                .base = 0x180,
                .config = &marikoConf,
                .calib_fuse_offset = 0x254,
                .fuse_corr = {
                    .alpha = 1074300,
                    .beta = 2734900,
                },
                .group = &tSensorGroupGpu,
            }, {
                .name = "pllx",
                .base = 0x1a0,
                .config = &marikoConf,
                .calib_fuse_offset = 0x260,
                .fuse_corr = {
                    .alpha = 1039700,
                    .beta = 6829100,
                },
                .group = &tSensorGroupPll,
            },
        };

        u32 calib[SocthermTSensor_EnumMax] = {};
        u64 socthermVa, carVa, fuseVa;
        bool isMariko;
    }  // namespace

    void EnableSensor(const TSensor *sensor, u32 sensorIdx) {
        u32 val = sensor->config->tall << SENSOR_CONFIG0_TALL_SHIFT;
        WriteReg(socthermVa, sensor->base + SENSOR_CONFIG0, val);

        val = (sensor->config->tsample - 1) << SENSOR_CONFIG1_TSAMPLE_SHIFT;
        val |= sensor->config->tiddq_en << SENSOR_CONFIG1_TIDDQ_EN_SHIFT;
        val |= sensor->config->ten_count << SENSOR_CONFIG1_TEN_COUNT_SHIFT;
        val |= SENSOR_CONFIG1_TEMP_ENABLE;
        WriteReg(socthermVa, sensor->base + SENSOR_CONFIG1, val);
        WriteReg(socthermVa, sensor->base + SENSOR_CONFIG2, calib[sensorIdx]);
    }

    void StartSensors() {
        u32 pdiv, hotspot;

        if (isMariko) {
            for (u32 i = 0; i < std::size(marikoTSensors); ++i) {
                EnableSensor(&marikoTSensors[i], i);
            }

            pdiv = (ReadReg(socthermVa, SENSOR_PDIV) & PDIV_MASK_T210B0) | PDIV_RATE_T210B0;
            hotspot = (ReadReg(socthermVa, SENSOR_HOTSPOT_OFF) & HOTSPOT_MASK_T210B0) | HOTSPOT_VAL;
        } else {
            for (u32 i = 0; i < std::size(eristaTSensors); ++i) {
                EnableSensor(&eristaTSensors[i], i);
            }

            pdiv = (ReadReg(socthermVa, SENSOR_PDIV) & PDIV_MASK_T210) | PDIV_RATE_T210;
            hotspot = (ReadReg(socthermVa, SENSOR_HOTSPOT_OFF) & HOTSPOT_MASK_T210) | HOTSPOT_VAL;
        }

        WriteReg(socthermVa, SENSOR_PDIV, pdiv);
        WriteReg(socthermVa, SENSOR_HOTSPOT_OFF, hotspot);
        WriteReg(socthermVa, TSENSOR_TSENSOR_CLKEN, TSENSOR_TSENSOR_ENABLE);
    }

    void ReadTSensors(TSensorTemps &temps) {
        const HocClkBpmpSharedInfo *bpmpInfo = bpmp::GetSharedInfo();
        if (bpmpInfo->magic != HOCCLK_BPMP_MAGIC) {
            return;
        }

        temps.cpu = bpmpInfo->tempCpu;
        temps.gpu = bpmpInfo->tempGpu;
        temps.pllx = bpmpInfo->tempPllx;
        temps.mem = (board::GetSocType() == HocClkSocType_Erista) ? bpmpInfo->tempMem : bpmpInfo->tempPllx;
    }

    void InitializeSoctherm() {
        isMariko = board::GetSocType() == HocClkSocType_Mariko;

        constexpr u64 SocthermPa = 0x700E2000, FusePa = 0x7000F000, CarPa = 0x60006000;
        R_UNLESS(MapAddress(socthermVa, SocthermPa, "soctherm"));
        R_UNLESS(MapAddress(fuseVa, FusePa, "fuse"));
        R_UNLESS(MapAddress(carVa, CarPa, "car"));

        WriteReg(carVa, CAR_CLK_SOURCE_TSENSOR, CAR_CLK_SOURCE_TSENSOR_VAL);
        SetBits(carVa, CAR_CLK_OUT_ENB_V, 0x10);
        svcSleepThread(2000);

        TSensorSharedCalib sharedCal = {};
        CalcSharedCal(&tfuse, &sharedCal, fuseVa);

        if (isMariko) {
            for (u32 i = 0; i < std::size(marikoTSensors); ++i) {
                CalcTSensorCalib(marikoTSensors[i].config, &sharedCal, &marikoTSensors[i].fuse_corr, &calib[i], marikoTSensors[i].calib_fuse_offset,
                                 fuseVa);
            }
        } else {
            for (u32 i = 0; i < std::size(eristaTSensors); ++i) {
                CalcTSensorCalib(eristaTSensors[i].config, &sharedCal, &eristaTSensors[i].fuse_corr, &calib[i], eristaTSensors[i].calib_fuse_offset,
                                 fuseVa);
            }
        }

        StartSensors();

        fileUtils::LogLine("[Soctherm] Finished init.");
    }

}  // namespace tsensor
