#pragma once

#include <stdint.h>

enum car_mode
{
    // Yellow LED
    CAR_MODE_IDLE = 0,
    
    // Blue LED
    CAR_MODE_MANUAL = 1,
    
    // Green LED
    CAR_MODE_AUTO = 2,

    // Red LED
    CAR_MODE_FAULT = 3,
};

#define CAR_MDOE_LAST CAR_MODE_FAULT

enum car_task
{
    CAR_TASK_IDLE = 0,
    CAR_TASK_GOTO_PICK = 1,
    CAR_TASK_GOTO_FIND_BOX = 2,
    CAR_TASK_GOTO_PICK_BOX = 3,
    CAR_TASK_GOTO_DROP = 4,
    CAR_TASK_GOTO_DROP_BOX = 5,
};

#define SPEED_SAMPLES 8

struct car_state
{
    volatile uint8_t mode;

    int32_t current_x;
    int32_t current_y;
    int32_t current_r;

    int32_t target_x;
    int32_t target_y;
    int32_t target_r;

    uint8_t pinch;
    uint8_t z_dir;

    volatile int32_t target_speeds[4];
    int32_t current_speeds_pre_avg[4][SPEED_SAMPLES];
    volatile int32_t current_speeds[4];
    volatile int32_t positions[4];

    int32_t errors[4];
    int32_t error_sums[4];

    uint32_t motor_pwm_widths[4];
};

extern struct car_state car;

