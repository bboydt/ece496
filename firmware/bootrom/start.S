.equ XIP_CTRL, 0xFFFFEF00

.global __image_base

.text
.global _start
_start:

    // Configure SPI Flash
    //
    // 25 MHz SPI Clock | 5-byte data width | Read_1_1_1 (low freq mode)
    // 
    // XIP_CTRL_EN = 1
    // XIP_CTRL_CDIV0 = 0
    // XIP_CTRL_PRSC0 = 0
    // XIP_CPOL = 0
    // XIP_CPHA = 0
    // XIP_CTRL_SPI_NBYTES = 8 (3 address bytes + 1 command + 4 data bytes
    // XIP_CTRL_SPI_ABYTES = 3
    // XIP_CTRL_RD_CMD = 0x03

    // write config into control reg
    li a0, XIP_CTRL
    li a1, 0x00207601
    sw a1, 0(a0)
    
    // jump to flash image
    jump __image_base, t0
