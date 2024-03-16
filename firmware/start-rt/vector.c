#include <stdint.h>

// defines trap codes
#include <neorv32.h>

#include <rt/stack.h>
#include <rt/syscall.h>
#include <rt/tick.h>

#include "common.h"
#include "soc/leds.h"

extern void rt_syscall_handler(void);

RT_STACK(stack, 256);

#define __irq__ __attribute__((interrupt("machine")))

void riscv_mtvec_mti(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_msi(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_mei(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq0(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq1(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq2(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq3(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq4(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq5(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq6(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq7(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq8(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq9(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq10(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq11(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq12(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq13(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq14(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 
void riscv_mtvec_platform_irq15(void) __attribute__((interrupt("machine"), weak, alias("riscv_mtvec_default"))); 

static inline void set_leds(uint32_t color)
{
    for (int i = 0; i < 7; i++)
    {
        LEDS->led[i] = color;
    }
}

static inline void dead_loop(void)
{
    for (;;)
    {
        __asm__ __volatile__ ("nop");
    }
}

__irq__ void riscv_mtvec_default(void) 
{
    set_leds(0x00FF1F00);
    dead_loop();
}

enum mtvec_exceptions
{
    MTVEC_EX_I_ACCESS_FAULT     = 0x01,
    MTVEC_EX_I_ILLEGAL          = 0x02,
    MTVEC_EX_I_MISALIGNED       = 0x00,
    MTVEC_EX_MENV_CALL          = 0x0b,
    MTVEC_EX_UENV_CALL          = 0x08,
    MTVEC_EX_BREAKPOINT         = 0x03,
    MTVEC_EX_STORE_MISALIGNED   = 0x06,
    MTVEC_EX_LOAD_MISALIGNED    = 0x04,
    MTVEC_EX_STORE_ACCESS_FAULT = 0x07,
    MTVEC_EX_LOAD_ACCESS_FAULT  = 0x05
};

__irq__ void riscv_mtvec_exception_old(void)
{
    uint32_t mcause;
    __asm__ __volatile__ ("csrr %0, mcause" : "=r"(mcause) ::);

    switch (mcause)
    {
        case MTVEC_EX_MENV_CALL:
            uint32_t mepc;
            __asm__ __volatile__ ("csrr %0, mepc" : "=r"(mepc) :: "memory");
            mepc += 4;
            __asm__ __volatile__ ("csrw mepc, %0" :: "r"(mepc) : "memory");
            rt_syscall_handler();
            break;
        case MTVEC_EX_BREAKPOINT:
            set_leds(0x00FF1FFF);
            dead_loop();
            break;
        default:
            set_leds(0x00FF0000);
            dead_loop();
            break;
    }
}

__attribute__((naked, section(".vector_table"), aligned(128)))
void vector_table(void)
{
    __asm__ __volatile__ (
        /* 0 */
        ".org  vector_table + 0*4;"
        "jal zero, riscv_mtvec_exception;"
        
        /* 3 */
        ".org  vector_table + 3*4;"
        "jal zero, riscv_mtvec_msi;"
        
        /* 7 */
        ".org  vector_table + 7*4;"
        "jal zero, riscv_mtvec_mti;"
        
        /* 11 */
        ".org  vector_table + 11*4;"
        "jal zero, riscv_mtvec_mei;"

        /* 16 */
        ".org  vector_table + 16*4;"
        "jal   riscv_mtvec_platform_irq0;"
        "jal   riscv_mtvec_platform_irq1;"
        "jal   riscv_mtvec_platform_irq2;"
        "jal   riscv_mtvec_platform_irq3;"
        "jal   riscv_mtvec_platform_irq4;"
        "jal   riscv_mtvec_platform_irq5;"
        "jal   riscv_mtvec_platform_irq6;"
        "jal   riscv_mtvec_platform_irq7;"
        "jal   riscv_mtvec_platform_irq8;"
        "jal   riscv_mtvec_platform_irq9;"
        "jal   riscv_mtvec_platform_irq10;"
        "jal   riscv_mtvec_platform_irq11;"
        "jal   riscv_mtvec_platform_irq12;"
        "jal   riscv_mtvec_platform_irq13;"
        "jal   riscv_mtvec_platform_irq14;"
        "jal   riscv_mtvec_platform_irq15;"

        :::
    );
}
