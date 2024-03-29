# Provides packet ID's for CarProtocol

from enum import Enum

class PacketId(Enum):
    HEARTBEAT = 0x00
    COMMAND = 0x01
    MESSAGE = 0x02
    SET_MODE = 0x03
    CONTROLLER = 0x04

class Packet():
    MAGIC = 0x42

    @classmethod
    def create(cls, pid: PacketId, data: bytes):
        return b''.join([cls.MAGIC.to_bytes(1), pid.value.to_bytes(1), len(data).to_bytes(1), data])
