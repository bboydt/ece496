#!/usr/bin/env python3

# Car UI

import os

from enum import Enum
from time import sleep
from threading import Lock, Thread
from socket import socket as Socket, AF_INET, SOCK_DGRAM
from hid import device as HIDDevice, enumerate as hid_enumerate
from dataclasses import dataclass

PORT = 42069

PID_HEARTBEAT = 0x00
PID_STATUS = 0x01
PID_SET_PARAM = 0x02
PID_CONTROLLER = 0x03
PID_LOG_MESSAGE = 0x04


class CarConnection():

    def __init__(self, address):
        ...

    def __del__(self):
        ...

#
# Controller 
#

@dataclass
class ControllerState():
    packet: list
    dpad: int
    triangle: bool
    circle: bool
    cross: bool
    square: bool
    left_x: int
    left_y: int
    right_x: int
    right_y: int

class Controller():

    def __init__(self, parent, callback, pid=0x054C, vid=0x09CC):
        self.parent = parent
        self.callback = callback
        self.vid = pid
        self.pid = vid
        self.controller = None
        self.should_disconnect = False
        self.poll_thread = None

    def __del__(self):
        if self.controller is not None:
            self.controller.close()

    def connect(self):
        if self.is_connected():
            self.parent.print_error('Already connected to controller.')
            return False

        devices = hid_enumerate(vendor_id=self.vid, product_id=self.pid)
        if len(devices) > 0:
            device = devices[0]
            self.controller = HIDDevice()
            self.controller.open(device['vendor_id'], device['product_id'])

            self.should_disconnect = False
            self.poll_thread = Thread(target=Controller.poll_packets, args=(self,))
            self.poll_thread.start()
            
            self.parent.print(f'Controller connected: \'{device['product_string']}\'')
            return True
        else:
            return False
    
    def disconnect(self):
        if self.is_connected():
            self.should_disconnect = True
            self.poll_thread.join()

    def is_connected(self):
        return self.poll_thread is not None and self.poll_thread.is_alive()

    def poll_packets(self):
        while not self.should_disconnect:
            packet = self.controller.read(64, 500)
            if packet:
                dpad = packet[5] & 0x0F
                triangle = (packet[5] & (1<<7)) != 0
                circle = (packet[5] & (1<<6)) != 0
                cross = (packet[5] & (1<<5)) != 0
                square = (packet[5] & (1<<4)) != 0
                left_x = packet[1]
                left_y = packet[2]
                right_x = packet[3]
                right_y = packet[4]
                self.callback(ControllerState(packet, dpad, triangle, circle, cross, square, left_x, left_y, right_x, right_y))
        # when the loop is done, disconnect
        self.controller.close()




#
# Command Line Interface (CLI)
#

class CLIStates(Enum):
    NORMAL = 0,
    SETTINGS = 1,
    SET_MODE = 2,
    SAVE_POS = 3,
    GOTO_POS = 4

