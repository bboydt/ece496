#include <neorv32.h>

#define BAUD_RATE 115200

void isr(void)
{
}

int main(void)
{
    neorv32_gpio_pin_clr(0);

    neorv32_uart0_setup(BAUD_RATE, 0);
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

    while (1)
    {
        for (int i = 0; i < 10000000; i++);
        neorv32_gpio_pin_toggle(0);
    }
}
