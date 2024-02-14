/* This script provides sections for a bootable image. */

OUTPUT_FORMAT("elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    .text : ALIGN(4)
    {
        KEEP(*(.text._start));
        KEEP(*(.text._crt0_init));
        KEEP(*(.text._trap_entry));
        *(.text);
        . = ALIGN(4);
    } > ROM

    .rodata : ALIGN(4)
    {
        *(.rodata);
        . = ALIGN(4);
    } > ROM

    .data : ALIGN(4)
    {
        PROVIDE(__global_pointer$ = .);
        *(.data);
        . = ALIGN(4);
    } > BRAM AT > ROM

    .bss : ALIGN(4)
    {
        *(.bss);
        . = ALIGN(4);
    } > BRAM
}

__stack_begin = ORIGIN(RAM) + LENGTH(RAM) - 4;

__data_addr = ADDR(.data);
__data_load_addr = LOADADDR(.data);
__data_load_end = LOADADDR(.data) + SIZEOF(.data);

__bss_load_addr = ADDR(.bss);
__bss_load_end = ADDR(.bss) + SIZEOF(.bss);
