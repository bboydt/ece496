#include <neorv32.h>

#include <stdint.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <soc/common.h>
#include <soc/encoders.h>
#include <soc/leds.h>
#include <soc/pwms.h>

#include <rt/task.h>
#include <rt/stack.h>
#include <rt/tick.h>

#include "protocol.h"
#include "car.h"

struct car_state car = { 0 };

//
// Motor Control Task
//

//#define MOTOR_CONTROL_TASK_PERIOD MS_TO_TICKS(1000/20)
//
//#define min(x, y) (((x) < (y)) ? (x) : (y))
//#define max(x, y) (((x) > (y)) ? (x) : (y))
//
//void* memset(void *dst, int val, size_t len)
//{
//    for (int i = 0; i < len; i++)
//    {
//        ((uint8_t*)dst)[i] = (uint8_t)val;
//    }
//    return dst;
//}
//
//#define SPEED_SAMPLES 8
//
//static inline int32_t clamp(int32_t x, int32_t min, int32_t max)
//{
//    return (x < min) ? min : ((x > max) ? max : x);
//}
//#endif
//
//#define KP (16)
//#define KI (0)
//#define KD (1)
//
//#if 0
//static void motor_control(void)
//{
//    unsigned long last_wake_tick = 0;
//
//    uint32_t dt = MOTOR_CONTROL_TASK_PERIOD;
//    int32_t pos, prev_pos = 0, speeds[SPEED_SAMPLES] = { 0 }, speed = 0;
//    uint32_t last_read_tick = 0;
//    int ii = 0;
//
//    int32_t e; // error
//    int32_t prev_e = 0;
//    int32_t i = 0; // integral
//    int32_t d; // derivative
//
//    int32_t target_speed = SPEED_SAMPLES*20;
//    int32_t avg_speed;
//
//    int32_t width = 0;
//
//    for (;;)
//    {
//        pos = ENCODERS->positions[3];
//        speed -= speeds[ii];
//        speeds[ii] = (pos - prev_pos);
//        speed += speeds[ii];
//        prev_pos = pos;
//        
//        avg_speed = speed;
//        
//        e = target_speed - speed;
//        i += e;
//        d = e - prev_e;
//        prev_e = e;
//
//        width = clamp(width + (KP * e) + (KI * i) + (KD * d), 0, 0xFFFF);
//
//        PWMS->duty_cycles[3] = 0x000000FF & (width >> 8); 
//
//        dt = TICKS_TO_MS(last_wake_tick - last_read_tick);
//        last_read_tick = last_wake_tick;
//
//        neorv32_uart_printf(NEORV32_UART0, "\e[34mINFO:\e[0m\tspeed=%d,\tavg_speed=%d,\twidth=%d\n", speeds[ii], speed / SPEED_SAMPLES, width);
//        
//        ii = (ii + 1) % SPEED_SAMPLES;
//
//        rt_task_sleep_periodic(&last_wake_tick, MOTOR_CONTROL_TASK_PERIOD);
//        
//    }
//
//}
//
//RT_TASK(motor_control, RT_STACK_MIN, 0);

//
// Initialization
//

void mcu_init(void)
{
    PWMS->duty_cycles[4] = 20;

    for (int i = 0; i < 7; i++)
    {
        LEDS->led[i] = 0x00000000;
    }

    for (int i = 0; i < 4; i++)
    {
        PWMS->duty_cycles[i] = 0x00;
    }

    neorv32_uart_setup(NEORV32_UART0, 115200, ~0);
    neorv32_uart_setup(NEORV32_UART1, 115200, ~0);

    for (int i = 0; i < 4; i++)
    {
        car.target_speeds[i] = 0;
    }

    neorv32_uart_puts(NEORV32_UART0, ":: Warehouse BOT ::\n");
}
