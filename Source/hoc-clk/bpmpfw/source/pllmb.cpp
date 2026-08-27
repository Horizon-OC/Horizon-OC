/*
 * Copyright (c) Souldbminer
 * Copyright (c) KazushiMe
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

#include "pllmb.hpp"

namespace pllmb {

    u64 MeasurePtoHz(u32 ptoId, u32 divider, u32 preselReg, u32 preselMask) {
        u32 preselOrigBit = 0;

        if (preselReg) {
            u32 val = MMIO32(freq::CarBase + preselReg);
            usleep(10);
            preselOrigBit = val & preselMask;
            val &= ~preselMask;
            val |= preselMask;
            MMIO32(freq::CarBase + preselReg) = val;
            usleep(10);
        }

        constexpr u32 cycleCount = 16;
        const u32 savedCntl = MMIO32(freq::CarBase + freq::CarPtoClkCntCntl);

        u32 val = freq::PtoDivSelDiv1 | freq::PtoClkEnable | (cycleCount - 1);
        val |= ptoId << freq::PtoSrcSelShift;

        MMIO32(freq::CarBase + freq::CarPtoClkCntCntl) = val;
        usleep(10);
        MMIO32(freq::CarBase + freq::CarPtoClkCntCntl) = val | freq::PtoCntRst;
        usleep(10);
        MMIO32(freq::CarBase + freq::CarPtoClkCntCntl) = val;
        usleep(10);
        MMIO32(freq::CarBase + freq::CarPtoClkCntCntl) = val | freq::PtoCntEn;
        usleep(500);

        while (MMIO32(freq::CarBase + freq::CarPtoClkCntStatus) & PtoCyclePeriod)
            ;

        u32 cnt = MMIO32(freq::CarBase + freq::CarPtoClkCntStatus) & freq::PtoClkCntMask;
        cnt *= divider;

        const u64 rateHz = (static_cast<u64>(cnt) * 32768) / cycleCount;

        usleep(10);
        MMIO32(freq::CarBase + freq::CarPtoClkCntCntl) = savedCntl;
        usleep(10);

        if (preselReg) {
            u32 val2 = MMIO32(freq::CarBase + preselReg);
            usleep(10);
            val2 &= ~preselMask;
            val2 |= preselOrigBit;
            MMIO32(freq::CarBase + preselReg) = val2;
            usleep(10);
        }

        return rateHz;
    }

    u64 MeasureRamClockHz() {
        const u64 pllmbHz = MeasurePtoHz(PtoPllmb, 2, PllmMisc2, PllmMisc2PllmbBit);
        if (pllmbHz != 0) {
            return pllmbHz;
        }
        return MeasurePtoHz(PtoPllm, 2, PllmMisc2, PllmMisc2PllmBit);
    }

} // namespace pllmb
