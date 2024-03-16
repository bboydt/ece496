#pragma once

#define ENCODERS_BASE ((uint32_t)0xF0000000UL)

struct encoder_regs
{
    uint32_t positions[4];
};

#define ENCODERS ((struct encoder_regs*) ENCODERS_BASE)

