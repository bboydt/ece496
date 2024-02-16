#include <neorv32.h>

#define BAUD_RATE 115200
#define ISR __attribute__((interrupt("machine")))
#define MTIME_PERIOD (NEORV32_SYSINFO->CLK / 1000) // 1ms

uint32_t systick = 0;

ISR void uart0_rx_handler(void) 
{
    neorv32_uart0_putc(neorv32_uart0_getc());
    neorv32_cpu_csr_clr(CSR_MIP, 1 << UART0_RX_FIRQ_PENDING);
}

ISR void timer_handler(void)
{
    neorv32_mtime_set_timecmp(neorv32_mtime_get_timecmp() + MTIME_PERIOD);
    systick++;
}

static void print_startup_message()
{
    neorv32_uart0_puts("\n");
    neorv32_uart0_puts("           __..-''\"'\"'-=.+\n");
    neorv32_uart0_puts("     __..''      __.-' R |\n");
    neorv32_uart0_puts("    [---......+'' ./  E  |\n");
    neorv32_uart0_puts("    |\\       /| ./  T  ./|\n");
    neorv32_uart0_puts("    | \\     /'|/   T ./  ]\n");
    neorv32_uart0_puts("    | /'---'\\ |  U  / ..''\n");
    neorv32_uart0_puts("    [/       \\| B ./-'    \n");
    neorv32_uart0_puts("    \"\"....____V_.'\"       \n");
    neorv32_uart0_puts("\n");
    neorv32_uart0_puts("Howdy sailor!!\n");
    neorv32_uart0_puts("\n");
}


static void uart0_init()
{
    neorv32_uart0_setup(BAUD_RATE, ~1); // enables irq for non empty rx fifo
    neorv32_cpu_csr_set(CSR_MIE, 1 << UART0_RX_FIRQ_ENABLE);
}

static void mtime_init()
{
    neorv32_mtime_set_timecmp(neorv32_mtime_get_time() + MTIME_PERIOD);
    neorv32_cpu_csr_set(CSR_MIE, 1 << CSR_MIE_MTIE);
}

static void enable_interrupts()
{
    neorv32_cpu_csr_set(CSR_MSTATUS, 1 << CSR_MSTATUS_MIE);
}


int main(void)
{
    neorv32_gpio_pin_clr(0);
    neorv32_cpu_csr_clr(CSR_MSTATUS, 1 << CSR_MSTATUS_MIE);

    uart0_init();
    mtime_init();
    enable_interrupts();

    print_startup_message();

    uint32_t last_toggle = 0;
    while (1)
    {
        uint32_t current_tick = systick;
        if (current_tick - last_toggle >= 1000)
        {
            last_toggle = current_tick;
            neorv32_gpio_pin_toggle(0);
        }
    }
}
