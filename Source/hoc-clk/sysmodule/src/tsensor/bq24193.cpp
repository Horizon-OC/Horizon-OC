/*
 * Copyright (c) Souldbminer, Lightos_ and Horizon OC Contributors
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

#include "bq24193.hpp"

namespace tsensor {
    static u8 bq24193_get_reg(u8 reg) {
        u8 out;
        I2cRead_OutU8(I2cDevice_Bq24193, reg, &out);
        return out;
    }
    u8 GetBqTemp() {
        u8 regVal = bq24193_get_reg(BQ24193_FaultReg);
        return regVal & BQ24193_FAULT_THERM_MASK;
    }
}  // namespace tsensor
