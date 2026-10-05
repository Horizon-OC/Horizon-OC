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

#include "clkrst_ext.hpp"

namespace hos::clkrst {
    /// @brief Sets the minimum voltage for a session's module to the one required for @param hz. Does not work with cpu module
    /// @param session A ClkrstSession pointer
    /// @param hz The hz to set the vmin for
    /// @return Result code
    Result SetMinimumVoltageClockRate(ClkrstSession *session, u32 hz) {
        return serviceDispatchIn(&session->s, 9, hz);
    }
    
    Result GetDvfsTable(ClkrstSession *session, u32 *out_rate_table, s32 in_rate_count, u32 *out_voltage_table, s32 in_voltage_count, s32 *out_count) {
        const struct {
            s32 rate_count;
            s32 voltage_count;
        } in = { in_rate_count, in_voltage_count };

        s32 out = 0;
        Result rc = serviceDispatchInOut(&session->s, 11, in, out,
            .buffer_attrs = {
                SfBufferAttr_Out | SfBufferAttr_HipcAutoSelect,
                SfBufferAttr_Out | SfBufferAttr_HipcAutoSelect,
            },
            .buffers = {
                { out_rate_table,    in_rate_count    * sizeof(u32) },
                { out_voltage_table, in_voltage_count * sizeof(u32) },
            },
        );

        if (R_SUCCEEDED(rc) && out_count) {
            *out_count = out;
        }

        return rc;
    }
} // namespace hos::clkrst
