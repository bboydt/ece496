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
static int32_t motor_speeds[4]; // rotations per second

static void measure_speed(void)
{
    uint32_t last_wake_tick = 0;

    for (;;)
    {
        for (int i = 0; i < 4; i++)
        {
            int32_t position = ENCODERS->positions[i];
            int32_t delta = (position - motor_positions[i]);
            if (delta < 0) delta = -delta;

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
#pragma GCC diagnostic push 
#pragma GCC diagnostic ignored "-Wformat"
            fprintf(stdout, "motor[%u] { pos=%d, speed=%d }\n", i, motor_positions[i], motor_speeds[i]);
#pragma GCC diagnostic pop
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

    for (int i = 0; i < 4; i++)
    {
        PWMS->ctrls[i] = 0x00000013;
        PWMS->vals[i] = 0x2BF2057E; // 1KHz 50%
    }

    neorv32_uart_setup(stdout, 115200, ~0);
}
