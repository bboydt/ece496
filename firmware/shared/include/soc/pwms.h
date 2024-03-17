#pragma once

#define PWM_BASE ((uint32_t)0xF0000200UL)
#define PWMS ((struct pwm_regs*)PWM_BASE)

#define PWM_CTRLS_CLKDIV_OFFS 0
#define PWM_CTRLS_CLKDIV_MASK 0x0000000FUL
#define PWM_CTRLS_CLKDIV(n) (PWM_CTRLS_CLKDIV_MASK & ((n) << PWM_CTRLS_CLKDIV_OFFS))

#define PWM_CTRLS_EN_OFFS 4
#define PWM_CTRLS_EN_MASK 0x00000010UL
#define PWM_CTRLS_EN(n) (PWM_CTRLS_EN_MASK & ((n) << PWM_CTRLS_EN_OFFS))

#define PWM_VAL_CMP_OFFS 0
#define PWM_VAL_CMP_MASK 0x0000FFFFUL
#define PWM_VAL_CMP(n) (PWM_VAL_CMP_MASK & ((n) << PWM_VAL_CMP_OFFS))

#define PWM_VAL_MAX_OFFS 16
#define PWM_VAL_MAX_MASK 0xFFFF0000UL
#define PWM_VAL_MAX(n) (PWM_VAL_MAX_MASK & ((n) << PWM_VAL_MAX_OFFS))

struct pwm_regs
{
    uint32_t ctrls[4];
    uint32_t vals[4];
};
