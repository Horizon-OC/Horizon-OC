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

#include "regs.hpp"

[[maybe_unused]] void usleep(u32 us) {
    while (us) {
        const u32 delay = (us > HaltMaxCnt) ? HaltMaxCnt : us;
        us -= delay;
        MMIO32(FlowCtlrBase + FlowCtlrHaltCopEvents) = HaltModeWaitEvent | HaltUsec | delay;
    }
}

[[maybe_unused]] void msleep(u32 ms) {
    while (ms) {
        const u32 delay = (ms > HaltMaxCnt) ? HaltMaxCnt : ms;
        ms -= delay;
        MMIO32(FlowCtlrBase + FlowCtlrHaltCopEvents) = HaltModeWaitEvent | HaltMsec | delay;
    }
}

bool g_uartLoggingEnabled = true;

void UartPutc(char c) {
    if (!g_uartLoggingEnabled) {
        return;
    }
    while (!(MMIO32(UartBBase + UartLsr) & UartLsrThre))
        ;
    MMIO32(UartBBase + UartThr) = static_cast<u32>(static_cast<u8>(c));
}

void UartFlush() {
    if (!g_uartLoggingEnabled) {
        return;
    }
    while (!(MMIO32(UartBBase + UartLsr) & UartLsrTmty))
        ;
}

void UartPuts(const char *s) {
    while (*s) {
        UartPutc(*s++);
    }
}

void UartPutHex32(u32 v) {
    static const char digits[] = "0123456789ABCDEF";
    for (int i = 28; i >= 0; i -= 4) {
        UartPutc(digits[(v >> i) & 0xF]);
    }
}
