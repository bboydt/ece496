.text
.org 0x0
.global _reset_vector
_reset_vector:
    j _start


.org 0x20
// controls flow of all interrupts
_trap_handler:
    
    // store registers
    add sp, sp, -(4*32)
    sw x31, (4*30)(sp)
    sw x30, (4*29)(sp)
    sw x29, (4*28)(sp)
    sw x28, (4*27)(sp)
    sw x27, (4*26)(sp)
    sw x26, (4*25)(sp)
    sw x25, (4*24)(sp)
    sw x24, (4*23)(sp)
    sw x23, (4*22)(sp)
    sw x22, (4*21)(sp)
    sw x21, (4*20)(sp)
    sw x20, (4*19)(sp)
    sw x19, (4*18)(sp)
    sw x18, (4*17)(sp)
    sw x17, (4*16)(sp)
    sw x16, (4*15)(sp)
    sw x15, (4*14)(sp)
    sw x14, (4*13)(sp)
    sw x13, (4*12)(sp)
    sw x12, (4*11)(sp)
    sw x11, (4*10)(sp)
    sw x10, (4*9 )(sp)
    sw x9 , (4*8 )(sp)
    sw x8 , (4*7 )(sp)
    sw x7 , (4*6 )(sp)
    sw x6 , (4*5 )(sp)
    sw x5 , (4*4 )(sp)
    sw x4 , (4*3 )(sp)
    sw x3 , (4*2 )(sp)
    //sw x2 , (4*1 )(sp)
    sw x1 , (4*0 )(sp)
    
    // execute execption handler
    csrr a0, mcause
    jal _execption_handler

    // load registers
    lw x31, (4*30)(sp)
    lw x30, (4*29)(sp)
    lw x29, (4*28)(sp)
    lw x28, (4*27)(sp)
    lw x27, (4*26)(sp)
    lw x26, (4*25)(sp)
    lw x25, (4*24)(sp)
    lw x24, (4*23)(sp)
    lw x23, (4*22)(sp)
    lw x22, (4*21)(sp)
    lw x21, (4*20)(sp)
    lw x20, (4*19)(sp)
    lw x19, (4*18)(sp)
    lw x18, (4*17)(sp)
    lw x17, (4*16)(sp)
    lw x16, (4*15)(sp)
    lw x15, (4*14)(sp)
    lw x14, (4*13)(sp)
    lw x13, (4*12)(sp)
    lw x12, (4*11)(sp)
    lw x11, (4*10)(sp)
    lw x10, (4*9 )(sp)
    lw x9 , (4*8 )(sp)
    lw x8 , (4*7 )(sp)
    lw x7 , (4*6 )(sp)
    lw x6 , (4*5 )(sp)
    lw x5 , (4*4 )(sp)
    lw x4 , (4*3 )(sp)
    lw x3 , (4*2 )(sp)
    //lw x2 , (4*1 )(sp)
    lw x1 , (4*0 )(sp)
    add sp, sp, (4*32)
    
    mret



// entry point from the bootrom
_start:
    // set stack pointer
    la sp, __stack_pointer

    // set trap vector
    la t0, _execption_handler
    csrw mtvec, t0

    // xxx testing for code flow
    li a0, 0
    call neorv32_gpio_pin_set

    // copy sections
    la t0, __copy_table__
    la t1, __copy_table_end__
0:  beq t0, t1, 2f
    lw t2, (0*4)(t0) // addr
    lw t3, (1*4)(t0) // loadaddr
    lw t4, (2*4)(t0) // sizeof
    add t0, t0, (3*4)
1:  beq zero, t4, 0b // while t4 != 0
    lw t5, (0)(t3)
    sw t5, (0)(t2)
    add t2, t2, 4
    add t3, t3, 4
    add t4, t4, -4
    j 1b
2:

    // xxx
    li a0, 1
    call neorv32_gpio_pin_set


    // zero sections
    la t0, __zero_table__
    la t1, __zero_table_end__
0:  beq t0, t1, 2f
    lw t2, (0*4)(t0) // addr
    lw t3, (1*4)(t0) // sizeof
    add t0, t0, (2*4)
1:  beq zero, t3, 0b // while t3 != 0
    sw zero, (0)(t2)
    add t2, t2, 4
    add t3, t3, -4
    j 1b
2:

    // xxx
    li a0, 2
    call neorv32_gpio_pin_set


    // run init functions
    la t0, __init_array__
    la t1, __init_array_end__
0:  beq t0, t1, 1f // while t0 != t1
    lw t2, (0*4)(t0) // task init function
    add t0, t0, (1*4)
    jalr t2
    j 0b
1:

    // xxx testing for code flow
    li a0, 3
    call neorv32_gpio_pin_set


    // jump to _init
    jal _init

    // jump to rt_start
    jal rt_start

_exit_loop:
    j _exit_loop






