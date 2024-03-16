#pragma once

#define PWM_BASE ((uint32_t)0xF0000200)

#define PWM_MAX_OFFS

#define PWM_MAX_MASK 0xFFFF0000
#define PWM_MAX_MASK 0x0000FFFF

#define PWM_MAX(n) (PWM_MAX_MASK & ((n) << 0))
#define PWM_THRESHOLD(n) (PWM_THRESHOLD_MASK & ((n) << 16))

struct pwm_regs
{
    uint32_t pwm0;
    uint32_t pwm1;
};
