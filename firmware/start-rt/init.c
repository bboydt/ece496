#include <neorv32.h>

#include "common.h"


void _init(void)
{
    // set mtime to fire in 1ms
    neorv32_mtime_set_timecmp(neorv32_mtime_get_time() + TICK_PERIOD);
    neorv32_cpu_csr_set(CSR_MIE, 1 << CSR_MIE_MTIE);

    mcu_init();
}

