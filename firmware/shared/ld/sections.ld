/* This script provides sections for an app image. */

OUTPUT_FORMAT("elf32-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(_reset_vector)

SECTIONS
{
    .text :
    {
        . = ALIGN(__rx_region_align__);
        __rx_region__ = .;
        KEEP(*(.text._reset_vector));
        KEEP(*(.text._trap_entry));
        *(.text);
        __rx_region_end__ = .;
    } > FLASH
    __rx_region_size__ = __rx_region_end__ - __rx_region__;
    __rx_region_align__ = 4;

    ASSERT(_reset_vector == ORIGIN(FLASH), "_reset_vector must be placed at the beginning of flash")

    .vector_table :
    {
        . = ALIGN(__ro_region_align__);
        __ro_region__ = .;
        KEEP(*(.vector_table.vector_table));
    } > FLASH

    .rodata :
    {
        *(.rodata*)
        . = ALIGN(__ro_region_align__);
        __ro_region_end__ = .;
    } > FLASH

    __ro_region_size__ = __ro_region_end__ - __ro_region__;
    __ro_region_align__ =
        DEFINED(__mpu_align_power2__) ? (1 << LOG2CEIL(__ro_region_size__)) :
        DEFINED(__mpu_align_32__) ? 32 : 0;

    .data :
    {
        . = ALIGN(__rw_region_align__);
        __rw_region__ = .;
        *(.data*)
    } > SRAM AT> FLASH

    .bss (NOLOAD) :
    {
        *(.bss*)
        . = ALIGN(__rw_region_align__);
        __rw_region_end__ = .;
    } > SRAM

    __rw_region_size__ = __rw_region_end__ - __rw_region__;
    __rw_region_align__ =
        DEFINED(__mpu_align_power2__) ? (1 << LOG2CEIL(__rw_region_size__)) :
        DEFINED(__mpu_align_32__) ? 32 : 0;

    .priv_data :
    {
        . = ALIGN(__priv_rw_region_align__);
        __priv_rw_region__ = .;
        *(.priv_data*)
    } > SRAM AT> FLASH

    .priv_bss (NOLOAD) :
    {
        *(.priv_bss*)
        . = ALIGN(__priv_rw_region_align__);
        __priv_rw_region_end__ = .;
    } > SRAM

    __priv_rw_region_size__ = __priv_rw_region_end__ - __priv_rw_region__;
    __priv_rw_region_align__ =
        DEFINED(__mpu_align_power2__) ? (1 << LOG2CEIL(__priv_rw_region_size__)) :
        DEFINED(__mpu_align_32__) ? 32 : 0;

    .stack (NOLOAD) :
    {
        *(.stack*)
    } > SRAM

    .copy_table : ALIGN(4)
    {
        __copy_table__ = .;
        LONG(ADDR(.data))
        LONG(LOADADDR(.data))
        LONG(SIZEOF(.data))
        LONG(ADDR(.priv_data))
        LONG(LOADADDR(.priv_data))
        LONG(SIZEOF(.priv_data))
        __copy_table_end__ = .;
    } > FLASH

    .zero_table : ALIGN(4)
    {
        __zero_table__ = .;
        LONG(ADDR(.bss))
        LONG(SIZEOF(.bss))
        LONG(ADDR(.priv_bss))
        LONG(SIZEOF(.priv_bss))
        __zero_table_end__ = .;
    } > FLASH

    .init_array : ALIGN(4)
    {
        __init_array__ = .;
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array*)))
        __init_array_end__ = .;
    } > FLASH

}

__stack_pointer = ORIGIN(SRAM) + LENGTH(SRAM) - 4;
