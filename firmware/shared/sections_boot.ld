/* This script provides sections for a bootable image. */

OUTPUT_FORMAT("elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_start)

SECTIONS
{
    .text : ALIGN(4)
    {
        KEEP(*(.text._start));
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
        __data_start = .;
        PROVIDE(__global_pointer$ = .);
        *(.data);
        . = ALIGN(4);
        __data_end = .;
    } > RAM AT > ROM

    .bss : ALIGN(4)
    {
        __bss_start = .;
        *(.bss);
        . = ALIGN(4);
        __bss_end = .;
    } > RAM
}

__stack_begin = ORIGIN(RAM) + LENGTH(RAM) - 4;
__data_load_start = LOADADDR(.data);

