#pragma once

#include <stdint.h>

#define SOC_SERVOS_BASE 0xF0000200UL

__attribute__ ((packed, aligned(4)))
struct servo_regs
{
    uint32_t servos[2];
};

#define SOC_SERVOS ((volatile struct servo_regs*)SOC_SERVOS_BASE)

