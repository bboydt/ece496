#include <stdint.h>

#include <soc/leds.h>

#include "rt_extra.h"
#include <rt/stack.h>
#include <rt/syscall.h>
#include <rt/tick.h>

enum riscv_exceptions
{
    RISCV_EXCEPTION_I_ACCESS = 0x1,
    RISCV_EXCEPTION_I_ILLEGAL = 0x2,
    RISCV_EXCEPTION_I_MISALIGNED = 0x0,
    RISCV_EXCEPTION_MENV_CALL = 0xB,
    RISCV_EXCEPTION_UENV_CALL = 0x8,
    RISCV_EXCEPTION_BREAKPOINT = 0x3,
    RISCV_EXCEPTION_S_MISALIGNED = 0x6,
    RISCV_EXCEPTION_L_MISALIGNED = 0x4,
    RISCV_EXCEPTION_S_ACCESS = 0x7,
    RISCV_EXCEPTION_L_ACCESS = 0x5
};

enum riscv_interrupts
{
    RISCV_INTERRUPT_MEI = 0xB,
    RISCV_INTERRUPT_MSI = 0x3,
    RISCV_INTERRUPT_MTI = 0x7,

    RISCV_INTERRUPT_FIRQ_0 = 0x10,
    RISCV_INTERRUPT_FIRQ_1 = 0x11,
    RISCV_INTERRUPT_FIRQ_2 = 0x12,
    RISCV_INTERRUPT_FIRQ_3 = 0x13,
    RISCV_INTERRUPT_FIRQ_4 = 0x14,
    RISCV_INTERRUPT_FIRQ_5 = 0x15,
    RISCV_INTERRUPT_FIRQ_6 = 0x16,
    RISCV_INTERRUPT_FIRQ_7 = 0x17,
    RISCV_INTERRUPT_FIRQ_8 = 0x18,
    RISCV_INTERRUPT_FIRQ_9 = 0x19,
    RISCV_INTERRUPT_FIRQ_10 = 0x1A,
    RISCV_INTERRUPT_FIRQ_11 = 0x1B,
    RISCV_INTERRUPT_FIRQ_12 = 0x1C,
    RISCV_INTERRUPT_FIRQ_13 = 0x1D,
    RISCV_INTERRUPT_FIRQ_14 = 0x1E,
    RISCV_INTERRUPT_FIRQ_15 = 0x1F
};

extern void rt_syscall_handler(void);

uint8_t rt_interrupt_stack[2048];

static void fault_handler(void)
{
    for (int i = 0; i < 7; i++)
    {
        SOC_LEDS->leds[i] = 0x00FF0000;
    }

    for (;;)
    {
        __asm__("nop" :::);
    }
}

static void default_handler(void)
{
    for (int i = 0; i < 7; i++)
    {
        SOC_LEDS->leds[i] = 0x007FFF00;
    }

    for (;;)
    {
        __asm__("nop" :::);
    }
}

extern void mti_handler(void) __attribute__((weak, alias("default_handler")));

struct vector_table
{
    void (*exception_handlers[16])(void);
    void (*interrupt_handlers[32])(void);
};

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverride-init"
const struct vector_table vector_table __attribute__((section(".vector_table"))) = {
    .exception_handlers = {
        [0 ... 15] = default_handler,
        [RISCV_EXCEPTION_MENV_CALL] = rt_syscall_handler,
        [RISCV_EXCEPTION_I_ACCESS] = fault_handler,
        [RISCV_EXCEPTION_I_ILLEGAL] = fault_handler,
        [RISCV_EXCEPTION_I_MISALIGNED] = fault_handler,
        [RISCV_EXCEPTION_S_MISALIGNED] = fault_handler,
        [RISCV_EXCEPTION_L_MISALIGNED] = fault_handler,
        [RISCV_EXCEPTION_S_ACCESS] = fault_handler,
        [RISCV_EXCEPTION_L_ACCESS] = fault_handler
    },

    .interrupt_handlers = {
        [0 ... 31] = default_handler,
        [RISCV_INTERRUPT_MTI] = mti_handler
    }
};
#pragma GCC diagnostic pop
