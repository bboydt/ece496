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

#define AXIS_MAP_CONFIG 0x41
#define AXIS_MAP_SIGN 0x42

#define IMU_TASK_PERIOD MS_TO_TICKS(1000/10)

static void imu(void)
{
    // give the IMU time to turn on
    rt_task_sleep(MS_TO_TICKS(2000));

    // go into config mode
    i2c_write_u8(IMU_ADDR, OPR_MODE, 0x00);
    rt_task_sleep(MS_TO_TICKS(100));
    i2c_write_u8(IMU_ADDR, AXIS_MAP_CONFIG, 0b000110);
    i2c_write_u8(IMU_ADDR, AXIS_MAP_SIGN, 0x00);
    
    // go into sensor fusion mode
    i2c_write_u8(IMU_ADDR, OPR_MODE, 0x08);
    rt_task_sleep(MS_TO_TICKS(100));

    uint32_t tick = 0;
    for (;;)
    {
        rt_task_sleep_periodic(&tick, IMU_TASK_PERIOD);
    }
}

RT_TASK(imu, RT_STACK_MIN, RT_TASK_PRIORITY_MIN);


