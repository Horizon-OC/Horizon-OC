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

#include <hocclk.h>
#include <switch.h>
#include "i2cDrv.h"
#include "../file/errors.hpp"

namespace HocI2c::wdt {

    typedef enum {
        MAX77620_WDT_2S   = 0, /* ~2   seconds */
        MAX77620_WDT_16S  = 1, /* ~16  seconds */
        MAX77620_WDT_64S  = 2, /* ~64  seconds */
        MAX77620_WDT_128S = 3, /* ~128 seconds */
    } max77620_wdt_time_t;

    void Arm(max77620_wdt_time_t timeout);
    void Disarm();

    void Pet();

    void ResetWdtEnableState();
    bool IsWdtEnabled();
} // namespace HocI2c::wdt
