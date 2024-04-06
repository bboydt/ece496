#pragma once

#include <neorv32.h>
#include <stdint.h>

#include "car.h"

#define PACKET_START 'B'
#define PACKET_MAX_LEN 256

#define PID_HEARTBEAT   0x00
#define PID_STATUS      0x01
#define PID_SET_PARAM   0x02
#define PID_CONTROLLER  0x03
#define PID_LOG_MESSAGE 0x04

#define PARAM_MODE   0x00
#define PARAM_DEBUG  0x01
#define PARAM_HOME_X 0x02
#define PARAM_HOME_Y 0x03
#define PARAM_HOME_R 0x04
#define PARAM_PICK_X 0x05
#define PARAM_PICK_Y 0x06
#define PARAM_PICK_R 0x07
#define PARAM_DROP_X 0x08
#define PARAM_DROP_Y 0x09
#define PARAM_DROP_R 0x0A

#define ESP_UART NEORV32_UART1

struct packet
{
    uint8_t pid;
    uint8_t len;
    uint8_t data[0xFF];
};

void server_run(int (*callback)(const struct packet *req_pck, struct packet *res_pck));
