# CCP - Car Control Protocol

DEFAULT_PORT 42069

START_TOKEN = b'B'

PID_HEARTBEAT = 0x00
PID_STATUS = 0x01
PID_SET_PARAM = 0x02
PID_CONTROLLER = 0x03
PID_LOG_MESSAGE = 0x04

PARAM_MODE = 0x00
PARAM_DEBUG = 0x01
PARAM_HOME_X = 0x02
PARAM_HOME_Y = 0x03
PARAM_HOME_R = 0x04
PARAM_PICK_X = 0x05
PARAM_PICK_Y = 0x06
PARAM_PICK_R = 0x07
PARAM_DROP_X = 0x08
PARAM_DROP_Y = 0x09
PARAM_DROP_R = 0x0A

def create_packet(pid: int, data: bytes):
    return b''.join([START_TOKEN, pid.to_bytes(1), len(data).to_bytes(1), data])

# returns (pid, data)
def parse_packet(packet: bytes):
    return (int.from_bytes(packet[1], 'little'), packet[3:])
