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

#include <stdio.h>
#include <string.h>

#include "libc_platform.hpp"
#include "regs.hpp"

#include <hocclk/bpmp.h>

namespace {

    HocClkBpmpSharedInfo &SharedInfo() {
        return *reinterpret_cast<HocClkBpmpSharedInfo *>(WorkRamStart);
    }

} // namespace

extern "C" void main() {
    InitializeLibc();

    printf("[hoc-bpmpfw] Starting bpmpfw\n");

    HocClkBpmpSharedInfo &info = SharedInfo();
    memset(&info, 0, sizeof(info));
    info.status = 0;
    info.magic = HOCCLK_BPMP_MAGIC; // written last

    for (;;) {

    }
}
