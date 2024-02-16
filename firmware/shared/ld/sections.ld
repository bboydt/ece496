/* This script provides sections for an app image. */

OUTPUT_FORMAT("elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    .text : ALIGN(4)
    {
        KEEP(*(.text._start));
        KEEP(*(.text._crt0_init));
        . = ALIGN(4);
        KEEP(*(.text._vector_table));
        *(.text);
    } > FLASH

    .rodata : ALIGN(4)
    {
        *(.rodata);
        . = ALIGN(4);
    } > FLASH

    .data : ALIGN(4)
    {
        PROVIDE(__global_pointer$ = .);
        *(.data);
        . = ALIGN(4);
    } > SRAM AT > FLASH

    .bss : ALIGN(4)
    {
        *(.bss);
        . = ALIGN(4);
    } > SRAM
}

__stack_begin = ORIGIN(SRAM) + LENGTH(SRAM) - 4;

__data_addr = ADDR(.data);
__data_load_addr = LOADADDR(.data);
__data_load_end = LOADADDR(.data) + SIZEOF(.data);

__bss_load_addr = ADDR(.bss);
__bss_load_end = ADDR(.bss) + SIZEOF(.bss);

