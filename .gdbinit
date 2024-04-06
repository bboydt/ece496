target extended-remote :3333
file build/fw/app/walleos/walleos.elf
set remote hardware-breakpoint-limit 1

mem 0x00000000 0x00004000 ro 32
mem 0x80000000 0x80008000 rw 32
mem 0xe0800000 0xe0c00000 ro 32
mem 0xF0000000 0xF1000000 ro 32

set pagination off
define hook-quit
    set confirm off
end
