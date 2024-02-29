#include <stdint.h>

// defines trap codes
#include <neorv32.h>

#include <rt/stack.h>
#include <rt/syscall.h>
#include <rt/tick.h>

extern void mti_handler(void);

RT_STACK(stack, 256);

static void default_handler(void)
{
    for (;;)
    {
        asm volatile ("nop");
    }
}

struct vector_table_t 
{
    void (*exception_handlers[16])(void);
    void (*machine_handlers[16])(void);
    void (*platform_handlers[16])(void);
};

#define TRAP_HANDLER_INDEX(n) ((n) & 0x0F)

void mti_handler(void) __attribute__((alias("default_handler"), weak));
void uart0_rx_handler(void) __attribute__((alias("default_handler"), weak));
void uart1_rx_handler(void) __attribute__((alias("default_handler"), weak));

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Woverride-init"
const struct vector_table_t vector_table __attribute__((section(".vector_table"), weak)) =
{
    // look in deps/neorv32/sw/lib/include/neorv32_cpu_csr.h for trap codes

    .exception_handlers =
    {
        [0 ... 15] = default_handler,
    },
    
    .machine_handlers = 
    {
        [0 ... 15] = default_handler,
        [TRAP_HANDLER_INDEX(TRAP_CODE_MTI)] = mti_handler,
    },

    .platform_handlers =
    {
        [0 ... 15] = default_handler,
        [2] = uart0_rx_handler,
        [4] = uart1_rx_handler
    },
};
#pragma GCC diagnostic pop
