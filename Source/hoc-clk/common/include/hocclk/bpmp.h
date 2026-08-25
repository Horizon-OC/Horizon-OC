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
    u32 reserved[2];
} HocClkBpmpSharedInfo;

static_assert(sizeof(HocClkBpmpSharedInfo) == 0x20);
