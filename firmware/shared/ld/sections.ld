/* This script provides sections for an app image. */

OUTPUT_FORMAT("elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    .text : ALIGN(4)
    {
        KEEP(*(.text._start));
        KEEP(*(.text.__vector_table));
        *(.text);
    } > FLASH

    .rodata : ALIGN(4)
    {
        *(.rodata);
        . = ALIGN(4);
    } > FLASH

    .bss : ALIGN(4)
    {
        *(.bss);
        *(.priv_bss);
        . = ALIGN(4);
    } > SRAM

    .data : ALIGN(4)
    {
        *(.data);
        *(.priv_data);
        . = ALIGN(4);
    } > SRAM AT > FLASH

}

__stack_pointer = ORIGIN(SRAM) + LENGTH(SRAM) - 4;

__bss_start = ADDR(.bss);
__bss_end = ADDR(.bss) + SIZEOF(.bss);

__data_start = ADDR(.data);
__data_end = ADDR(.data) + SIZEOF(.data);
__data_load_addr = LOADADDR(.data);


