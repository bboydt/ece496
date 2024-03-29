.global rt_syscall_pend
rt_syscall_pend:
    li t0, 1
    csrw mscratch, t0

// ecall handler
.global rt_syscall_handler
rt_syscall_handler:
    
    // note on why regs are not stored or loaded here:
    // this is always called from an interrupt
    // so, the registers have already been pushed onto 
    // the stack and will be popped off when we return

    // see if there is a new context to run
    add sp, sp, -(4*4)
    sw ra, (0*4)(sp)
    jal rt_syscall_run
    lw ra, (0*4)(sp)
    add sp, sp, (4*4)

    // if a0 != 0
    beq zero, a0, 0f

    // swap context
    // write stack pointer into rt_context_prev
    la t0, rt_context_prev
    lw t0, 0(t0)
    sw sp, 0(t0)
    mv sp, a0

0:  ret

// entry point for rt
.global rt_start
rt_start:
    jal rt_start_context
    mv sp, a0 # sp = ctx from rt_start_context

    # load registers
    lw t1 , (4*1 )(sp)
    lw t0 , (4*0 )(sp)
    
    csrw mepc, t1
    csrw mstatus, t0

    lw x31, (4*31)(sp)
    lw x30, (4*30)(sp)
    lw x29, (4*29)(sp)
    lw x28, (4*28)(sp)
    lw x27, (4*27)(sp)
    lw x26, (4*26)(sp)
    lw x25, (4*25)(sp)
    lw x24, (4*24)(sp)
    lw x23, (4*23)(sp)
    lw x22, (4*22)(sp)
    lw x21, (4*21)(sp)
    lw x20, (4*20)(sp)
    lw x19, (4*19)(sp)
    lw x18, (4*18)(sp)
    lw x17, (4*17)(sp)
    lw x16, (4*16)(sp)
    lw x15, (4*15)(sp)
    lw x14, (4*14)(sp)
    lw x13, (4*13)(sp)
    lw x12, (4*12)(sp)
    lw x11, (4*11)(sp)
    lw x10, (4*10)(sp)
    lw x9 , (4*9 )(sp)
    lw x8 , (4*8 )(sp)
    lw x7 , (4*7 )(sp)
    lw x6 , (4*6 )(sp)
    lw x5 , (4*5 )(sp)
    lw x4 , (4*4 )(sp)
    lw x3 , (4*3 )(sp)
    lw x1 , (4*2 )(sp)

    add sp, sp, (4*32)

    // enable interrupts
    li t0, 8
    csrs mstatus, t0

    // jump to task entry
    jalr ra

0:  j 0b

// initialize cycle counter
.global rt_cycle_init
rt_cycle_init:
0:  j 0b
    ret
