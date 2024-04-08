#pragma once

#include <neorv32.h>

#include <stdint.h>

#include <soc/motors.h>

#include <rt/rwlock.h>

#define SYS_MOTOR_CONTROL_BIT 0
#define SYS_SERVER_BIT 1
#define SYS_POS_CONTROL_BIT 2

#define CAR_SYSTEMS_READY 0b0011

#define MOTOR_SPEED_SAMPLES 4

enum car_state
{
    CAR_STATE_IDLE = 0,
    CAR_STATE_MANUAL = 1,
    CAR_STATE_AUTO = 2,
    CAR_STATE_FAULT = 3
};

struct global_state
{
    enum car_state state;

    // bit mask of all system statuses (1=running, 0=not running)
    uint32_t systems;

    struct position
    {
        int16_t x;
        int16_t y;
        int16_t r;
    } pos;
    
    struct motors_state
    {
        struct rt_rwlock rwlock;
        int32_t positions[SOC_MOTOR_COUNT];
        int32_t speed_samples[SOC_MOTOR_COUNT][MOTOR_SPEED_SAMPLES];
        int32_t current_speeds[SOC_MOTOR_COUNT];
        int32_t target_speeds[SOC_MOTOR_COUNT];
        int32_t errors[SOC_MOTOR_COUNT];

        int32_t con_speeds[SOC_MOTOR_COUNT];
        int32_t imu_speeds[SOC_MOTOR_COUNT];
    } motors;
};


extern volatile struct global_state car;

void fault(void);

#define printf(fmt, ...) neorv32_uart0_printf(fmt, ## __VA_ARGS__)

