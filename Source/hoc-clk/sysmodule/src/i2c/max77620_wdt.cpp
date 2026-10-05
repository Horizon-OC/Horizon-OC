/*
 *
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

#include "max77620_wdt.hpp"
#include "../file/config.hpp"
#include <lockable_mutex.h>
#include <mutex>
#include "../file/file_utils.hpp"

/* PMIC Registers */
#define MAX77620_REG_CNFGGLBL2  ((u8)0x01)
#define MAX77620_REG_CNFGGLBL3  ((u8)0x02)
#define MAX77620_REG_ONOFFCNFG2 ((u8)0x42)

/* CNFGGLBL2 bits */
#define MAX77620_WDTOFFC       ((u8)(1u << 4)) /* WDT clear in OFF mode   */
#define MAX77620_WDTSLPC       ((u8)(1u << 3)) /* WDT clear in sleep mode */
#define MAX77620_WDTEN         ((u8)(1u << 2)) /* WDT enabled             */

/* Watchdog timeout period (CNFGGLBL2[1:0]) */
#define MAX77620_TWD_MASK      ((u8)0x03)
#define MAX77620_TWD_2s        ((u8)0x00)
#define MAX77620_TWD_16s       ((u8)0x01)
#define MAX77620_TWD_64s       ((u8)0x02)
#define MAX77620_TWD_128s      ((u8)0x03)

/* CNFGGLBL3 watchdog-clear field (WDTC[1:0]). Writing 0x1 disables WDT. */
#define MAX77620_WDTC_MASK     ((u8)0x03)
#define MAX77620_WDTC_KICK     ((u8)0x01)

/* ONOFFCNFG2 bits */
#define MAX77620_ONOFFCNFG2_WD_RST_WK ((u8)(1u << 6))

/* Is this a watchdog or a watchcat, that is the question */

namespace i2c::wdt {

    static bool isWdtEnabled = false;

    static Result update_bits(u8 reg, u8 mask, u8 val) {
        u8 cur;
        Result rc;

        rc = I2cRead_OutU8(I2cDevice_Max77620Pmic, reg, &cur);
        if (R_FAILED(rc))
            return rc;

        cur = (u8)((cur & (u8)~mask) | (val & mask));
        return I2cSet_U8(I2cDevice_Max77620Pmic, reg, cur);
    }

    void Arm(max77620_wdt_time_t timeout) {
        if (isWdtEnabled) {
            return;
        }

        file::utils::LogLine("[Watchdog] armed");

        Result rc;
        u8 twd;

        if ((int)timeout < (int)MAX77620_WDT_2S || (int)timeout > (int)MAX77620_WDT_128S)
            return;

        twd = (u8)timeout;

        /* WDT expiry results in a restart */
        rc = update_bits(MAX77620_REG_ONOFFCNFG2,
                        MAX77620_ONOFFCNFG2_WD_RST_WK,
                        MAX77620_ONOFFCNFG2_WD_RST_WK);
        ASSERT_RESULT_OK(rc, "update_bits");

        /* Clear WDT in OFF and sleep modes. */
        rc = update_bits(MAX77620_REG_CNFGGLBL2,
                        (u8)(MAX77620_WDTOFFC | MAX77620_WDTSLPC),
                        (u8)(MAX77620_WDTOFFC | MAX77620_WDTSLPC));
        ASSERT_RESULT_OK(rc, "update_bits");

        /* Do what the kernel dows */
        rc = update_bits(MAX77620_REG_CNFGGLBL3,
                        MAX77620_WDTC_MASK, MAX77620_WDTC_KICK);
        ASSERT_RESULT_OK(rc, "update_bits");

        /* Program the period. */
        rc = update_bits(MAX77620_REG_CNFGGLBL2, MAX77620_TWD_MASK, twd);
        ASSERT_RESULT_OK(rc, "update_bits");

        /* Enable. */
        rc = update_bits(MAX77620_REG_CNFGGLBL2, MAX77620_WDTEN, MAX77620_WDTEN);
        ASSERT_RESULT_OK(rc, "update_bits");

        isWdtEnabled = true;
    }

    void Disarm() {
        if (!isWdtEnabled) {
            return;
        }
        file::utils::LogLine("[Watchdog] disarmed");

        Result rc = update_bits(MAX77620_REG_CNFGGLBL2, MAX77620_WDTEN, (u8)0x00);
        ASSERT_RESULT_OK(rc, "update_bits");

        isWdtEnabled = false;
    }

    void ResetWdtEnableState() {

        isWdtEnabled = false;
    }

    bool IsWdtEnabled() {
        return isWdtEnabled;
    }

    void Pet() {
        const bool watchdogEnabled = file::config::GetConfigValue(HocClkConfigValue_Watchdog);

        if (watchdogEnabled) {
            Arm(i2c::wdt::MAX77620_WDT_2S);
            Result rc = update_bits(MAX77620_REG_CNFGGLBL3, MAX77620_WDTC_MASK, MAX77620_WDTC_KICK);
            ASSERT_RESULT_OK(rc, "update_bits");
        } else {
            Disarm();
        }
    }

} // namespace i2c::wdt
