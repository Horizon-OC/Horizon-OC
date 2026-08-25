OUTPUT_ARCH(arm)
ENTRY(reset)

MEMORY
{
    bpmpfw : ORIGIN = 0x40004000, LENGTH = 32K
}

SECTIONS
{
    PROVIDE(__start__ = ORIGIN(bpmpfw));
    . = __start__;
    __code_start = . ;

    .vectors :
    {
        KEEP (*(.vectors .vectors.*))
        . = ALIGN(8);
    } >bpmpfw

    .text :
    {
        *(.text.unlikely .text.*_unlikely .text.unlikely.*)
        *(.text.exit .text.exit.*)
        *(.text.startup .text.startup.*)
        *(.text.hot .text.hot.*)
        *(.text .stub .text.* .gnu.linkonce.t.*)
        . = ALIGN(8);
    } >bpmpfw

    .init :
    {
        KEEP( *(.init) )
        . = ALIGN(8);
    } >bpmpfw

    .plt :
    {
        *(.plt)
        *(.iplt)
        . = ALIGN(8);
    } >bpmpfw

    .fini :
    {
        KEEP( *(.fini) )
        . = ALIGN(8);
    } >bpmpfw

    . = ALIGN(8);
    __rodata_start = . ;

    .rodata :
    {
        *(.rodata .rodata.* .gnu.linkonce.r.*)
        . = ALIGN(8);
    } >bpmpfw

    .eh_frame_hdr     : { __eh_frame_hdr_start = .; *(.eh_frame_hdr) *(.eh_frame_entry .eh_frame_entry.*) __eh_frame_hdr_end = .; } >bpmpfw
    .eh_frame         : ONLY_IF_RO { KEEP (*(.eh_frame)) *(.eh_frame.*) } >bpmpfw
    .gcc_except_table : ONLY_IF_RO { *(.gcc_except_table .gcc_except_table.*) } >bpmpfw
    .gnu_extab        : ONLY_IF_RO { *(.gnu_extab*) } >bpmpfw

    .hash              : { *(.hash) } >bpmpfw

    . = ALIGN(8);
    __data_start = . ;

    .eh_frame : ONLY_IF_RW { KEEP (*(.eh_frame)) *(.eh_frame.*) } >bpmpfw
    .gcc_except_table : ONLY_IF_RW { *(.gcc_except_table .gcc_except_table.*) } >bpmpfw
    .gnu_extab : ONLY_IF_RW { *(.gnu_extab*) } >bpmpfw
    .exception_ranges : ONLY_IF_RW { *(.exception_ranges .exception_ranges*) } >bpmpfw

    .preinit_array ALIGN(8) :
    {
        PROVIDE (__preinit_array_start = .);
        KEEP (*(.preinit_array))
        PROVIDE (__preinit_array_end = .);
    } >bpmpfw

    .init_array ALIGN(8) :
    {
        PROVIDE (__init_array_start = .);
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        PROVIDE (__init_array_end = .);
    } >bpmpfw

    .fini_array ALIGN(8) :
    {
        PROVIDE (__fini_array_start = .);
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        PROVIDE (__fini_array_end = .);
    } >bpmpfw

    .ctors ALIGN(8) :
    {
        KEEP (*crtbegin.o(.ctors)) /* MUST be first -- GCC requires it */
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*(.ctors))
    } >bpmpfw

    .dtors ALIGN(8) :
    {
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*(.dtors))
    } >bpmpfw

    __got_start__ = .;

    .got            : { *(.got) *(.igot) } >bpmpfw
    .got.plt        : { *(.got.plt)  *(.igot.plt) } >bpmpfw

    __got_end__ = .;

    .data ALIGN(8) :
    {
        *(.data .data.* .gnu.linkonce.d.*)
        SORT(CONSTRUCTORS)
    } >bpmpfw

    __bss_start__ = .;
    .bss ALIGN(8) :
    {
        *(.dynbss)
        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON)
        . = ALIGN(16);
    } >bpmpfw
    __bss_end__ = .;

    __end__ = ABSOLUTE(.) ;

    __stack_top = ORIGIN(bpmpfw) + LENGTH(bpmpfw) - 0x10;

    /DISCARD/ : { *(.group .comment .note .interp .ARM.attributes) }

    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .debug_info    0 : { *(.debug_info) }
    .debug_abbrev  0 : { *(.debug_abbrev) }
    .debug_line    0 : { *(.debug_line) }
    .debug_frame   0 : { *(.debug_frame) }
    .debug_str     0 : { *(.debug_str) }
    .debug_loc     0 : { *(.debug_loc) }
    .debug_macinfo 0 : { *(.debug_macinfo) }
}
