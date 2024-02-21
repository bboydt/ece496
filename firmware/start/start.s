.align 4
.section .text
.global _start
_start:
    # set stack pointer
    la sp, __stack_pointer

    # set vector table
    la t0, __vector_table
    # for now just use a single vector 
    # add t0, t0, 1
    csrw mtvec, t0

    # clear .bss
    la t0, __bss_start
    la t1, __bss_end
    bss_loop:
    beq t0, t1, bss_loop_end # stop if t0 == t1
    sw zero, (0)(t0) # *t0 = 0
    add t0, t0, 4 # t0 += 4
    j bss_loop
    bss_loop_end:
    
    # init .data
    la t0, __data_start
    la t1, __data_end
    la t2, __data_load_addr
    data_loop:
    beq t0, t1, data_loop_end # stop if t0 == t1
    lw t3, (0)(t0) # *t2 = *t0
    sw t3, (0)(t2) 
    add t0, t0, 4 # t0 += 4
    add t2, t2, 4 # t2 += 4
    j data_loop
    data_loop_end:

    li a0, 0
    jal neorv32_gpio_pin_set

    # jump to _init
    jal _init

    # jump to rt_start
    jal rt_start

_exit_loop:
    j _exit_loop
