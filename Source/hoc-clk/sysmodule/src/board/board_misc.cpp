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

#include <cmath>
#include <pwm.h>
#include <switch.h>

namespace board {

    Thread miscThread;
    u8 fanLevel = 0;
    PwmChannelSession *_iCon;

    void MiscThreadFunc(void *pwmCheckPtr) {
        Result pwmCheck = *static_cast<Result *>(pwmCheckPtr);
        double temp = 0;
        double rotationDuty = 0;

        while (true) {
            if (R_SUCCEEDED(pwmCheck)) {
                if (R_SUCCEEDED(pwmChannelSessionGetDutyCycle(_iCon, &temp))) {
                    temp *= 10;
                    temp = trunc(temp);
                    temp /= 10;
                    rotationDuty = 100.0 - temp;
                }
            }

            fanLevel = static_cast<u8>(rotationDuty);
            svcSleepThread(300'000'000);
        }
    }

    u8 GetFanLevel() {
        return fanLevel;
    }

    void StartMiscThread(Result pwmCheck, PwmChannelSession *iCon) {
        _iCon = iCon;
        threadCreate(&miscThread, MiscThreadFunc, &pwmCheck, NULL, 0x1000, 0x10, 3);
        threadStart(&miscThread);
    }

    void ExitMiscThread() {
        threadClose(&miscThread);
    }

}  // namespace board
