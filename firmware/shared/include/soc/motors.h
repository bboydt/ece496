#pragma once

#include <stdint.h>

#define SOC_MOTORS_BASE 0xF0000100UL

#define SOC_MOTOR_COUNT 4

struct motor_regs
{
    uint32_t motors[SOC_MOTOR_COUNT];
    int32_t positions[SOC_MOTOR_COUNT];
};

#define SOC_MOTORS ((volatile struct motor_regs*)SOC_MOTORS_BASE)
