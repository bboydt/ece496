
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
}

__image_base = 0xE0800000;
