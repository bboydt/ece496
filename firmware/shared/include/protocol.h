#pragma once

#define PACKET_START 'B'
#define PACKET_MAX_DATA_LEN 256

#define PACKET_PID_HEARTBEAT 0x00
#define PACKET_PID_COMMAND 0x01
#define PACKET_PID_MESSAGE 0x02
#define PACKET_PID_SET_MODE 0x03
#define PACKET_PID_CONTROLLER 0x04
#define PACKET_PID_LAST PACKET_PID_CONTROLLER

void handle_packet(uint8_t pid, uint8_t len, const uint8_t *data);

