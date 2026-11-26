#include "uart.hpp"

/* probably better to move this elsewhere? */
/* not all baud rates are allowed, so ensure your exosphere uses correct baud rate as we configure here */
/* 1M is good (supported 57600, 115200, 1000000 and 3000000), 3M will error out most adapters (trust me, i tried)*/
namespace file::uart {
    namespace {
        UartPortSession uartSession = { 0 };
        /* buffers must be 0x1000 byte aligned */
        constexpr u32 send_buffer_size = 0x1000;
        constexpr u32 rcv_buffer_size  = 0x1000;
        alignas(0x1000) char send_buffer[0x1000] = { 0 };
        alignas(0x1000) char rcv_buffer[0x1000]  = { 0 }; 
    }

    Result InitializeUartLogging(UartPort port, u32 baud_rate) {
        Result rc = uartInitialize();
        ASSERT_RESULT_OK(rc, "uartInitialize");

        rc = uartCreatePortSession(&uartSession);
        ASSERT_RESULT_OK(rc, "uartCreatePortSession");
        /* probably RE HID to figure out how to do this proper? */
        bool success = false;
        /* this will abort if the port is being used currently */
        /* the zero parameter may need tweaking */
        rc = uartPortSessionOpenPort(&uartSession, &success, port, baud_rate, UartFlowControlMode_Hardware, 0, 
            false, false, false, false, send_buffer, send_buffer_size, rcv_buffer, rcv_buffer_size);
        ASSERT_RESULT_OK(rc, "uartPortSessionOpenPort");
        /* why? */
        ASSERT_RESULT_OK((Result)(!success), "uartPortSessionOpenPort");

        /* we should be done here? */
        return rc;
    }

    void SendUartPacket(void* pkt, u32 pkt_len) {
        u64 out_sz = 0;
        uartPortSessionSend(&uartSession, pkt, (size_t)pkt_len, &out_sz);
        (void)out_sz;
    }

    void CloseUartLogging() {
        uartPortSessionClose(&uartSession);
        uartExit();
    }
}