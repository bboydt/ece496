#pragma once

#include <stdint.h>

#define LEDS_BASE ((uint32_t)0xF0000100UL)

struct led_regs
{
    uint32_t led[7];
};

#define LEDS ((struct led_regs*) LEDS_BASE)
