#include <neorv32.h>

#include "rt_extra.h"
#include <rt/tick.h>

#define TICK_PERIOD (MS_PER_TICK * (NEORV32_SYSINFO->CLK / 1000))

void mti_handler(void)
{
    neorv32_mtime_set_timecmp(neorv32_mtime_get_timecmp() + TICK_PERIOD);
    rt_tick_advance();
}

extern void mcu_init(void);

void _init(void)
{
    mcu_init();

    // set mtime to fire in 1ms
    neorv32_mtime_set_timecmp(neorv32_mtime_get_time() + TICK_PERIOD);
    neorv32_cpu_csr_set(CSR_MIE, 1 << CSR_MIE_MTIE);
}

