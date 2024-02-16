.global main
.global isr

.align 4
.section .text

// Entry Point
//

.global _start
_start:
    .option push
    .option norelax
    la gp, __global_pointer$
    .option pop
    j _crt0_init

// Setup C Runtime
//

.global _crt0_init
_crt0_init:
    // set stack pointer
    la sp, __stack_begin

    // set vector table
    la a0, _vector_table
    addi a0, a0, 1
    csrw mtvec, a0
    
    // init .data section
    la a0, __data_addr 
    la a1, __data_load_addr
    la a2, __data_load_end
    data_loop:
    beq a1, a2, data_done
    lw a3, 0(a0)
    sw a3, 0(a1)
    add a0, a0, 4
    add a1, a1, 4
    j data_loop
    data_done:

    // init .bss section
    la a0, __bss_load_addr
    la a1, __bss_load_end
    bss_loop:
    beq a0, a1, bss_done
    sw zero, 0(a0)
    add a0, a0, 4
    j bss_loop
    bss_done:

    // jump to main
    call main
    dead_loop:
    j dead_loop

