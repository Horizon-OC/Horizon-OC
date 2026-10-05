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

#include <cstring>
#include <fuse.h>
#include <switch.h>

#include "board.hpp"
#include "board_fuse.hpp"


namespace board {

    void SetGpuBracket(u16 speedo, u8 &gpuBracket) {
        switch (speedo) {
            // Mariko
            case 1300 ... 1624:
                gpuBracket = 0;
                break;
            case 1625 ... 1689:
                gpuBracket = 1;
                break;
            case 1690 ... 1753:
                gpuBracket = 2;
                break;
            case 1754 ... 1849:
                gpuBracket = 3;
                break;

            // Erista
            case 1850 ... 1925:
                gpuBracket = 0;
                break;
            case 1926 ... 2025:
                gpuBracket = 1;
                break;
            case 2026 ... 2100:
                gpuBracket = 2;
                break;
            case 2101 ... 2200:
                gpuBracket = 3;
                break;
            default:
                gpuBracket = 0;
                break;
        }
    }

    void ReadFuses(FuseData &speedo, u64 fuseVa) {
        constexpr u32 FuseOffset = 0x800;
        u8 *fusePtr = reinterpret_cast<u8 *>(fuseVa) + FuseOffset;

        speedo.cpuSpeedo = *reinterpret_cast<u16 *>(fusePtr + FUSE_CPU_SPEEDO_0_CALIB);
        speedo.gpuSpeedo = *reinterpret_cast<u16 *>(fusePtr + FUSE_CPU_SPEEDO_2_CALIB);
        speedo.socSpeedo = *reinterpret_cast<u16 *>(fusePtr + FUSE_SOC_SPEEDO_0_CALIB);
        speedo.cpuIDDQ = *reinterpret_cast<u16 *>(fusePtr + FUSE_CPU_IDDQ_CALIB) * 4;
        speedo.gpuIDDQ = *reinterpret_cast<u16 *>(fusePtr + FUSE_GPU_IDDQ_CALIB) * 5;
        speedo.socIDDQ = *reinterpret_cast<u16 *>(fusePtr + FUSE_SOC_IDDQ_CALIB) * 4;
        speedo.waferX = *reinterpret_cast<s16 *>(fusePtr + FUSE_OPT_X_COORDINATE);
        speedo.waferY = *reinterpret_cast<s16 *>(fusePtr + FUSE_OPT_Y_COORDINATE);
        speedo.waferX = (speedo.waferX & BIT(8)) ? (speedo.waferX - 512) : speedo.waferX;
    }

}  // namespace board
