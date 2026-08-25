#include <stdio.h>

#include "regs.hpp"

namespace {

    int UartStreamPutc(int c, FILE *) {
        UartPutc(static_cast<char>(c));
        return c;
    }

    FILE g_uartStdout = { nullptr, UartStreamPutc, nullptr, nullptr, nullptr, -1 };

} // namespace

void InitializeLibc() {
    stdout = &g_uartStdout;
}
