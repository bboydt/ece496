#!/usr/bin/env python3

from socket import socket, AF_INET, SOCK_DGRAM
from protocol import Packet, PacketId as PID
from cmd import Cmd
from threading import Thread
from hid import device as HidDevice
from enum import Enum
from dataclasses import dataclass

class Modes(Enum):
    IDLE = 0
    MANUAL = 1

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


class ClientApp(Cmd):

    mode = Modes.IDLE

    def poll_controller(self):
        while True:
            if (self.mode == Modes.MANUAL):
                raw_state = self.gamepad.read(64, 1000/20)
                if raw_state:
                    state = GamepadState(raw_state)
                    self.send(Packet.create(PID.CONTROLLER, b"".join([raw_state[1].to_bytes(1), raw_state[2].to_bytes(1), raw_state[3].to_bytes(1), raw_state[4].to_bytes(1)])))

    def preloop(self):
        self.prompt = "\033[1;37m(unknown)\033[0m "
        self.intro = ":: Warehouse Bot Client ::"
        self.sock = socket(AF_INET, SOCK_DGRAM)
        self.address = ("10.121.1.251", 42069)

        self.controller_thread = Thread(target=ClientApp.poll_controller, args=(self,), daemon=True)
        self.controller_thread.start()

    def postloop(self):
        if self.sock is not None:
            self.sock.close()

    def do_clear(self, line):
        print("\033c")

    def do_quit(self, line):
        print("Goodbye.")

    def do_cmd(self, line):
        packet = Packet.create(PID.COMMAND, bytes(line, encoding="utf8"))
        self.send(packet)

    def do_mode(self, line):
        if (line == "idle"):
            self.send(Packet.create(PID.SET_MODE, b"\x00"))
            self.prompt = "\033[1;31m(idle)\033[0m "
            self.mode = Modes.IDLE

        elif (line == "manual"):
            try:
                self.gamepad = HidDevice()
                self.gamepad.open(0x054C, 0x09CC)
                # if we got a gamepad, then set the mode
                self.send(Packet.create(PID.SET_MODE, b"\x01"))
                self.prompt = "\033[1;34m(manual)\033[0m "
                self.mode = Modes.MANUAL
            except OSError:
                print("\033[1;31mERROR:\033[0m Failed to find controller.")

    def send(self, packet: bytes):
        self.sock.sendto(packet, self.address)

#data, addr = self.sock.recvfrom(1024)

if __name__ == "__main__":
    client = ClientApp()
    try:
        client.cmdloop()
    except KeyboardInterrupt:
        client.postloop()
