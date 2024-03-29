#include <neorv32.h>

#include <rt/task.h>
#include <rt/stack.h>

#include <soc/encoders.h>
#include <soc/common.h>
#include <soc/leds.h>
#include <soc/pwms.h>

#include "motors.h"
#include "car.h"

#include <stdint.h>

#define MOTOR_CONTROL_TASK_PERIOD MS_TO_TICKS(1000/20)

#define KP 8
#define KI 0
#define KD 2

static int32_t clamp(int32_t x, int32_t min, int32_t max)
{
    return (x < min) ? min : ((x > max) ? max : x);
}

//#define CONTROL_LOOP_DEBUG 1

static void motor_control(void)
{
    uint32_t last_wake_tick = 0;

    int32_t speed;
    int32_t position;
    int32_t error;
    int32_t diff;

    int ii = 0;

    for (;;)
    {
        LEDS->led[1] ^= 0x00FF0000;

        // measure speeds
        for (int i = 0; i < 4; i++)
        {
            position = ENCODERS->positions[i];
            speed = (position - car.positions[i]);
            if (speed < -500 || 500 < speed)
            {
                car.positions[i] = position;
                continue;
            }

            car.current_speeds[i] -= car.current_speeds_pre_avg[i][ii];
            car.current_speeds_pre_avg[i][ii] = speed;
            car.current_speeds[i] += car.current_speeds_pre_avg[i][ii];
            car.positions[i] = position;

        }

        ii = (ii + 1) % SPEED_SAMPLES;

        // control loops
        for (int i = 0; i < 4; i++)
        {
            error = abs(car.target_speeds[i]) - abs(car.current_speeds[i]);
            diff = error - car.errors[i];
            car.errors[i] = error;
            car.error_sums[i] += error;
            
            car.motor_pwm_widths[i] = clamp(car.motor_pwm_widths[i] + error/KP + diff/KD, 0x00, 0xFF);

            if (abs(car.target_speeds[i]) > 10)
            {
                PWMS->duty_cycles[i] = car.motor_pwm_widths[i];
            }
            else
            {
                car.motor_pwm_widths[i] = 0;
                PWMS->duty_cycles[i] = 0;
            }

            if (car.target_speeds[i] < 0)
            {
                neorv32_gpio_pin_clr(i);
            }
            else
            {
                neorv32_gpio_pin_set(i);
            }
        }

#ifdef CONTROL_LOOP_DEBUG
        for (int i = 0; i < 4; i++)
            neorv32_uart_printf(NEORV32_UART0, "error[%d]=%d\t", i, car.errors[i]);
        neorv32_uart_printf(NEORV32_UART0, "  |  ");

        for (int i = 0; i < 4; i++)
            neorv32_uart_printf(NEORV32_UART0, "speed[%d]=%d\t", i, car.current_speeds[i]);
        neorv32_uart_printf(NEORV32_UART0, "  |  ");

        for (int i = 0; i < 4; i++)
            neorv32_uart_printf(NEORV32_UART0, "pwm[%d]=%d\t", i, car.motor_pwm_widths[i]);
        neorv32_uart_printf(NEORV32_UART0, "\n");
#endif

        rt_task_sleep_periodic(&last_wake_tick, MOTOR_CONTROL_TASK_PERIOD);
    }
}

RT_TASK(motor_control, 8<<10, RT_TASK_PRIORITY_MIN);
