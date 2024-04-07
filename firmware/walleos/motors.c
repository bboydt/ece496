#include "rt_extra.h"
#include <rt/task.h>
#include <rt/stack.h>

#include <stdlib.h>

#include <soc/motors.h>

#include "car.h"

#define KP 8
#define KD 2

static int32_t clamp(int32_t x, int32_t min, int32_t max)
{
    return (x < min) ? min : ((x > max) ? max : x);
}

#define CONTROL_MOTOR_TASK_PERIOD MS_TO_TICKS(1000/20)
static void control_motors(void)
{
    uint32_t tick = 0;

    int32_t position;
    int sample_index = 0;
    int32_t error;
    int32_t diff;
    uint32_t motor_state;

    // set state to ready and wait for other systems to come online
    do
    {
        car.systems |= (1<<SYS_MOTOR_CONTROL_BIT);
        rt_task_sleep(MS_TO_TICKS(100));
    }
    while (car.systems != CAR_SYSTEMS_READY);

    for (;;)
    {
        //rt_rwlock_wrlock(&car.motors.rwlock);
        
        // calculate speeds
        for (int i = 0; i < SOC_MOTOR_COUNT; i++)
        {
            // the "average" here isn't a true average
            // it is just a sum of samples without normalization

            position = SOC_MOTORS->positions[i];
            // remove previous sample from average
            car.motors.current_speeds[i] -= car.motors.speed_samples[sample_index];
            // calculate speed
            car.motors.speed_samples[sample_index] = (position - car.motors.positions[i]);
            // add new sample to average
            car.motors.current_speeds[i] += car.motors.speed_samples[sample_index];
            // store position
            car.motors.positions[i] = position;
        }
        sample_index = (sample_index + 1) % MOTOR_SPEED_SAMPLES;


        // run control loops
        for (int i = 0; i < SOC_MOTOR_COUNT; i++)
        {
            error = abs(car.motors.target_speeds[i]) - abs(car.motors.current_speeds[i]);
            diff = error - car.motors.errors[i];
            car.motors.errors[i] = error;

            // get current motor pwm, the rest will be regenerated
            motor_state = SOC_MOTORS->motors[i] & 0xFF;
            // determine next duty cycle (do not modify motor_state before doing this)
            motor_state |= clamp(motor_state + error/KP + diff/KD, 0x00, 0xFF);
            // only run motor if target speed magnitude > 10
            motor_state |= abs((car.motors.target_speeds[i]) > 10) << 31;
            // determine direction motor should run
            motor_state |= (car.motors.target_speeds[i] > 0) << 30;
            SOC_MOTORS->motors[i] = motor_state;
        }

        //rt_rwlock_wrunlock(&car.motors.rwlock);
    
        rt_task_sleep_periodic(&tick, CONTROL_MOTOR_TASK_PERIOD);
    }
}
RT_TASK(control_motors, RT_STACK_MIN, RT_TASK_PRIORITY_MIN);

