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
    la a0, _trap_entry
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


// Handle Interrupts
//

.global _trap_entry
_trap_entry:
    sw x1,   -1*4(sp)
    sw x5,   -2*4(sp)
    sw x6,   -3*4(sp)
    sw x7,   -4*4(sp)
    sw x10,  -5*4(sp)
    sw x11,  -6*4(sp)
    sw x12,  -7*4(sp)
    sw x13,  -8*4(sp)
    sw x14,  -9*4(sp)
    sw x15, -10*4(sp)
    sw x16, -11*4(sp)
    sw x17, -12*4(sp)
    sw x28, -13*4(sp)
    sw x29, -14*4(sp)
    sw x30, -15*4(sp)
    sw x31, -16*4(sp)
    addi sp, sp, -16*4
    call isr
    lw x1,  15*4(sp)
    lw x5,  14*4(sp)
    lw x6,  13*4(sp)
    lw x7,  12*4(sp)
    lw x10, 11*4(sp)
    lw x11, 10*4(sp)
    lw x12,  9*4(sp)
    lw x13,  8*4(sp)
    lw x14,  7*4(sp)
    lw x15,  6*4(sp)
    lw x16,  5*4(sp)
    lw x17,  4*4(sp)
    lw x28,  3*4(sp)
    lw x29,  2*4(sp)
    lw x30,  1*4(sp)
    lw x31,  0*4(sp)
    addi sp, sp, 16*4
    mret
