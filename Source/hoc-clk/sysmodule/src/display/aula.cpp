/*
 * Copyright (c) Souldbminer
 *
 * Copyright (c) CtCaer
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

#include "aula.hpp"
#include "common.hpp"

// I *think* HOS changes this in some ways, so look into it more

namespace display {
#define MMIO_REG32(base, off) *(vu32 *)((base) + (off))
#define DSI(off) MMIO_REG32(board::dsiVirtAddr, (off) << 2u)

    void _display_dsi_send_cmd(u8 cmd, u32 param, u32 wait) {
        DSI(DSI_WR_DATA) = (param << 8) | cmd;
        DSI(DSI_TRIGGER) = DSI_TRIGGER_HOST;

        if (wait)
            svcSleepThread(wait * 1000);  // usleep-equivalant
    }

    void SetDisplayColorMode(AulaColorMode mode) {
        if (mode == AulaDisplayColorMode_DoNotOverride)
            return;
        // send display command to change color mode.
        _display_dsi_send_cmd(MIPI_DSI_DCS_SHORT_WRITE_PARAM, MIPI_DCS_PRIV_SM_SET_COLOR_MODE | (mode << 8), 0);
    }

}  // namespace display