#include <neorv32.h>

#include <rt/task.h>

#include <soc/leds.h>

#include <server.h>


#include "rt_extra.h"
#include "car.h"

volatile struct global_state car;

//
// Initialization
//

void mcu_init(void)
{
    // Startup Lights
    for (int i = 0; i < SOC_LED_COUNT; i++)
        SOC_LEDS->leds[i] = 0xFFFF00;

    // UART
    neorv32_uart_setup(NEORV32_UART0, 115200, ~0);
    neorv32_uart_setup(NEORV32_UART1, 115200, ~0);
    neorv32_uart_puts(NEORV32_UART0, ":: Wall-E OS - v1 ::\n");

    // I2C
    neorv32_twi_setup(CLK_PRSC_128, 1, 1);

    // State
    car.systems = 0;
}

//
// Parameters
//

static uint8_t *param_ptrs[PARAM_LAST + 1] = 
{
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

static int set_param(uint8_t param, uint8_t value)
{
    if (param > PARAM_LAST)
        return 0;

    uint8_t *param_ptr = param_ptrs[param];
    if (param_ptr != NULL)
        *param_ptr = value;

    return 1;
}

//
// Packet Handling
//

int handle_packet(const struct packet *req_pck, struct packet *res_pck)
{
    switch (req_pck->pid)
    {
        case PID_HEARTBEAT:
            res_pck->pid = PID_HEARTBEAT | 0x80;
            res_pck->len = 1;
            res_pck->data[0] = req_pck->data[1] + 1;
            return 1;
        case PID_STATUS:
            res_pck->pid = PID_STATUS | 0x80;
            res_pck->len = 5;
            res_pck->data[0] = req_pck->data[1] + 1;
            res_pck->data[1] = car.state;
            res_pck->data[2] = car.pos.x;
            res_pck->data[3] = car.pos.y;
            res_pck->data[4] = car.pos.r;
            return 1;
        case PID_SET_PARAM:
            res_pck->pid = PID_SET_PARAM | 0x80;
            res_pck->len = 2;
            res_pck->data[0] = req_pck->data[1] + 1;
            res_pck->data[1] = set_param(req_pck->data[1], req_pck->data[2]);
            return 1;
        case PID_CONTROLLER:
            {
                int32_t x0 = req_pck->data[0] - 128;
                int32_t y0 = req_pck->data[1] - 128;
                int32_t x1 = (req_pck->data[2] - 128)/2;
                //int32_t y1 = req_pck->data[3] - 128;
                
                if (abs(x1) <= 10) x1 = 0;
                if (abs(x0) > abs(y0))
                {
                    // sideways
                    if (abs(x0) <= 10) x0 = 0;
                    car.motors.con_speeds[0] = -x0 - x1;
                    car.motors.con_speeds[1] =  x0 - x1;
                    car.motors.con_speeds[2] = -x0 + x1;
                    car.motors.con_speeds[3] =  x0 + x1;
                }
                else
                {
                    // stright
                    if (abs(y0) <= 10) y0 = 0;
                    car.motors.con_speeds[0] = -y0 - x1;
                    car.motors.con_speeds[1] = -y0 - x1;
                    car.motors.con_speeds[2] = -y0 + x1;
                    car.motors.con_speeds[3] = -y0 + x1;
                }
            }
            return 0;
        default:
            return 0;
    }
}

RT_TASK(server_run, 8<<10, 0);
