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

/* --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#include <hocclk.h>
#include <i2c.h>
#include <max17050.h>
#include <switch.h>
#include <tmp451.h>

#include "../hos/apm_ext.h"
#include "board.hpp"
#include <ipc_server.h>
#include <lockable_mutex.h>

namespace board {

    HocClkProfile GetProfile() {
        u32 mode = 0;
        Result rc = apmExtGetPerformanceMode(&mode);
        ASSERT_RESULT_OK(rc, "apmExtGetPerformanceMode");

        if (mode) {
            return HocClkProfile_Docked;
        }

        PsmChargerType chargerType;

        rc = psmGetChargerType(&chargerType);
        ASSERT_RESULT_OK(rc, "psmGetChargerType");

        if (chargerType == PsmChargerType_EnoughPower) {
            return HocClkProfile_HandheldChargingOfficial;
        } else if (chargerType == PsmChargerType_LowPower) {
            return HocClkProfile_HandheldChargingUSB;
        }

        return HocClkProfile_Handheld;
    }

}  // namespace board
