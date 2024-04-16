#!/usr/bin/env python3

import serial
import enum

s = serial.Serial('/dev/cu.usbserial-TG1109110', 115200)

class States(enum.Enum):
    READ_HEADER_1 = 0
    READ_HEADER_2 = 1
    READ_DIST_L = 2
    READ_DIST_H = 3
    READ_STRENGTH_L = 4
    READ_STRENGTH_H = 5
    READ_TEMP_L = 6
    READ_TEMP_H = 7
    READ_CHECKSUM = 8

try:
    state = States.READ_HEADER_1

    while True:
        packet = s.read_until(b'\x59\x59')
        dist = int.from_bytes(packet[0:2], 'little')
        strength = int.from_bytes(packet[2:4], 'little')
        temp = int.from_bytes(packet[4:6], 'little')
        print(f'dist={dist}\tstrength={strength}\ttemp={temp}')
except KeyboardInterrupt:
    s.close()

