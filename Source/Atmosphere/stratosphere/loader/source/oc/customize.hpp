/*
 * Copyright (C) Switch-OC-Suite
 *
 * Copyright (c) 2023 hanai3Bi
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

#pragma once

#define CUST_REV 6
#define KIP_VERSION 250

#include "oc_common.hpp"
#include "pcv/pcv_common.hpp"

namespace ams::ldr::hoc {

enum TableConfig: u32 {
    DEFAULT_TABLE = 1,
    TBREAK_1581   = 2,
    TBREAK_1683   = 3,
    EXTREME_TABLE = 4,
};

enum StepMode: u32 {
    StepMode_66MHz  = 0,
    StepMode_100MHz = 1,
    StepMode_Jedec  = 2,
    StepMode_133MHz = 3,
};

enum ReadLatency: u32 {
    RL_2133 = 40,
    RL_1866 = 36,
    RL_1600 = 32,
    RL_1331 = 28,
};

enum WriteLatency: u32 {
    WL_2133 = 18,
    WL_1866 = 16,
    WL_1600 = 14,
    WL_1331 = 12,
};

using CustomizeCpuDvfsTable = pcv::cvb_entry_t[pcv::DvfsTableEntryLimit];
using CustomizeGpuDvfsTable = pcv::cvb_entry_t[pcv::DvfsTableEntryLimit];
static_assert(sizeof(CustomizeCpuDvfsTable) == sizeof(CustomizeGpuDvfsTable));
static_assert(sizeof(CustomizeCpuDvfsTable) == sizeof(pcv::cvb_entry_t) * pcv::DvfsTableEntryLimit);

struct CustomizeTable {
    u8  cust[4]    = {'C', 'U', 'S', 'T'};
    u32 custRev    = CUST_REV;
    u32 kipVersion = KIP_VERSION;

    u32 hpMode;

    u32 commonEmcMemVolt;
    u32 eristaEmcMaxClock;

    StepMode stepMode;
    u32 marikoEmcMaxClock;
    u32 marikoEmcVddqVolt;
    s32 emcDvbShift;
    u32 marikoSocVmax;
    // advanced config
    u32 t1_tRCD;
    u32 t2_tRP;
    u32 t3_tRAS;
    u32 t4_tRRD;
    u32 t5_tRFC;
    u32 t6_tRTW;
    u32 t7_tWTR;
    u32 t8_tREFI;

    u32 timingEmcTbreak;
    u32 low_t1_tRCD;
    u32 low_t2_tRP;
    u32 low_t3_tRAS;
    u32 low_t4_tRRD;
    u32 low_t5_tRFC;
    u32 low_t6_tRTW;
    u32 low_t7_tWTR;
    u32 low_t8_tREFI;

    u32 readLatency[4];
    u32 writeLatency[4];

    u32 eristaCpuUV;
    u32 eristaCpuVmin;
    u32 eristaCpuMaxVolt;
    u32 eristaCpuUnlock;

    u32 marikoCpuUVLow;
    u32 marikoCpuUVHigh;
    u32 tableConf;
    u32 marikoCpuLowVmin;
    u32 marikoCpuHighVmin;
    u32 marikoCpuMaxVolt;
    u32 marikoCpuMaxClock;

    u32 eristaCpuBoostClock;
    u32 marikoCpuBoostClock;

    u32 eristaGpuUV;
    u32 eristaGpuVmin;

    u32 marikoGpuUV;
    u32 marikoGpuVmin;
    u32 marikoGpuVmax;

    s32 commonGpuVoltOffset;

    u32 eristaGpuVoltArray[27];
    u32 marikoGpuVoltArray[24];
    s32 marikoSocVoltArray[28];

    u32 fineTune_t6_tRTW;
    u32 fineTune_t7_tWTR;

    u32 reserved[60];

    CustomizeCpuDvfsTable eristaCpuDvfsTable;
    CustomizeCpuDvfsTable eristaCpuDvfsTableSLT;

    CustomizeCpuDvfsTable marikoCpuDvfsTable;
    CustomizeCpuDvfsTable marikoCpuDvfsTableSLT;
    CustomizeCpuDvfsTable marikoCpuDvfsTable1581Tbreak;
    CustomizeCpuDvfsTable marikoCpuDvfsTable1683Tbreak;
    CustomizeCpuDvfsTable marikoCpuDvfsTableExtreme;

    CustomizeGpuDvfsTable eristaGpuDvfsTable;
    CustomizeGpuDvfsTable eristaGpuDvfsTableSLT;
    CustomizeGpuDvfsTable eristaGpuDvfsTableHiOPT;

    CustomizeGpuDvfsTable marikoGpuDvfsTable;
    CustomizeGpuDvfsTable marikoGpuDvfsTableSLT;
    CustomizeGpuDvfsTable marikoGpuDvfsTableHiOPT;
    CustomizeGpuDvfsTable marikoGpuDvfsTableHiOPT15;
    CustomizeGpuDvfsTable marikoGpuDvfsTableHighUV;

};

extern volatile CustomizeTable C;

}
