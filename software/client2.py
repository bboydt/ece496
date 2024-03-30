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

class States(Enum):
    COMMAND = 0
    OPTIONS = 1
    
class CarConnection():

    def __init__(self, address):
        ...

    def __del__(self):
        ...

@dataclass
class ControllerState():
    packet: list
    dpad: int
    triangle: bool
    circle: bool
    cross: bool
    square: bool

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
                self.callback(ControllerState(packet, dpad, triangle, circle, cross, square))
        # when the loop is done, disconnect
        self.controller.close()

class CLI():
    DISCONNECTED_COLOR = (30, 47)
    IDLE_COLOR = (30, 43)
    MANUAL_COLOR = (30, 44)
    AUTO_COLOR = (30, 42)

    def __init__(self):
        self.stop_requested = False
        self.print_lock = Lock()
        self.prompt = '\033[30m>\033[0m '
        self.motd = 'Type \'help\' for a list of commands. Press C-d to exit.'
        self.size = os.get_terminal_size()
        self.controller = Controller(self, self.handle_controller)
        self.state = States.COMMAND
        self.header_color = (30, 47)
        self.prev_controller_state = ControllerState([], 0, False, False, False, False)
        self.menu_items = ['option a', 'option b', 'option c']
        self.menu_selection = 0
        self.set_header('Disconnected')
        self.init_commands()

    def __del__(self):
        ...

    #
    # Main Loop
    # 

    # runs main loop
    def run(self):
        # clear screen and go to second line
        self.print('\033[2J\033[1;0H')
        # print motd
        self.print(self.motd)
        # check for controller
        if not self.controller.connect():
            self.print_warning('Controller not connected.')

        # do main loop
        try:
            while not self.stop_requested:
                self.draw_header()
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


    #
    # Header
    #

    # redraws the header
    def draw_header(self):
        str = ''
        str += '\0337' # save cursor pos
        str += '\033[H' # goto 0,0
        str += self.header + '\n'
        str += '\0338' # restore cursor pos
        self.print(str, end='')

    # sets the header with padding and full length background
    def set_header(self, header, color=None, prefix='Warehouse Bot v3 | '):
        # set color if provided
        if color is not None:
            self.header_color = color
        # update header
        header = prefix + header
        self.header = f'\033[1;{self.header_color[0]};{self.header_color[1]}m '
        self.header += header
        self.header += ' ' * (self.size[0] - len(header) - 1)
        self.header += '\033[0m'

    def update_option_header(self):
        header = 'Make a section: '
        for i in range(0, len(self.menu_items)):
            if i == self.menu_selection:
                header += f'\033[{self.header_color[0]+10};{self.header_color[1]-10}m'
                header += ' ' + self.menu_items[i] + ' '
                header += f'\033[{self.header_color[0]};{self.header_color[1]}m'
            else:
                header += ' ' + self.menu_items[i] + ' '

        self.set_header(header, prefix = '')
        self.draw_header()

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
        self.menu_selection = (self.menu_selection + d) % len(self.menu_items)
        self.update_option_header()

    #
    # Controller
    #

    def handle_controller(self, state):
        if state.dpad != self.prev_controller_state.dpad:
            match state.dpad:
                case 2:
                    self.update_selection(+1)
                case 6:
                    self.update_selection(-1)

        self.prev_controller_state = state


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
        self.set_header('Connected', color=self.IDLE_COLOR)
        self.print('Connected.')

    def disconnect(self, args):
        self.print('Disconnected.')
        self.set_header('Disconnected', color=self.DISCONNECTED_COLOR)

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


if __name__ == '__main__':
    cli = CLI()
    cli.run()

