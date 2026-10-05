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

#include <switch.h>

namespace board {

    struct FuseData {
        u16 cpuSpeedo;
        u16 gpuSpeedo;
        u16 socSpeedo;

        u16 cpuIDDQ;
        u16 gpuIDDQ;
        u16 socIDDQ;

        s16 waferX;
        s16 waferY;
    };

    void ReadFuses(FuseData &speedo, u64 fuseVa);
    void SetGpuBracket(u16 gpuSpeedo, u8 &gpuBracket);

}  // namespace board
