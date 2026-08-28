/*
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

#pragma once

#include <stdint.h>
#include "board.h"

#define HOCCLK_BPMP_MAGIC 0x42504D50 // 'BPMP'

typedef enum {
    HocClkBpmpCmd_None            = 0,
    HocClkBpmpCmd_RequestShutdown = 1, // Request BPMP-FW to shut down for sleep mode.
    HocClkBpmpCmd_SetUartEnabled  = 2, // cmdArg1: 0/1.
} HocClkBpmpCmd;

/*
 * Shared with BPMP-FW
 * Any 64 bit data type is not permitted.
 */
typedef struct {
    u32 magic;      // should be HOCCLK_BPMP_MAGIC once bpmpfw is running
    u32 status;     // Reserved for error codes and such
    s32 tempCpu;
    s32 tempGpu;
    s32 tempMem;
    s32 tempPllx;
    u32 freqCpu;    // kHz
    u32 freqMem;    // kHz
    u32 freqMemPll; // kHz
    u32 emcLoadAll;
    u32 emcLoadCpu;
    u32 emcBwAll;
    u32 emcBwCpu;
    u32 emcBwGpu;
    u32 cmd;        // HocClkBpmpCmd; 0 = no command pending
    u32 cmdArg1;
    u32 cmdArg2;
    u32 cmdArg3;
    u32 cmdArg4;
    s32 tempAO;
} HocClkBpmpSharedInfo;

static_assert(sizeof(HocClkBpmpSharedInfo) == 0x50);
