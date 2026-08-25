#pragma once
#include <cstdint>

using u8  = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;

#define MMIO32(addr) (*reinterpret_cast<volatile u32 *>(addr))

constexpr u32 FwRamStart    = 0x40004000;
constexpr u32 FwRamEnd      = 0x4000C000;
constexpr u32 WorkRamStart  = FwRamEnd;
constexpr u32 WorkRamEnd    = 0x40010000;

constexpr u32 FlowCtlrBase        = 0x60007000;
constexpr u32 FlowCtlrHaltCopEvents = 0x004;

constexpr u32 HaltModeWaitEvent = (2u << 29);
constexpr u32 HaltUsec          = (0u << 24);

[[maybe_unused]] inline void FlowCtlrHaltUsec(u32 usec) {
    MMIO32(FlowCtlrBase + FlowCtlrHaltCopEvents) = HaltModeWaitEvent | HaltUsec | (usec & 0xFFFFFFu);
}

constexpr u32 UartBBase = 0x70006040;
constexpr u32 UartThr   = 0x00;
constexpr u32 UartLsr   = 0x14;
constexpr u32 UartLsrThre = (1u << 5);

inline void UartPutc(char c) {
    while (!(MMIO32(UartBBase + UartLsr) & UartLsrThre))
        ;
    MMIO32(UartBBase + UartThr) = static_cast<u32>(static_cast<u8>(c));
}

inline void UartPuts(const char *s) {
    while (*s) {
        UartPutc(*s++);
    }
}

inline void UartPutHex32(u32 v) {
    static const char digits[] = "0123456789ABCDEF";
    for (int i = 28; i >= 0; i -= 4) {
        UartPutc(digits[(v >> i) & 0xF]);
    }
}
