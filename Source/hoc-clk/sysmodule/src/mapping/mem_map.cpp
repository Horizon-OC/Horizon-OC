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

#include <switch.h>

#include "../file/file_utils.hpp"

Result QueryMemoryMapping(u64 *virtaddr, u64 physaddr, u64 size) {
    if (hosversionAtLeast(10, 0, 0)) {
        u64 out_size;
        return svcQueryMemoryMapping(virtaddr, &out_size, physaddr, size);
    } else {
        return svcLegacyQueryIoMapping(virtaddr, physaddr, size);
    }
}

Result MapAddress(u64 &va, const u64 &physAddr, const char *name) {
    Result mapResult = QueryMemoryMapping(&va, physAddr, 0x1000);
    if (R_FAILED(mapResult)) {
        fileUtils::LogLine("Failed to map %s! %u", name, R_DESCRIPTION(mapResult));
    }

    return mapResult;
}

Result SmcCopyFromIram(void *dest, uintptr_t src, u32 size) {
    SecmonArgs args;
    args.X[0] = 0xF0000201;
    args.X[1] = (u64)dest;
    args.X[2] = (u64)src;
    args.X[3] = size;
    args.X[4] = 0;
    svcCallSecureMonitor(&args);
    Result rc = 0;
    if (args.X[0] != 0) {
        rc = (26u | ((u32)args.X[0] << 9u));
    }
    return rc;
}

Result SmcCopyToIram(uintptr_t dest, const void *src, u32 size) {
    SecmonArgs args;
    args.X[0] = 0xF0000201;
    args.X[1] = (u64)src;
    args.X[2] = (u64)dest;
    args.X[3] = size;
    args.X[4] = 1;
    svcCallSecureMonitor(&args);
    Result rc = 0;
    if (args.X[0] != 0) {
        rc = (26u | ((u32)args.X[0] << 9u));
    }
    return rc;
}

bool IsPatchedExosphere() {
    constexpr u64 EvpPhysBase          = 0x6000F000;
    constexpr u64 EvpCopResetVector    = 0x200;

    SecmonArgs args = {};
    args.X[0] = 0xF0000002;
    args.X[1] = EvpPhysBase + EvpCopResetVector;
    args.X[2] = 0;
    args.X[3] = 0;
    svcCallSecureMonitor(&args);

    return args.X[0] == 0;
}

u32 SmcReadWriteRegister(u64 phys_addr, u32 mask, u32 value) {
    SecmonArgs args = {};
    args.X[0] = 0xF0000002;
    args.X[1] = phys_addr;
    args.X[2] = mask;
    args.X[3] = value;
    svcCallSecureMonitor(&args);
    return (u32)args.X[1];
}