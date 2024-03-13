#pragma once

#define TICK_PERIOD (NEORV32_SYSINFO->CLK / 100) 

// called by _init(), intended for device initialization
void mcu_init(void);
