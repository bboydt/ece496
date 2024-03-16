#include <neorv32.h>

#include <stdio.h>

#include <rt/start.h>
#include <rt/task.h>
#include <rt/tick.h>

#include "common.h"
#include "soc/leds.h"

static void blinky()
{
    unsigned long last_wake_tick = 0;
    const unsigned long sleep_period = 100;
    int i = 1;

    for (;;)
    {
        LEDS->led[i] = 0;
        i ^= 1;
        LEDS->led[i] ^= 0x00001F1F;
        rt_task_sleep_periodic(&last_wake_tick, sleep_period);
        fprintf(stdout, "Blinky!\n");
    }
}

static void shouty()
{
    unsigned long last_wake_tick = 0;
    const unsigned long sleep_period = 50;
    int i = 1;

    for (;;)
    {
        LEDS->led[i+2] = 0;
        i ^= 1;
        LEDS->led[i+2] ^= 0x001F1F00;
        rt_task_sleep_periodic(&last_wake_tick, sleep_period);
        fprintf(stdout, "Shawty like a melody!\n");
    }
}

static void shouty_2()
{
    unsigned long last_wake_tick = 0;
    const unsigned long sleep_period = 25;
    int i = 1;

    for (;;)
    {
        LEDS->led[i+4] = 0;
        i ^= 1;
        LEDS->led[i+4] ^= 0x001F0000;
        rt_task_sleep_periodic(&last_wake_tick, sleep_period);
        fprintf(stdout, "Up in my head!\n");
    }
}


RT_TASK(blinky, 2048, 1);
RT_TASK(shouty, 2048, 1);
RT_TASK(shouty_2, 2048, 1);

void mcu_init(void)
{
    LEDS->led[0] = 0x00000000;

    neorv32_uart_setup(stdout,  115200, ~0);
    fprintf(stdout, ">>> STARTUP <<<\n");
}

