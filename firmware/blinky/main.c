#include <neorv32.h>

#include <rt/start.h>
#include <rt/task.h>
#include <rt/tick.h>

#include "common.h"

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


static void blinky()
{
    unsigned long last_wake_tick = 0;

    const unsigned long sleep_period = 500;

    neorv32_gpio_pin_set(2);

    for (;;)
    {
        //neorv32_gpio_pin_toggle(2);
        neorv32_uart0_puts("blinky!!\n");
        rt_task_sleep_periodic(&last_wake_tick, sleep_period);
    }
}

RT_TASK(blinky, RT_STACK_MIN, 1);

void mcu_init(void)
{
    // setup uart0
    neorv32_uart0_setup(115200, ~0); 
    //neorv32_cpu_csr_set(CSR_MIE, 1 << UART0_RX_FIRQ_ENABLE);


    print_startup_message();

    neorv32_gpio_pin_set(1);
}
