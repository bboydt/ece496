#pragma once

#define TICK_PERIOD (NEORV32_SYSINFO->CLK / 10) 

// called by _init(), intended for device initialization
void mcu_init(void);
