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

#include <battery.h>
#include <cmath>
#include <hocclk.h>
#include <i2c.h>
#include <max17050.h>
#include <pwm.h>
#include <switch.h>
#include <tmp451.h>

#include "../file/config.hpp"
#include "../hos/apm_ext.h"
#include "../i2c/i2cDrv.h"
#include "../soc/dram_mrr.hpp"
#include "../tsensor/aotag.hpp"
#include "../tsensor/bq24193.hpp"
#include "../tsensor/soctherm.hpp"
#include "board.hpp"
#include <ipc_server.h>
#include <lockable_mutex.h>


namespace board {

    namespace {
        /* Only poll every 10 ticks to avoid stalling EMC too much */
        constexpr u32 DramMr4PollDivisor = 20; // 1 - 20 seconds depending on polling interval
        u32 gDramMr4Tick = DramMr4PollDivisor;
        s32 gDramMr4Millis = 30000; /* 4x refresh default */

        s32 PollDramMr4TempMilli() {
            u8 mr4 = 0;
            if (!soc::mrr::ReadRamMr4(&mr4))
                return 0; /* stock exosphere */

            /* TODO: verify this table manually*/
            /* Higher tREFI multiplier = slower refresh and vice versa */
            switch (mr4 & 0x7)
            {
                case 0x0: return 0;      /* below operating range */
                case 0x1: return 30000;  /* 4x refresh */
                case 0x2: return 50000;  /* 2x refresh */
                case 0x3: return 85000;  /* 1x refresh */
                case 0x4: return 95000;  /* 0.5x refresh */
                case 0x5: return 100000; /* 0.25x refresh */
                case 0x6: return 105000; /* 0.25x refresh, de-rated */
                default:  return 110000; /* above operating range */
            }
        }
    }

    s32 GetTemperatureMilli(HocClkThermalSensor sensor) {
        s32 millis = 0;
        BatteryChargeInfo info;

        tsensor::TSensorTemps temps = {};
        tsensor::ReadTSensors(temps);

        switch (sensor) {
            case HocClkThermalSensor_SOC: {
                millis = tmp451TempSoc();
                break;
            }
            case HocClkThermalSensor_PCB: {
                millis = tmp451TempPcb();
                break;
            }
            case HocClkThermalSensor_Skin: {
                if (HOSSVC_HAS_TC) {
                    Result rc;
                    rc = tcGetSkinTemperatureMilliC(&millis);
                    ASSERT_RESULT_OK(rc, "tcGetSkinTemperatureMilliC");
                }
                break;
            }
            case HocClkThermalSensor_Battery: {
                batteryInfoGetChargeInfo(&info);
                millis = batteryInfoGetTemperatureMiliCelsius(&info);
                break;
            }
            case HocClkThermalSensor_PMIC: {
                millis = 50000;
                break;
            }
            case HocClkThermalSensor_CPU: {
                millis = temps.cpu;
                break;
            }
            case HocClkThermalSensor_GPU: {
                millis = temps.gpu;
                break;
            }
            case HocClkThermalSensor_MEM: {
                if (board::GetSocType() == HocClkSocType_Mariko && tsensor::IsInitialized() && tsensor::ReadAotag() > 0) {
                    if(board::GetConsoleType() == HocClkConsoleType_Aula) { // Aula has a misplaced thermal sensor that makes tBoard report too high
                        millis = (tsensor::ReadAotag() * 0.40f) + (gDramMr4Millis * 0.60f);
                    } else { // On other consoles it's placed correctly so avoid relying on AOTAG
                        millis = (tsensor::ReadAotag() * 0.20f) + (tmp451TempPcb() * 0.40f) + (gDramMr4Millis * 0.40f); 
                    }
                } else {
                    millis = board::GetSocType() == HocClkSocType_Mariko ? temps.pllx : temps.mem;
                }
                break;
            }
            case HocClkThermalSensor_PLLX: {
                millis = temps.pllx;
                break;
            }
            case HocClkThermalSensor_BQ24193: {
                millis = bq24193::getBQTemp();
                break;
            }
            case HocClkThermalSensor_AO: {
                millis = tsensor::ReadAotag();
                break;
            }
            case HocClkThermalSensor_DRAM: {
                if (++gDramMr4Tick >= DramMr4PollDivisor) {
                    gDramMr4Tick = 0;
                    gDramMr4Millis = PollDramMr4TempMilli();
                }
                millis = gDramMr4Millis;
                break;
            }
            default: {
                ASSERT_ENUM_VALID(HocClkThermalSensor, sensor);
            }
        }

        return std::max(0, millis);
    }

    s32 GetPowerMw(HocClkPowerSensor sensor) {
        switch (sensor) {
            case HocClkPowerSensor_Now:
                return max17050PowerNow();
            case HocClkPowerSensor_Avg:
                return max17050PowerAvg();
            default:
                ASSERT_ENUM_VALID(HocClkPowerSensor, sensor);
        }

        return 0;
    }

}  // namespace board
