#!/usr/bin/env python3

from socket import socket, AF_INET, SOCK_DGRAM
from protocol import Packet, PacketId as PID

class ServerApp():

    def __init__(self):
        self.sock = socket(AF_INET, SOCK_DGRAM)
        self.address = ("localhost", 42069)
        self.sock.bind(self.address)

    def run(self):
        try:
            while True:
                data, addr = self.sock.recvfrom(1024)
                print(f"\033[1;35mPACKET:\033[0m addr={addr} data={data.hex()}")

                if (data[0] != Packet.MAGIC):
                    print(f"\033[1;31mERROR:\033[0m Invalid packet.")
                else:
                    if (data[1] == PID.HEARTBEAT.value):
                        print(f"\033[1;35mPACKET:\033[0m Heartbeat.")
                        self.sock.sendto(Packet.create(PID.HEARTBEAT, b"\x00"), addr)

        except KeyboardInterrupt:
            self.cleanup()

    def cleanup(self):
        if self.sock is not None:
            self.sock.close()

if __name__ == "__main__":
    ServerApp().run()

