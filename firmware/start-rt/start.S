.text

// entry point from the bootrom
.org 0x0
.global _reset_vector
_reset_vector:
    // disable interrupts
    csrc mstatus, 8

    // set stack pointer
    la sp, __stack_pointer

    // set global pointer
    //la gp, __global_pointer$

    // set trap vector
    la t0, rt_trap_handler
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






