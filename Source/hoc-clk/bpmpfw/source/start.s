/*
 * Copyright (c) Atmosphere-NX
 *
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

.section    .vectors, "ax", %progbits
.align      3
.global     reset
reset:
    b _start
    b _panic

.section    .text.start, "ax", %progbits
.align      3
.global     _start
.extern     main
_start:
    /* Mask IRQ + FIQ, enter SVC mode. */
    msr cpsr_f, #0xC0
    msr cpsr_cf, #0xD3

    /* Set the stack pointer. */
    ldr sp, =__stack_top

    /* Zero .bss. */
    ldr r0, =__bss_start__
    ldr r1, =__bss_end__
    mov r2, #0
1:
    cmp r0, r1
    bge 2f
    str r2, [r0], #4
    b   1b
2:

    /* Run global constructors. */
    ldr r4, =__init_array_start
    ldr r5, =__init_array_end
3:
    cmp r4, r5
    bge 4f
    ldr r0, [r4], #4
    mov lr, pc
    bx  r0
    b   3b
4:

    bl  main

.global     _panic
_panic:
    ldr r0, =0x70006040 /* UART_B_BASE */
    adr r3, _panic_msg
7:
    ldrb r1, [r3], #1
    cmp  r1, #0
    beq  _panic_hang
8:
    ldr r2, [r0, #0x14] /* UART_LSR */
    tst r2, #0x20       /* UART_LSR_THRE */
    beq 8b
    str r1, [r0]
    b   7b

_panic_msg:
    .asciz "[bpmpfw] PANIC\n"
    .align 2

_panic_hang:
    b _panic_hang
