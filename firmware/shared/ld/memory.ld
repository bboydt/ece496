MEMORY
{
    /* ROM and SRAM are implemented as block rams */
    ROM   (RX)  : ORIGIN = 0x00000000, LENGTH = 16K
    SRAM  (RW) : ORIGIN = 0x00004000, LENGTH = 16K
    /* RAM is the DRAM located on the board. We only use 1GB of the 8GB. */
    RAM   (RW) : ORIGIN = 0x80000000, LENGTH = 1024M
    /* FLASH is the spiflash. The DFU bootloader allocates firmware a 4MB partition at 0x800000 in the flash. */
    FLASH (RX)  : ORIGIN = 0xE0800000, LENGTH = 4M
}
