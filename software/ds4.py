#!/usr/bin/env python3


import time
import hid 
from dataclasses import dataclass
import socket

s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s.connect(("192.168.10.130", 1011))

gamepad = hid.device()
gamepad.open(0x054c, 0x09cc)

@dataclass
class AxisState:
    x: int
    y: int

    def __init__(self, x, y, center = 128):
        self.x = x - center
        self.y = y - center

@dataclass
class GamepadState:
    left: AxisState
    right: AxisState

    def __init__(self, raw):
        self.left = AxisState(raw[1], raw[2])
        self.right = AxisState(raw[3], raw[4])

state = GamepadState([0, 128, 128, 128, 128])
try:
    while True:
        raw_state = gamepad.read(64, 1000/20)
        if raw_state:
            state = GamepadState(raw_state)
            #s.send(b"Hello")

        if abs(state.left.x) > 50:
            if state.left.x < 0:
                s.send(bytes("a", encoding="utf8"))
                print("a")
            else:
                s.send(bytes("d", encoding="utf8"))
                print("d")
#
#        if abs(state.left.y) > 50:
#            if state.left.y > 0:
#                s.send(bytes("s", encoding="utf8"))
#                print("s")
#            else:
#                s.send(bytes("w", encoding="utf8"))
#                print("w")

except KeyboardInterrupt:
    gamepad.close()
    s.disconnect()
