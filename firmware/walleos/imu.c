#include <stdint.h>

#include <neorv32.h>

#include <soc/leds.h>
#include <i2c.h>

#include <rt/task.h>
#include <rt/stack.h>

#include "rt_extra.h"
#include "car.h"

#define IMU_ADDR 0x28

#define CHIP_ID 0x00
#define CALIB_STAT 0x35
#define OPR_MODE 0x3D
#define UNIT_SEL 0x3B

#define EUL_X 0x1A
#define EUL_Y 0x1C
#define EUL_Z 0x1E

#define QUAT_W 0x20
#define QUAT_X 0x22
#define QUAT_Y 0x24
#define QUAT_Z 0x26

#define LIN_ACC_X 0x28
#define LIN_ACC_Y 0x2A
#define LIN_ACC_Z 0x2C

#define AXIS_MAP_CONFIG 0x41
#define AXIS_MAP_SIGN 0x42

#define IMU_TASK_PERIOD MS_TO_TICKS(1000/10)

#define MAX_SPEED 20
#define DEG_DIV 4
#define DEG_MUL (16/DEG_DIV)
#define KP 80
#define KD 8
#define KI 128

static int32_t clamp(int32_t x, int32_t min, int32_t max)
{
    return (x < min) ? min : ((x > max) ? max : x);
}

static void imu(void)
{
    // give the IMU time to turn on
    rt_task_sleep(MS_TO_TICKS(5000));

    // go into config mode
    i2c_write_u8(IMU_ADDR, OPR_MODE, 0x00);
    rt_task_sleep(MS_TO_TICKS(100));
    i2c_write_u8(IMU_ADDR, AXIS_MAP_CONFIG, 0b000000);
    i2c_write_u8(IMU_ADDR, AXIS_MAP_SIGN, 0x00);
    
    // go into sensor fusion mode
    i2c_write_u8(IMU_ADDR, OPR_MODE, 0x08);
    rt_task_sleep(MS_TO_TICKS(1000));

    int16_t angle, error, prev_error = 0, diff;
    int32_t speed = 0;
    int16_t target = 0;

    uint32_t tick = 0;
    for (;;)
    {
        angle = i2c_read_s16(IMU_ADDR, EUL_X)/DEG_DIV;
        error = target - angle;
        if (error < -180*DEG_MUL)
            error = error + 360*DEG_MUL;
        else if (error > 180*DEG_MUL)
            error = error - 360*DEG_MUL;


        diff = error - prev_error;
        prev_error = error;


        speed += error/KP + diff/KD;
        speed = clamp(speed, -MAX_SPEED, MAX_SPEED);

        car.motors.imu_speeds[0] = -speed;
        car.motors.imu_speeds[1] = -speed;
        car.motors.imu_speeds[2] = speed;
        car.motors.imu_speeds[3] = speed;

        rt_task_sleep_periodic(&tick, IMU_TASK_PERIOD);
    }
}

RT_TASK(imu, 32<<10, RT_TASK_PRIORITY_MIN);


