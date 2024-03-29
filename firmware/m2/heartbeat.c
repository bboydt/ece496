#include <soc/common.h>
#include <soc/leds.h>

#include <rt/task.h>
#include <rt/stack.h>

#include <stdint.h>

#include "car.h"

#define HEARTBEAT_TASK_PERIOD MS_TO_TICKS(500)

const uint32_t colors[4] =
{
    [CAR_MODE_IDLE]   = 0xFFFF00,
    [CAR_MODE_MANUAL] = 0x0000FF,
    [CAR_MODE_AUTO]   = 0x00FF00,
    [CAR_MODE_FAULT]  = 0xFF0000,
};

static void heartbeat(void)
{
    uint32_t last_wake_tick = 0;
    int color_mask = 0;
    for (;;)
    {
        color_mask ^= 0xFFFFFF;
        LEDS->led[0] = color_mask & colors[car.mode];
        rt_task_sleep_periodic(&last_wake_tick, HEARTBEAT_TASK_PERIOD);
    }
}

RT_TASK(heartbeat, RT_STACK_MIN, RT_TASK_PRIORITY_MIN);
