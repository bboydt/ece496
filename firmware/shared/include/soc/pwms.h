#pragma once

#include <stdint.h>

#define PWM_BASE ((uint32_t)0xF0000200UL)
#define PWMS ((struct pwm_regs*)PWM_BASE)

struct pwm_regs
{
    uint32_t duty_cycles[4];
};
