/*
 * Copyright (c) Souldbminer, Lightos and Horizon OC Contributors
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
#include "regs.hpp"

constexpr u32 SocthermBase       = 0x700E2000;
constexpr u32 SocthermSensorTemp1 = 0x1C8;
constexpr u32 SocthermSensorTemp2 = 0x1CC;

inline s32 TranslateSocthermTemp(u16 val) {
    s32 t = ((val >> 8) & 0xFF) * 1000;
    if (val & (1u << 7)) {
        t += 500;
    }
    if (val & (1u << 0)) {
        t = -t;
    }
    return t;
}
