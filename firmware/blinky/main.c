#include <neorv32.h>

#include <rt/start.h>
#include <rt/task.h>
#include <rt/tick.h>

#include "common.h"
#include "leds.h"

static void blinky()
{
    unsigned long last_wake_tick = 0;
    const unsigned long sleep_period = 500;

    for (;;)
    {
        LEDS->led[0] ^= 0x00001F1F;
        rt_task_sleep_periodic(&last_wake_tick, sleep_period);
    }
}

RT_TASK(blinky, 1024, 1);

void mcu_init(void)
{
    LEDS->led[0] = 0x00000000;
}
