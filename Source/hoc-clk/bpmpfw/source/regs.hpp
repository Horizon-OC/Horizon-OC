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
#include "defs.hpp"

#define MMIO32(addr) (*reinterpret_cast<volatile u32 *>(addr))

constexpr u32 FwRamStart    = 0x40004000;
constexpr u32 FwRamEnd      = 0x4000C000;
constexpr u32 WorkRamStart  = FwRamEnd;
constexpr u32 WorkRamEnd    = 0x40010000;

constexpr u32 FlowCtlrBase        = 0x60007000;
constexpr u32 FlowCtlrHaltCopEvents = 0x004;

constexpr u32 HaltModeWaitEvent = (2u << 29);
constexpr u32 HaltMsec          = (1u << 24);
constexpr u32 HaltUsec          = (1u << 25);
constexpr u32 HaltMaxCnt        = 0xFF;

[[maybe_unused]] void usleep(u32 us);
[[maybe_unused]] void msleep(u32 ms);

constexpr u32 TmrBase        = 0x60005000;
constexpr u32 TimerUsCntr1Us = 0x10;

constexpr u32 UartBBase = 0x70006040;
constexpr u32 UartThr   = 0x00;
constexpr u32 UartLsr   = 0x14;
constexpr u32 UartLsrThre = (1u << 5);
constexpr u32 UartLsrTmty = (1u << 6);

extern bool g_uartLoggingEnabled;

void UartPutc(char c);

void UartFlush();

void UartPuts(const char *s);

void UartPutHex32(u32 v);
