#include <neorv32.h>

#include <soc/leds.h>

#include "rt_extra.h"
#include "car.h"

volatile struct global_state car;

void mcu_init(void)
{
    // Startup Lights
    for (int i = 0; i < SOC_LED_COUNT; i++)
        SOC_LEDS->leds[i] = 0xFFFF00;

    // UART
    neorv32_uart_setup(NEORV32_UART0, 115200, ~0);
    neorv32_uart_setup(NEORV32_UART1, 115200, ~0);
    neorv32_uart_puts(NEORV32_UART0, ":: Wall-E OS - v1 ::\n");

    // I2C
    neorv32_twi_setup(CLK_PRSC_128, 1, 1);

    // State
    car.systems = 0;
}
