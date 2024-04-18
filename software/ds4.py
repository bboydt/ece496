#!/usr/bin/env python3


import time
import hid 
from dataclasses import dataclass
import socket

gamepad = hid.device()
gamepad.open(0x054c, 0x09cc)

try:
    while True:
        raw_state = gamepad.read(64, 1000/20)
        if raw_state:
            print(raw_state)

except KeyboardInterrupt:
    gamepad.close()
    s.disconnect()
