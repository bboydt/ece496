.macro push_context
    add sp, sp, -128
    sw x31, (31*4)(sp)
    sw x30, (30*4)(sp)
    sw x29, (29*4)(sp)
    sw x28, (28*4)(sp)
    sw x27, (27*4)(sp)
    sw x26, (26*4)(sp)
    sw x25, (25*4)(sp)
    sw x24, (24*4)(sp)
    sw x23, (23*4)(sp)
    sw x22, (22*4)(sp)
    sw x21, (21*4)(sp)
    sw x20, (20*4)(sp)
    sw x19, (19*4)(sp)
    sw x18, (18*4)(sp)
    sw x17, (17*4)(sp)
    sw x16, (16*4)(sp)
    sw x15, (15*4)(sp)
    sw x14, (14*4)(sp)
    sw x13, (13*4)(sp)
    sw x12, (12*4)(sp)
    sw x11, (11*4)(sp)
    sw x10, (10*4)(sp)
    sw x9 , ( 9*4)(sp)
    sw x8 , ( 8*4)(sp)
    sw x7 , ( 7*4)(sp)
    sw x6 , ( 6*4)(sp)
    sw x5 , ( 5*4)(sp)
    sw x4 , ( 4*4)(sp)
    sw x3 , ( 3*4)(sp)
    sw x1 , ( 2*4)(sp)
    csrr t1, mstatus
    csrr t0, mepc
    sw t1 , ( 1*4)(sp)
    sw t0 , ( 0*4)(sp)
.endm

.macro pop_context
    lw t0 , ( 0*4)(sp)
    lw t1 , ( 1*4)(sp)
    csrw mepc, t0
    csrw mstatus, t1
    lw x1 , ( 2*4)(sp)
    lw x3 , ( 3*4)(sp)
    lw x4 , ( 4*4)(sp)
    lw x5 , ( 5*4)(sp)
    lw x6 , ( 6*4)(sp)
    lw x7 , ( 7*4)(sp)
    lw x8 , ( 8*4)(sp)
    lw x9 , ( 9*4)(sp)
    lw x10, (10*4)(sp)
    lw x11, (11*4)(sp)
    lw x12, (12*4)(sp)
    lw x13, (13*4)(sp)
    lw x14, (14*4)(sp)
    lw x15, (15*4)(sp)
    lw x16, (16*4)(sp)
    lw x17, (17*4)(sp)
    lw x18, (18*4)(sp)
    lw x19, (19*4)(sp)
    lw x20, (20*4)(sp)
    lw x21, (21*4)(sp)
    lw x22, (22*4)(sp)
    lw x23, (23*4)(sp)
    lw x24, (24*4)(sp)
    lw x25, (25*4)(sp)
    lw x26, (26*4)(sp)
    lw x27, (27*4)(sp)
    lw x28, (28*4)(sp)
    lw x29, (29*4)(sp)
    lw x30, (30*4)(sp)
    lw x31, (31*4)(sp)
    add sp, sp, 128
.endm


.text
.org 0x0
.global _reset_vector
_reset_vector:
    j _start


.org 0x20
// controls flow of all interrupts
.global _trap_handler
_trap_handler:

    // push context
    add sp, sp, -128
    sw x31, (31*4)(sp)
    sw x30, (30*4)(sp)
    sw x29, (29*4)(sp)
    sw x28, (28*4)(sp)
    sw x27, (27*4)(sp)
    sw x26, (26*4)(sp)
    sw x25, (25*4)(sp)
    sw x24, (24*4)(sp)
    sw x23, (23*4)(sp)
    sw x22, (22*4)(sp)
    sw x21, (21*4)(sp)
    sw x20, (20*4)(sp)
    sw x19, (19*4)(sp)
    sw x18, (18*4)(sp)
    sw x17, (17*4)(sp)
    sw x16, (16*4)(sp)
    sw x15, (15*4)(sp)
    sw x14, (14*4)(sp)
    sw x13, (13*4)(sp)
    sw x12, (12*4)(sp)
    sw x11, (11*4)(sp)
    sw x10, (10*4)(sp)
    sw x9 , ( 9*4)(sp)
    sw x8 , ( 8*4)(sp)
    sw x7 , ( 7*4)(sp)
    sw x6 , ( 6*4)(sp)
    sw x5 , ( 5*4)(sp)
    sw x4 , ( 4*4)(sp)
    sw x3 , ( 3*4)(sp)
    sw x1 , ( 2*4)(sp)
    csrr t0, mepc

    li t1, 0x0000000b
    csrr t2, mcause
    bne t1, t2, 0f
    add t0, t0, 4
0:  

    sw t0 , ( 0*4)(sp)

    // decode cause
    csrr t0, mcause

    li t1, 0xFFFFFFF0
    and t1, t1, t0

    li t2, 0x00000000
    bne t1, t2, 0f
    li t1, (0*4*16) // offs of table
    j 1f
0:  li t2, 0x80000000
    bne t1, t2, 0f
    li t1, (1*4*16) // offs of table
    j 1f
0:  li t2, 0x80000010
    bne t1, t2, 0f
    li t2, (2*4*16) // offs of table
    j 1f
0:  call rt_trap

1:  li t3, 0x0000000F
    and t3, t3, t0
    sll t3, t3, 2 // offs of handler

    la t0, vector_table
    add t0, t0, t2 // add offs of table
    add t0, t0, t3 // add offs of handler

    // call handler
    lw t0, (0)(t0)
    jalr t0

    // pop context
    lw t0 , ( 0*4)(sp)
    csrw mepc, t0

    lw x1 , ( 2*4)(sp)
    lw x3 , ( 3*4)(sp)
    lw x4 , ( 4*4)(sp)
    lw x5 , ( 5*4)(sp)
    lw x6 , ( 6*4)(sp)
    lw x7 , ( 7*4)(sp)
    lw x8 , ( 8*4)(sp)
    lw x9 , ( 9*4)(sp)
    lw x10, (10*4)(sp)
    lw x11, (11*4)(sp)
    lw x12, (12*4)(sp)
    lw x13, (13*4)(sp)
    lw x14, (14*4)(sp)
    lw x15, (15*4)(sp)
    lw x16, (16*4)(sp)
    lw x17, (17*4)(sp)
    lw x18, (18*4)(sp)
    lw x19, (19*4)(sp)
    lw x20, (20*4)(sp)
    lw x21, (21*4)(sp)
    lw x22, (22*4)(sp)
    lw x23, (23*4)(sp)
    lw x24, (24*4)(sp)
    lw x25, (25*4)(sp)
    lw x26, (26*4)(sp)
    lw x27, (27*4)(sp)
    lw x28, (28*4)(sp)
    lw x29, (29*4)(sp)
    lw x30, (30*4)(sp)
    lw x31, (31*4)(sp)
    add sp, sp, 128

    mret



// entry point from the bootrom
_start:
    // disable interrupts
    li t0, 8
    csrc mstatus, t0

    // set stack pointer
    la sp, __stack_pointer

    // set trap vector
    la t0, _trap_handler
    csrw mtvec, t0

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

    // run init functions
    la t0, __init_array__
    la t1, __init_array_end__
0:  beq t0, t1, 1f // while t0 != t1
    lw t2, (0*4)(t0) // task init function
    add t0, t0, (1*4)
    jalr ra, t2
    j 0b
1:

    // jump to _init
    jal _init

    // jump to rt_start
    jal rt_start

_exit_loop:
    j _exit_loop






