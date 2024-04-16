#include <neorv32.h>
#include <stdint.h>

#include <rt/start.h>
#include <rt/task.h>
#include "rt_extra.h"

#include <soc/leds.h>

#define BLINKY_TASK_PERIOD MS_TO_TICKS(500)

static void blinky()
{
    uint32_t tick = 0;
    for (;;)
    {
        SOC_LEDS->leds[0] ^= 0x00007F7F;
        rt_task_sleep_periodic(&tick, BLINKY_TASK_PERIOD);
    }
}

RT_TASK(blinky, RT_STACK_MIN, 1);

void mcu_init(void)
{
    SOC_LEDS->leds[0] = 0x000000;
}
