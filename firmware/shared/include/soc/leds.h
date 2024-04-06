#pragma once

#include <stdint.h>

#define SOC_LEDS_BASE 0xF0000000UL

#define SOC_LED_COUNT 7

struct led_regs
{
    uint32_t leds[SOC_LED_COUNT];
};

#define SOC_LEDS ((volatile struct led_regs*)SOC_LEDS_BASE)
