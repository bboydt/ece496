#include <neorv32.h>

void isr(void)
{
}

int main(void)
{
    neorv32_gpio_pin_clr(0);

    while (1)
    {
        for (int i = 0; i < 10000; i++);
        neorv32_gpio_pin_toggle(0);
    }
}
