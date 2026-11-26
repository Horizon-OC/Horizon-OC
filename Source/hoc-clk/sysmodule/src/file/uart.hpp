#pragma once
#include <hocclk.h>
#include <switch.h>
#include "errors.hpp"
namespace file::uart {
    Result InitializeUartLogging(UartPort port, u32 baud_rate);
    void SendUartPacket(void* pkt, u32 pkt_len);
    void CloseUartLogging();
}