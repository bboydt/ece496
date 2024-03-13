target extended-remote :3333
file build/fw/app/blinky/blinky.elf
set remote hardware-breakpoint-limit 1
mem 0x00000000 0x00004000 ro
mem 0x00004000 0x00008000 rw
mem 0x0e080000 0x0e0c0000 ro
