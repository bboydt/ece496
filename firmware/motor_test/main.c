#include <neorv32.h>

#include <soc/motors.h>
#include <soc/leds.h>

#include "rt_extra.h"
#include <rt/task.h>
#include <rt/stack.h>

//
// Test
//

#define TEST_TASK_PERIOD MS_TO_TICKS(500)

void test(void)
{
    uint32_t tick = 0;
    uint32_t motor_index = 0;
    uint32_t state_index = 0;

    const uint32_t states[16] =
    {
        0x8000000F,
        0x8000001F,
        0x8000003F,
        0x8000007F,
        0x8000003F,
        0x8000001F,
        0x8000000F,
        0x00000000,
        0xC000000F,
        0xC000001F,
        0xC000003F,
        0xC000007F,
        0xC000003F,
        0xC000001F,
        0xC000000F,
        0x00000000
    };

    const uint32_t colors[16] = 
    {
        0x00000F00,
        0x00001F00,
        0x00003F00,
        0x00007F00,
        0x00003F00,
        0x00001F00,
        0x00000F0F,
        0x00000000,
        0x000F0000,
        0x001F0000,
        0x003F0000,
        0x007F0000,
        0x003F0000,
        0x001F0000,
        0x000F0000,
        0x00000000
    };

    for (;;)
    {
        SOC_MOTORS->motors[motor_index] = states[state_index];
        SOC_LEDS->leds[motor_index] = colors[state_index];
        motor_index = (motor_index + (state_index == 15)) % 4;
        state_index = (state_index + 1) % 16;
        neorv32_uart_printf(NEORV32_UART0, "motor[%d]=%X (%d)\n", motor_index, states[state_index], state_index);
        rt_task_sleep_periodic(&tick, TEST_TASK_PERIOD);
    }
}

RT_TASK(test, RT_STACK_MIN, RT_TASK_PRIORITY_MIN);

//
// Speed Printout
//

#define PRINT_TASK_PERIOD MS_TO_TICKS(250)

static void print(void)
{
    uint32_t tick = 0;
    int32_t prev_pos[4] = { 0 };
    for (;;)
    {
        rt_task_sleep_periodic(&tick, PRINT_TASK_PERIOD);
        neorv32_uart_printf(NEORV32_UART0, "speed: ");
        for (int i = 0; i < 4; i++)
        {
            int32_t pos = SOC_MOTORS->positions[i];
            int32_t speed = pos - prev_pos[i];
            neorv32_uart_printf(NEORV32_UART0, "%d\t", speed);
            prev_pos[i] = pos;
        }
        neorv32_uart_printf(NEORV32_UART0, "\n");
    }
}

RT_TASK(print, RT_STACK_MIN, RT_TASK_PRIORITY_MIN);

//
// Init
//

void mcu_init(void)
{
    neorv32_uart_setup(NEORV32_UART0, 115200, ~0);
    neorv32_uart_puts(NEORV32_UART0, ":: Warehouse BOT - Motor Test ::\n");
}
