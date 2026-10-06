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

#include "i2c.h"

#define I2C_CMD_SND 0
#define I2C_CMD_RCV 1

Result i2csessionExtRegReceive(I2cSession *s, u8 in, void *out, u8 out_size) {
    u8 cmdlist[5] = { I2C_CMD_SND | (I2cTransactionOption_Start << 6), sizeof(in), in,

                      I2C_CMD_RCV | (I2cTransactionOption_All << 6), out_size };

    return i2csessionExecuteCommandList(s, out, out_size, cmdlist, sizeof(cmdlist));
}
