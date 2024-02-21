#include <stdint.h>

#include <rt/stack.h>
#include <rt/syscall.h>
#include <rt/tick.h>

extern void mti_handler(void);

RT_STACK(stack, 256);

enum trap_cause_t
{
    TRAP_INSTRUCTION_ACCESS = 0, // Instruction access fault
    TRAP_INSTRUCTION_ILLEGAL = 1, // Illegal instruction
    TRAP_INSTRUCTION_MISALIGNED = 2, // Instruction address misaligned
    TRAP_BREAKPOINT = 3, // Breakpoint (ebrake)
    TRAP_LOAD_MISALIGNED = 4, // Load address misaligned
    TRAP_LOAD_ACCESS = 5, // Load access fault
    TRAP_STORE_MISALIGNED = 6, // Store address misaligned
    TRAP_STORE_ACCESS = 7, // Store access fault
    TRAP_UENV_CALL = 8, // User mode ecall
    TRAP_MENV_CALL = 9, // Machine mode ecall
    TRAP_MSI = 10, // Machine software interrupt
    TRAP_MTI = 11, // Machine timer interrupt
    TRAP_MEI = 12, // Machine external interrupt

    // Fast Interrupt Request Channels
    TRAP_FIRQ_0  = 13,
    TRAP_FIRQ_1  = 14,
    TRAP_FIRQ_2  = 15,
    TRAP_FIRQ_3  = 16,
    TRAP_FIRQ_4  = 17,
    TRAP_FIRQ_5  = 18,
    TRAP_FIRQ_6  = 19,
    TRAP_FIRQ_7  = 20,
    TRAP_FIRQ_8  = 21,
    TRAP_FIRQ_9  = 22,
    TRAP_FIRQ_10 = 23,
    TRAP_FIRQ_11 = 24,
    TRAP_FIRQ_12 = 25,
    TRAP_FIRQ_13 = 26,
    TRAP_FIRQ_14 = 27, 
    TRAP_FIRQ_15 = 28,

    TRAP_ENUM_COUNT = 29
};

static void default_handler(void)
{
    for (;;)
    {
        asm volatile ("nop");
    }
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverride-init"
const void (*vector_table[29])(void) __attribute__((section(".vector_table"))) =
{
    [0 ... TRAP_ENUM_COUNT - 1] = default_handler,
    [TRAP_MENV_CALL] = rt_syscall_handler,
    [TRAP_MTI] = mti_handler
};
#pragma GCC diagnostic pop

// this is called by _trap_handler
void _execption_handler(uint32_t cause)
{
    // @todo: assert(cause < TRAP_ENUM_COUNT);
    vector_table[cause]();
}

