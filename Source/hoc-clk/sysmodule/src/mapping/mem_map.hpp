/*
 * Copyright (c) Souldbminer, Lightos and Horizon OC Contributors
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
#include <switch.h>

Result QueryMemoryMapping(u64 *virtaddr, u64 physaddr, u64 size);
Result MapAddress(u64 &va, const u64 &physAddr, const char *name);
Result SmcCopyFromIram(void *dest, uintptr_t src, u32 size);
Result SmcCopyToIram(uintptr_t dest, const void *src, u32 size);
bool IsPatchedExosphere();
u32 SmcReadWriteRegister(u64 phys_addr, u32 mask, u32 value);
