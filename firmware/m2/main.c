#include <neorv32.h>

#include <soc/encoders.h>
#include <soc/leds.h>
#include <soc/pwms.h>

#include <rt/task.h>
#include <rt/stack.h>

#include <stdint.h>
#include <stdio.h>

//
// Heartbeat Task
//

// Blinkys an LED at 1Hz

static void heartbeat(void)
{
    uint32_t last_wake_tick = 0;
    const uint32_t sleep_period = 50;

    for (;;)
    {
        LEDS->led[0] ^= 0x00000F0F;
        rt_task_sleep_periodic(&last_wake_tick, sleep_period);
    }
}

RT_TASK(heartbeat, RT_STACK_MIN, RT_TASK_PRIORITY_MIN);

//
// Traction Control
//

#define MEASURE_SPEED_PERIOD 50

static int32_t motor_positions[4]; // raw encoder values
static uint32_t motor_speeds[4]; // rotations per second

static void measure_speed(void)
{
    uint32_t last_wake_tick = 0;

    for (;;)
    {
        for (int i = 0; i < 4; i++)
        {
            int32_t position = ENCODERS->positions[i];
            int32_t delta = 0x7FFFFFFF & (uint32_t)(position - motor_positions[i]);
            
            motor_speeds[i] = delta;
            motor_positions[i] = position;
        }

        rt_task_sleep_periodic(&last_wake_tick, MEASURE_SPEED_PERIOD);
    }
}

RT_TASK(measure_speed, RT_STACK_MIN, RT_TASK_PRIORITY_MAX);

static void report_speeds(void)
{
    uint32_t last_wake_tick = 0;

    for (;;)
    {
        for (int i = 0; i < 4; i++)
        {
            fprintf(stdout, "motor[%u] { pos=%i, speed=%u }\n", i, motor_positions[i], motor_speeds[i]);
        }
        rt_task_sleep_periodic(&last_wake_tick, 50);
    }
}

RT_TASK(report_speeds, RT_STACK_MIN, RT_TASK_PRIORITY_MIN + 1);

//
// Initialization
//

void mcu_init(void)
{
    for (int i = 0; i < 7; i++)
    {
        LEDS->led[i] = 0x00000000;
    }

    PWMS->ctrls[0] = 0x00000013;
    PWMS->vals[0] = 0x2BF215F9; // 1KHz 50%

    neorv32_uart_setup(stdout, 115200, ~0);
}
