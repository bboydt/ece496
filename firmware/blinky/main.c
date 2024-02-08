#include <neorv32.h>

void isr(void)
{
}

int main(void)
{
    while (1)
    {
        neorv32_gpio_pin_toggle(0);
    }
}
