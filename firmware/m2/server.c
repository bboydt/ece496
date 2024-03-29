#include <neorv32.h>

#include <rt/task.h>

#include <stdint.h>
#include <stdio.h>

#include <soc/common.h>
#include <soc/leds.h>
#include <soc/pwms.h>

#include "protocol.h"
#include "car.h"

#define CONTROLLER_BUTTON_TRIANGLE (1<<0)
#define CONTROLLER_BUTTON_CIRCLE (1<<1)
#define CONTROLLER_BUTTON_CROSS (1<<2)
#define CONTROLLER_BUTTON_SQUARE (1<<3)
#define CONTROLLER_BUTTON_R1 (1<<4)
#define CONTROLLER_BUTTON_L1 (1<<5)
#define CONTROLLER_BUTTON_DPAD_UP (1<<6)
#define CONTROLLER_BUTTON_DPAD_DOWN (1<<7)
#define CONTROLLER_BUTTON_DPAD_RIGHT (1<<8)
#define CONTROLLER_BUTTON_DPAD_LEFT (1<<9)
#define CONTROLLER_BUTTON_RIGHT_JOY_STICK (1<<10)
#define CONTROLLER_BUTTON_LEFT_JOY_STICK (1<<11)


struct controller
{
    int8_t x0;
    int8_t y0;
};

void handle_packet(uint8_t pid, uint8_t len, const uint8_t *data)
{
    switch (pid)
    {
        case PACKET_PID_HEARTBEAT:
            break;
        
        case PACKET_PID_COMMAND:
            break;
        
        case PACKET_PID_MESSAGE:
            // FAULT
            break;

        case PACKET_PID_SET_MODE:
            if (len == 1)
            {
                neorv32_uart_printf(NEORV32_UART0, "SET_MODE=%c\n", data[0] + '0');
                uint8_t mode = data[0];
                car.mode = mode;
            }
            else
            {
                // FAULT
            }
            break;

        case PACKET_PID_CONTROLLER:
            {
                int32_t x0 = data[0] - 128;
                int32_t y0 = data[1] - 128;
                int32_t x1 = (data[2] - 128)/2;
                int32_t y1 = data[3] - 128;
                
                if (abs(x0) > abs(y0))
                {
                    // sideways
                    car.target_speeds[0] = x0 + x1;
                    car.target_speeds[1] = -x0 + x1;
                    car.target_speeds[2] = -x0 - x1;
                    car.target_speeds[3] = x0 - x1;
                }
                else
                {
                    // stright
                    car.target_speeds[0] = y0 + x1;
                    car.target_speeds[1] = y0 + x1;
                    car.target_speeds[2] = y0 - x1;
                    car.target_speeds[3] = y0 - x1;
                }

                if (abs(y1) > 32)
                {
                    car.pinch += (y1 > 0);
                    if (car.pinch < 8)
                    {
                        car.pinch = 8;
                    }
                    else if (car.pinch > 20)
                    {
                        car.pinch = 20;
                    }
                    PWMS->duty_cycles[4] = car.pinch;
                }

                neorv32_uart_printf(NEORV32_UART0, "x0=%d\ty0=%d\tx1=%d\ty1=%d\n", x0, y0, x1, y1);
            }
            break;

    }
}


#define esp32_uart NEORV32_UART1

enum server_states
{
    SERVER_STATE_READ_START = 0,
    SERVER_STATE_READ_PID   = 1,
    SERVER_STATE_READ_LEN   = 2,
    SERVER_STATE_READ_DATA  = 3
};

enum server_uart_states
{
    SERVER_UART_STATE_READ_HEADER = 0,
    SERVER_UART_STATE_READ_LEN = 1,
    SERVER_UART_STATE_READ_DATA = 2
};

static uint8_t uart_getc(neorv32_uart_t *UARTx)
{
    while (1) {
        LEDS->led[3] ^= 0x00FF00;
        if (UARTx->CTRL & (1<<UART_CTRL_RX_NEMPTY)) { // data available?
            return (char)(UARTx->DATA >> UART_DATA_RTX_LSB);
        }
    }
}

static uint8_t server_uart_getc()
{
    static int state = SERVER_UART_STATE_READ_HEADER;
    static uint32_t data_len = 0;

    const char *header = "+IPD,";
    const char *header_cur = header;

    uint8_t c;

    while (1)
    {
        switch (state)
        {
            case SERVER_UART_STATE_READ_HEADER:
                c = uart_getc(esp32_uart);
                if (c == *header_cur)
                {
                    header_cur++;
                    if (*header_cur == '\00')
                    {
                        state = SERVER_UART_STATE_READ_LEN;
                    }
                }
                else
                {
                    header_cur = header;
                }
                break;

            case SERVER_UART_STATE_READ_LEN:
                c = uart_getc(esp32_uart);
                
                if (c < '0' || c > '9')
                {
                    // FAULT
                }

                if (c == ':')
                {
                    state = SERVER_UART_STATE_READ_DATA;
                }
                else
                {
                    int i = c - '0';
                    data_len *= 10;
                    data_len += i;
                }
                break;

            case SERVER_UART_STATE_READ_DATA:
                if (data_len > 0)
                {
                    data_len--;
                    c = uart_getc(esp32_uart);
                    return c;
                }
                else
                {
                    header_cur = header;
                    state = SERVER_UART_STATE_READ_HEADER;
                }
                break;
        }
    }
}

static void server(void)
{
    LEDS->led[2] = 0xFFFFFF;
    neorv32_uart_puts(esp32_uart, "ATE0\r\n");
    rt_task_sleep(MS_TO_TICKS(1000));
    //neorv32_uart_puts(esp32_uart, "AT+CIPCLOSE\r\n");
    //rt_task_sleep(MS_TO_TICKS(1000));
    neorv32_uart_puts(esp32_uart, "AT+CIPSTART=\"UDP\",\"0.0.0.0\",161,42069,2\r\n");
    rt_task_sleep(MS_TO_TICKS(1000));
        
    uint8_t c, pid, len, data[PACKET_MAX_DATA_LEN];
    size_t bytes_read;

    int state = SERVER_STATE_READ_START;
    for (;;)
    {
        c = server_uart_getc();

        switch (state)
        {
            case SERVER_STATE_READ_START:
            {
                if (c == PACKET_START)
                {
                    state = SERVER_STATE_READ_PID;
                }
                break;
            }

            case SERVER_STATE_READ_PID:
            {
                pid = c;
                state = SERVER_STATE_READ_LEN;
                break;
            }

            case SERVER_STATE_READ_LEN:
            {
                len = c;
                bytes_read = 0;
                state = SERVER_STATE_READ_DATA;
                break;
            }
            
            case SERVER_STATE_READ_DATA:
            {
                data[bytes_read++] = c;
                if (bytes_read == len)
                {
                    handle_packet(pid, len, data);
                    state = SERVER_STATE_READ_START;
                }
                break;
            }
        }
    }
}

RT_TASK(server, 8<<10, RT_TASK_PRIORITY_MAX);

void server_send(uint8_t pid, uint8_t len, const uint8_t *data)
{
    neorv32_uart_putc(esp32_uart, PACKET_START);
    neorv32_uart_putc(esp32_uart, pid);
    neorv32_uart_putc(esp32_uart, len);
    for (int i = 0; i < len; i++)
    {
        neorv32_uart_putc(esp32_uart, data[i]);
    }
}

