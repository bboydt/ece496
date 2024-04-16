#include "rt_extra.h"
#include <rt/task.h>
#include <rt/stack.h>

#include <soc/leds.h>

#include <stdint.h>

#include "car.h"

#define HEARTBEAT_TASK_PERIOD MS_TO_TICKS(500)
#define HEARTBEAT_LED_COLOR 0x00FFFF

const uint32_t state_led_colors[4] = 
{
    [CAR_STATE_IDLE]   = 0xFFFF00,
    [CAR_STATE_MANUAL] = 0x0000FF,
    [CAR_STATE_AUTO]   = 0x00FF00,
    [CAR_STATE_FAULT]  = 0xFF0000
};

static void heartbeat(void)
{
    // wait for all systems up
    while (car.systems != CAR_SYSTEMS_READY)
        rt_task_yield();

    car.state = CAR_STATE_IDLE;

    // clear leds
    for (int i = 0; i < SOC_LED_COUNT; i++)
    {
        SOC_LEDS->leds[i] = 0;
    }

    // 1Hz blink
    uint32_t tick = 0;
    int led_on = 0;
    for (;;)
    {
        SOC_LEDS->leds[6] = led_on ? state_led_colors[car.state] : 0;
        led_on = !led_on;
        rt_task_sleep_periodic(&tick, HEARTBEAT_TASK_PERIOD);
    }
}

RT_TASK(heartbeat, RT_STACK_MIN, 1);
