#include <neorv32.h>

#define BAUD_RATE 115200

__attribute__((interrupt("machine"), weak))
void uart0_rx_handler(void) 
{
    neorv32_uart0_getc();
    neorv32_uart0_puts("it worked!\n");
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


int main(void)
{
    neorv32_gpio_pin_clr(0);

    neorv32_uart0_setup(BAUD_RATE, 1);
    neorv32_cpu_csr_set(CSR_MIE, 1 << UART0_RX_FIRQ_ENABLE);
    neorv32_cpu_csr_set(CSR_MSTATUS, 1 << CSR_MSTATUS_MIE);

    print_startup_message();
    
    while (1)
    {
        for (int i = 0; i < 10000; i++);
        neorv32_gpio_pin_toggle(0);
    }
}
