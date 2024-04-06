#pragma once

// called by _init(), intended for device initialization
void mcu_init(void);

// configures tick rate of rt
#define MS_PER_TICK 10
#define MS_TO_TICKS(n) ((n) / MS_PER_TICK)
#define TICKS_TO_MS(n) ((n) * MS_PER_TICK)

