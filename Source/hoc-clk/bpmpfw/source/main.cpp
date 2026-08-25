#include <stdio.h>

#include "libc_platform.hpp"
#include "regs.hpp"

extern "C" void main() {
    InitializeLibc();

    printf("[hoc-bpmpfw] Starting bpmpfw\n");

    for (;;);
}
