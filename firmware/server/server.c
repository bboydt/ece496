#include "server.h"

#include "rt_extra.h"
#include <rt/task.h>

static uint8_t esp_getc(int *state, int *data_len);
static int recv(struct packet *pck);
static void send(const struct packet *pck);

// create a task for this function
void server_run(int (*callback)(const struct packet *req_pck, struct packet *res_pck))
{
    struct packet req_pck, res_pck;
    for (;;)
    {
        if (recv(&req_pck))
        {
            if (callback(&req_pck, &res_pck))
            {
                send(&res_pck);
            }
        }

        rt_task_yield();
    }
}

// esp8266 parsing states
enum esp_parsing_state
{
    ESP_PARSING_STATE_READ_HEADER,
    ESP_PARSING_STATE_READ_LEN,
    ESP_PARSING_STATE_READ_DATA
};

// parses output from esp8266 to get packet
// blocks until start of transmission
// returns when byte in data is received
static uint8_t esp_getc(int *state, int *data_len)
{
    const char *header = "+IPD,";
    const char *header_cur = header;

    uint8_t c;
    while (1)
    {
        // block until byte received
        while ((ESP_UART->CTRL & (1<<UART_CTRL_RX_NEMPTY)) == 0)
            rt_task_yield();
        c = (uint8_t)(ESP_UART->DATA >> UART_DATA_RTX_LSB);

        switch (*state)
        {
            case ESP_PARSING_STATE_READ_HEADER:
                if (c == *header_cur)
                {
                    header_cur++;
                    if (*header_cur == '\00')
                    {
                        *state = ESP_PARSING_STATE_READ_LEN;
                    }
                }
                else
                {
                    header_cur = header;
                }
                break;

            case ESP_PARSING_STATE_READ_LEN:
                if (c < '0' || c > '9')
                {
                    // FAULT
                }

                if (c == ':')
                {
                    *state = ESP_PARSING_STATE_READ_DATA;
                }
                else
                {
                    int i = c - '0';
                    *data_len *= 10;
                    *data_len += i;
                }
                break;

            case ESP_PARSING_STATE_READ_DATA:
                if (*data_len > 0)
                {
                    (*data_len)--;
                    return c;
                }
                else
                {
                    header_cur = header;
                    *state = ESP_PARSING_STATE_READ_HEADER;
                }
                break;
        }
    }
}

enum packet_parsing_state
{
    PACKET_PARSING_STATE_READ_START,
    PACKET_PARSING_STATE_READ_PID,
    PACKET_PARSING_STATE_READ_LEN,
    PACKET_PARSING_STATE_READ_DATA
};

// writes recieved packet into pck
// blocks until packet received
static int recv(struct packet *pck)
{
    int state = ESP_PARSING_STATE_READ_HEADER;
    int data_len = 0;
    int bytes_read;
    uint8_t c;
    while (1)
    {
        c = esp_getc(&state, &data_len);

        switch (state)
        {
            case PACKET_PARSING_STATE_READ_START:
            {
                if (c == PACKET_START)
                {
                    state = PACKET_PARSING_STATE_READ_PID;
                }
                break;
            }

            case PACKET_PARSING_STATE_READ_PID:
            {
                pck->pid = c;
                state = PACKET_PARSING_STATE_READ_LEN;
                break;
            }

            case PACKET_PARSING_STATE_READ_LEN:
            {
                pck->len = c;
                bytes_read = 0;
                state = PACKET_PARSING_STATE_READ_DATA;
                break;
            }
            
            case PACKET_PARSING_STATE_READ_DATA:
            {
                pck->data[bytes_read++] = c;
                if (bytes_read == pck->len)
                {
                    return 1;
                }
            }
        }
    }
}

// sends packet to esp
static void send(const struct packet *pck)
{
    neorv32_uart_printf(ESP_UART, "AT+CIPSEND=%d\r\n", pck->len + 3);
    neorv32_uart_putc(ESP_UART, 'B');
    neorv32_uart_putc(ESP_UART, pck->pid);
    neorv32_uart_putc(ESP_UART, pck->len);
    for (int i = 0; i < pck->len; i++)
        neorv32_uart_putc(ESP_UART, pck->data[i]);
}

