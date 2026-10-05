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

#include <hocclk.h>
#include <switch.h>

namespace hos::clkrst {
    Result SetMinimumVoltageClockRate(ClkrstSession *session, u32 hz);
    Result GetDvfsTable(ClkrstSession *session, u32 *out_rate_table, s32 in_rate_count, u32 *out_voltage_table, s32 in_voltage_count, s32 *out_count);
} // namespace hos::clkrst