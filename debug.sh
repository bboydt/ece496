# Runs OpenOCD and GDB

# run OpenOCD
openocd -f openocd_neorv32.cfg &
openocd_pid=$!

# run GDB
riscv64-unknown-elf-gdb -q

# close OpenOCD
kill -9 $openocd_pid