class CLI():
    DISCONNECTED_COLOR = (30, 47)
    IDLE_COLOR = (30, 43)
    MANUAL_COLOR = (30, 44)
    AUTO_COLOR = (30, 42)

    SETTINGS_MENU = ('Settings: ', ['set mode', 'save position'])
    MODES_MENU = ('Select Mode: ', ['idle', 'manual', 'auto'])
    POSITIONS_MENU = ('Select Position: ', ['home', 'pick', 'drop'])

    def __init__(self):

        # state
        self.stop_requested = False
        self.state = CLIStates.SETTINGS

        # menu
        self.menu = self.SETTINGS_MENU
        self.menu_index = 0

        # terminal
        self.print_lock = Lock()
        self.prompt = '\033[30m>\033[0m '
        self.motd = 'Type \'help\' for a list of commands. Press C-d to exit.'
        self.size = os.get_terminal_size()
        self.header_color = (30, 47)

        # controller
        self.controller = Controller(self, self.handle_controller)
        self.prev_controller_state = ControllerState([], 0, False, False, False, False, 0, 0, 0, 0)

        self.menu_items = ['set mode', '', 'option c']
        self.menu_selection = 0
        #self.set_header('Disconnected')
        self.init_commands()
        self.ping = 0
    

    def __del__(self):
        ...

    #
    # Main Loop
    # 

    # runs main loop
    def run(self):
        self.sock = Socket(AF_INET, SOCK_DGRAM)
        self.address = ("10.121.1.251", 42069)
        self.packet_thread = Thread(target=CLI.receive_packets, args=(self,))
        self.packet_thread.start()

        # clear screen and go to second line
        self.print('\033[2J\033[1;0H')
        # print motd
        self.print(self.motd)
        # check for controller
        if not self.controller.connect():
            self.print_warning('Controller not connected.')
        else:
            self.print('Press \'options\' on controller to open actions menu.')

        # do main loop
        try:
            while not self.stop_requested:
                self.redraw()
                self.print(self.prompt, end='')
                command = input()
                args = command.split()
                if len(args) != 0:
                    self.exec(args)
        except (KeyboardInterrupt, EOFError):
            ...

        # cleanup
        self.print('goodbye.')
        self.controller.disconnect()

    def redraw(self):
        match self.state:
            case CLIStates.NORMAL:
                self.draw_header(prefix='Warehouse Bot v3 | ', body='TODO', tail='ping:100ms')
            case CLIStates.SETTINGS | CLIStates.SET_MODE | CLIStates.SAVE_POS | CLIStates.GOTO_POS:
                self.ping += 1
                self.draw_header(prefix=self.menu[0], options=self.menu[1], tail=f'ping:{self.ping}ms')

    #
    # Header
    #

    def draw_header(self, prefix=None, body=None, options=None, tail=None):
        
        body_len = 0
        if self.state in [CLIStates.SETTINGS, CLIStates.SET_MODE, CLIStates.SAVE_POS, CLIStates.GOTO_POS]:
            items = [f' {x} ' for x in self.menu[1]]
            body_len = sum([len(x) for x in items])
            items[self.menu_index] = f'\033[40;37m{items[self.menu_index]}\033[30;47m'
            body = ''.join(items)
        else:
            body_len = len(body)

        spacer = ' ' * (self.size[0] - len(prefix) - body_len - len(tail))

        str = ''.join([
            '\0337',
            '\033[H',
            '\033[30;47m',
            prefix,
            body,
            spacer,
            tail,
            '\n',
            '\033[0m',
            '\0338'
        ])
        self.print(str, end='')

    #
    # Printing
    #

    # prints using lock
    def print(self, str, **kwargs):
        with self.print_lock:
            print(str, **kwargs)

    # prints an error
    def print_error(self, str):
        self.print('\033[1;31mERROR:\033[0m ' + str)

    # prints a warning
    def print_warning(self, str):
        self.print('\033[1;33mWARNING:\033[0m ' + str)

    #
    # Menu
    #

    def update_selection(self, d):
        self.menu_index = (self.menu_index + d) % len(self.menu[1])
        self.redraw()

    def handle_selection(self):
        match self.state:
            case CLIStates.SETTINGS:
                print('settings')
                match self.menu[1][self.menu_index]:
                    case 'set mode':
                        self.state = CLIStates.SET_MODE
                        self.menu = self.MODES_MENU
                        self.menu_index = 0
                    case 'save position':
                        self.state = CLIStates.SAVE_POS
                        self.menu = self.POSITIONS_MENU
                        self.menu_index = 0
                    case 'goto position':
                        self.state = CLIStates.GOTO_POS
                        self.menu = self.POSITIONS_MENU
                        self.menu_index = 0

            case CLIStates.SET_MODE:
                print('set mode')
                self.state = CLIStates.SETTINGS
                self.menu = self.SETTINGS_MENU
                self.menu_index = 0
            
            case CLIStates.SAVE_POS:
                print('save position')
                self.state = CLIStates.SETTINGS
                self.menu = self.SETTINGS_MENU
                self.menu_index = 0

            case CLIStates.GOTO_POS:
                print('goto position')
                self.state = CLIStates.SETTINGS
                self.menu = self.SETTINGS_MENU
                self.menu_index = 0

            case _:
                self.state = CLIStates.SETTINGS
                self.menu = self.SETTINGS_MENU
                self.menu_index = 0
        
        self.redraw()

    #
    # Controller
    #

    def handle_controller(self, state):
        #if state.dpad != self.prev_controller_state.dpad:
        #    match state.dpad:
        #        case 2:
        #            self.update_selection(+1)
        #        case 6:
        #            self.update_selection(-1)

        #if state.cross & (state.cross ^ self.prev_controller_state.cross):
        #    self.handle_selection()

        self.prev_controller_state = state

        buttons = 0
        buttons |= ((1<<0) if state.cross    else 0)
        buttons |= ((1<<1) if state.square   else 0)
        buttons |= ((1<<2) if state.triangle else 0)
        buttons |= ((1<<3) if state.circle   else 0)

        data_ints = [state.left_x, state.left_y, state.right_x, state.right_y, buttons]
        data = b''.join([x.to_bytes(1) for x in data_ints])
        self.send_packet(PID_CONTROLLER, data)

    #
    # Commands
    #

    def init_commands(self):
        commands = {
            'help': 'Prints the help message.',
            'quit': 'Exits the program.',
            'clear': 'Clears the screen buffer.',
            'connect': 'Attempts to connect to the car.',
            'disconnect': 'Closes connection to the car.',
            'ds4': 'Connects/disconnects to ds4 controller. Usage: ds4 [-d]',
            'heartbeat': 'Sends heartbeat packet to car.'
        }

        self.commands = {}
        for command, help_msg in commands.items():
            self.commands[command] = (getattr(self, command), help_msg)

    # execute a command
    def exec(self, args):
        fn = self.commands.get(args[0].lower(), (self.unknown_command,))[0]
        res = fn(args)
        if res is not None:
            self.stop_requested = res
    
    def unknown_command(self, args):
        self.print_error(f'Unkown command \'{args[0]}\'.\nSee \'help\' for a list of commands.')

    def help(self, args):
        str = '--- Commands ---'
        for cmd in self.commands:
            str += '\n' + cmd + ' - ' + self.commands[cmd][1]
        self.print(str)
    
    def quit(self, args):
        return True

    def clear(self, args):
        self.print('\033[2J\033[1;0H')

    def connect(self, args):
        self.print('Connecting...')
        #self.set_header('Connected', color=self.IDLE_COLOR)
        self.print('Connected.')

    def disconnect(self, args):
        self.print('Disconnected.')
        #self.set_header('Disconnected', color=self.DISCONNECTED_COLOR)

    def ds4(self, args):
        connect = True
        if len(args) > 1:
            if args[1].lower() == '-d':
                connect = False

        if connect:
            if not self.controller.connect():
                self.print_error('Failed to connect controller.')
        else:
            self.controller.disconnect()
            self.print('Controller disconnected.')

    def heartbeat(self, args):
        self.send_packet(PID_HEARTBEAT, (48).to_bytes(1))

    #
    # Packets
    #

    def receive_packets(self):

        while True:
            inbound = self.sock.recv(258);
            print(inbound)




    def send_packet(self, pid: int, data: bytes):
        packet = b''.join([b'B', pid.to_bytes(1), len(data).to_bytes(1), data])
        self.sock.sendto(packet, self.address)

if __name__ == '__main__':
    cli = CLI()
    cli.run()


