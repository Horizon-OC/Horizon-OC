/*
 * Copyright (c) 2014 - 2017, NVIDIA CORPORATION.  All rights reserved.
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

#include "aotag.hpp"

namespace aotag {

    void Update(HocClkBpmpSharedInfo &info) {
        const u32 regval = MMIO32(PmcBase + PmcTsensorStatus1);

        if (!(regval & Status1ValidBit)) {
            info.tempAO = InvalidSentinel;
            return;
        }

        const u32 abs      = (regval >> Status1AbsShift) & Status1AbsMask;
        const bool frac    = (regval & Status1FracBit) != 0;
        const bool negative = (regval & Status1SignBit) != 0;

        s32 temp = static_cast<s32>(abs) * 1000 + (frac ? 500 : 0);
        if (negative) {
            temp = -temp;
        }

        info.tempAO = temp;
    }

} // namespace aotag
