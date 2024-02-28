#!/usr/bin/env python3

import time
import hid 
from dataclasses import dataclass

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
        raw_state = gamepad.read(64, 10)
        if raw_state:
            state = GamepadState(raw_state)

        if abs(state.left.x) > 50:
            if state.left.x < 0:
                print("a")
            else:
                print("d")

        if abs(state.left.y) > 50:
            if state.left.y > 0:
                print("s")
            else:
                print("w")

except KeyboardInterrupt:
    gamepad.close()
