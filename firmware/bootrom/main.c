#include <neorv32.h>

#define BAUD_RATE 115200

void isr(void)
{
}

int main(void)
{
    neorv32_uart0_setup(BAUD_RATE, 0);

    neorv32_uart0_puts("Bootrom started...\n");
    neorv32_uart0_puts("Checking for image header...\n");
    neorv32_uart0_puts("ERROR: No image detected. Reboot to try again.\n");

    for (;;);
}
