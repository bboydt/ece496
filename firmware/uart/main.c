#include <neorv32.h>

#define BAUD_RATE 19200

void isr(void)
{
}

int main(void)
{
    neorv32_uart0_setup(BAUD_RATE, 0);
    neorv32_uart0_puts("\n");
    neorv32_uart0_puts(" _______  __   __  _______  _______  _______  ______    _______  _______  ___   _______  ___   _  __ \n"); 
    neorv32_uart0_puts("|  _    ||  | |  ||       ||       ||       ||    _ |  |       ||       ||   | |       ||   | | ||  |\n"); 
    neorv32_uart0_puts("| |_|   ||  | |  ||_     _||_     _||    ___||   | ||  |  _____||_     _||   | |       ||   |_| ||  |\n"); 
    neorv32_uart0_puts("|       ||  |_|  |  |   |    |   |  |   |___ |   |_||_ | |_____   |   |  |   | |       ||      _||  |\n"); 
    neorv32_uart0_puts("|  _   | |       |  |   |    |   |  |    ___||    __  ||_____  |  |   |  |   | |      _||     |_ |__|\n"); 
    neorv32_uart0_puts("| |_|   ||       |  |   |    |   |  |   |___ |   |  | | _____| |  |   |  |   | |     |_ |    _  | __ \n"); 
    neorv32_uart0_puts("|_______||_______|  |___|    |___|  |_______||___|  |_||_______|  |___|  |___| |_______||___| |_||__|\n");
    neorv32_uart0_puts("\n");
    neorv32_uart0_puts("Howdy sailor!\n");

    while (1)
    {
        neorv32_gpio_pin_toggle(0);
    }
}
