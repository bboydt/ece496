#pragma once

#include <stdint.h>

#define SOC_SERVOS_BASE 0xF0000200UL

struct motor_regs
{
    uint32_t servos[2];
};

#define SOC_SERVOS ((volatile struct motor_regs*)SOC_SERVOS_BASE)

