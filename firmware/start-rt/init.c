#include <neorv32.h>

#include <rt/tick.h>

#include "common.h"

#define TICK_PERIOD (NEORV32_SYSINFO->CLK / 100) 

__attribute__((interrupt("machine"))) void riscv_mtvec_mti(void)
{
    neorv32_mtime_set_timecmp(neorv32_mtime_get_timecmp() + TICK_PERIOD);
    rt_tick_advance();
}

void _init(void)
{
    mcu_init();

    // set mtime to fire in 1ms
    neorv32_mtime_set_timecmp(neorv32_mtime_get_time() + TICK_PERIOD);
    neorv32_cpu_csr_set(CSR_MIE, 1 << CSR_MIE_MTIE);
    neorv32_cpu_csr_set(CSR_MIE, 1 << CSR_MIE_MSIE);
}

