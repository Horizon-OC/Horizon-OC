/*
 * Copyright (C) hanai3bi (meha) and Souldbminer
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

#define CUST_REV 2

#include "oc_common.hpp"
#include "pcv/pcv_common.hpp"

namespace ams::ldr::oc {

#include "mtc_timing_table.hpp"

enum MtcConfig: u32 {
    AUTO_ADJ_ALL = 0,
    CUSTOM_ADJ_ALL = 1,
    NO_ADJ_ALL = 2,
    
    CUSTOMIZED_ALL = 4,
};

using CustomizeCpuDvfsTable = pcv::cvb_entry_t[pcv::DvfsTableEntryLimit];
using CustomizeGpuDvfsTable = pcv::cvb_entry_t[pcv::DvfsTableEntryLimit];
static_assert(sizeof(CustomizeCpuDvfsTable) == sizeof(CustomizeGpuDvfsTable));
static_assert(sizeof(CustomizeCpuDvfsTable) == sizeof(pcv::cvb_entry_t) * pcv::DvfsTableEntryLimit);

constexpr uint32_t ERISTA_MTC_MAGIC = 0x43544D45; // EMTC
constexpr uint32_t MARIKO_MTC_MAGIC = 0x43544D4D; // MMTC

typedef struct CustomizeTable {
    u8  cust[4] = {'C', 'U', 'S', 'T'};
    u32 custRev;
    u32 mtcConf;
    u32 commonCpuBoostClock;
    u32 commonEmcMemVolt;

    // Erista CPU/EMC
    u32 eristaCpuMaxVolt;
    u32 eristaEmcMaxClock;

    // Mariko CPU/EMC
    u32 marikoCpuMaxVolt;
    u32 marikoEmcMaxClock;
    u32 marikoEmcVddqVolt;
    u32 marikoCpuUV;
    u32 marikoGpuUV;
    u32 commonGpuVoltOffset;
    u32 marikoCpuHighVoltOffset;
    u32 marikoCpuHighUV;

    u32 cpuMaxFreq;
    u32 gpuMaxFreq;
    u32 gpuVmax;
    u32 gpuVmin;

    u32 marikoEmcDvbShift;

    // RAM timings (u32)
    u32 latency;
    u32 BL;
    u32 tRFCpb;
    u32 tRFCab;
    u32 tRAS;
    u32 tRPpb;
    u32 tRPab;
    u32 tRC;
    u32 tWTR;
    u32 tWR;
    u32 tR2REF;
    u32 tRCD;
    u32 tREFpb;
    u32 tMRWCKEL;
    u32 tSR;
    u32 tFAW;

    // RAM timings (double)
    double tDQSCK_min;
    double tDQSCK_max;
    double tWPRE;
    double tRPST;
    double tDQSS_max;
    double tDQS2DQ_max;
    double tDQSQ;
    double tRTP;
    double tRRD;
    double tXP;
    double tCMDCKE;
    double tCKELCS;
    double tCSCKEH;
    double tXSR;
    double tCKE;
    double tCKCKEH;

    // Mariko GPU voltages
    u32 marikoGpuVoltArray[24];

    
    CustomizeCpuDvfsTable eristaCpuDvfsTable;
    CustomizeCpuDvfsTable marikoCpuDvfsTable;
    CustomizeCpuDvfsTable marikoCpuDvfsTableSLT;
    CustomizeGpuDvfsTable eristaGpuDvfsTable;
    CustomizeGpuDvfsTable marikoGpuDvfsTable;
    CustomizeGpuDvfsTable marikoGpuDvfsTableSLT;
    CustomizeGpuDvfsTable marikoGpuDvfsTableHiOPT;
    CustomizeGpuDvfsTable marikoGpuDvfsTableUv3;
    //EristaMtcTable* eristaMtcTable;
    //MarikoMtcTable* marikoMtcTable;
    u32 eristaCPUvMax;
    u32 marikoCPUvMax;
    u32 MemVltMax;
    u32 marikoVDDQMax;
    u8 marikoB3;

} CustomizeTable;
//static_assert(sizeof(CustomizeTable) == sizeof(u8) * 4 + sizeof(u32) * 10 + sizeof(CustomizeCpuDvfsTable) * 5 + sizeof(void*) * 2);
//static_assert(sizeof(CustomizeTable) == 7000);

extern volatile CustomizeTable C;

//extern volatile EristaMtcTable EristaMtcTablePlaceholder;
//extern volatile MarikoMtcTable MarikoMtcTablePlaceholder;

}
