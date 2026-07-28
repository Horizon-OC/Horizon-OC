/*
 * Copyright (c) Lightos_
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

#include "../../oc_common.hpp"
#include "timing_tables.hpp"

namespace ams::ldr::hoc::pcv::mariko {

    const ReplacePatch g_rext_table[] = {
        {2'133'000, 0x1A}, {2'166'000, 0x19}, {2'200'000, 0x19},
        {2'233'000, 0x19}, {2'266'000, 0x1A}, {2'300'000, 0x1B},
        {2'333'000, 0x1B}, {2'366'000, 0x1B}, {2'400'000, 0x1B},
        {2'433'000, 0x1B}, {2'466'000, 0x1B}, {2'500'000, 0x1A},
        {2'533'000, 0x1C}, {2'566'000, 0x1B}, {2'600'000, 0x1B},
        {2'633'000, 0x1B}, {2'666'000, 0x1B}, {2'700'000, 0x1C},
        {2'733'000, 0x1C}, {2'766'000, 0x1D}, {2'800'000, 0x1D},
        {2'833'000, 0x1D}, {2'866'000, 0x1D}, {2'900'000, 0x1D},
        {2'933'000, 0x1C}, {2'966'000, 0x1D}, {3'000'000, 0x1D},
        {3'033'000, 0x1D}, {3'066'000, 0x1D}, {3'100'000, 0x1D},
        {3'133'000, 0x1D}, {3'166'000, 0x1C}, {3'200'000, 0x1C},
    };

    const u32 g_rext_table_size = sizeof(g_rext_table) / sizeof(g_rext_table[0]);

    const ReplacePatch *FindRext() {
        for (u32 i = 0; i < g_rext_table_size; i++) {
            if (g_rext_table[i].freq >= C.marikoEmcMaxClock) {
                return &g_rext_table[i];
            }
        }
        return nullptr;
    }

}
