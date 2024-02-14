MEMORY
{
    /* ROM and SRAM are implemented as block rams */
    ROM   : ORIGIN = 0x00000000, LENGTH = 32K
    SRAM  : ORIGIN = 0x00004000, LENGTH = 16K
    /* RAM is the DRAM located on the board. We only use 1GB of the 8GB. */
    RAM   : ORIGIN = 0x80000000, LENGTH = 1024M
    /* FLASH is the spiflash. The DFU bootloader only gives firmware a 4MB partition. */
    FLASH : ORIGIN = 0xE0000000, LENGTH = 4M
}

