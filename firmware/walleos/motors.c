#include "rt_extra.h"
#include <rt/task.h>
#include <rt/stack.h>

#include <stdlib.h>

#include <soc/motors.h>

#include "car.h"

#define KP 2
#define KD 2

int32_t clamp(int32_t x, int32_t min, int32_t max)
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

    for (int i = 0; i < SOC_MOTOR_COUNT; i++)
    {
        car.motors.positions[i] = 0;
        car.motors.current_speeds[i] = 0;
        car.motors.target_speeds[i] = 0;

        car.motors.con_speeds[i] = 0;

        for (int j = 0; j < MOTOR_SPEED_SAMPLES; j++)
        {
            car.motors.speed_samples[i][j] = 0;
        }
    }

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
            car.motors.current_speeds[i] -= car.motors.speed_samples[i][sample_index];
            // calculate speed
            car.motors.speed_samples[i][sample_index] = (position - car.motors.positions[i]);
            // add new sample to average
            car.motors.current_speeds[i] += car.motors.speed_samples[i][sample_index];
            // store position
            car.motors.positions[i] = position;
        }

        // run control loops
        for (int i = 0; i < SOC_MOTOR_COUNT; i++)
        {
            car.motors.target_speeds[i] = car.motors.con_speeds[i]/4 + ((i <= 1) ? -car.motors.imu_speed : car.motors.imu_speed);

            error = car.motors.target_speeds[i] - car.motors.current_speeds[i];
            diff = error - car.motors.errors[i];
            car.motors.errors[i] = error;
            
            car.motors.pwms[i] = clamp(car.motors.pwms[i] + error/KP + diff/KD, -255, 255);

            // get current motor pwm, the rest will be regenerated
            motor_state = SOC_MOTORS->motors[i] & 0xFF;
            // determine next duty cycle (do not modify motor_state before doing this)
            motor_state = abs(car.motors.pwms[i]);
            // only run motor if target speed magnitude > 10. lol jk actually crank that soulja boy
            motor_state |= 1 << 31;
            // determine direction motor should run
            motor_state |= (car.motors.pwms[i] > 0) << 30;
            SOC_MOTORS->motors[i] = motor_state;
        }

        sample_index = (sample_index + 1) % MOTOR_SPEED_SAMPLES;

        //rt_rwlock_wrunlock(&car.motors.rwlock);
    
        rt_task_sleep_periodic(&tick, CONTROL_MOTOR_TASK_PERIOD);
    }
}

RT_TASK(control_motors, 8<<10, 1);

