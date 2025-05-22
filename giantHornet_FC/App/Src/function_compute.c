#include "hardware_imu.h"
#include "filter_Kalman.h"
#include "function_pid.h"
#include "motor.h"

static PID_t pid_roll, pid_pitch, pid_yaw;

void control_loop_init(float dt) 
{
    PID_Init(&pid_roll,  1.0f, 0.0f, 0.05f, dt, -500, 500);
    PID_Init(&pid_pitch, 1.0f, 0.0f, 0.05f, dt, -500, 500);
    PID_Init(&pid_yaw,   1.0f, 0.0f, 0.00f, dt, -500, 500);
}

void motor_SetPWM(int motor, int pwm)
{
    return;
}

void control_loop_1khz(SPI_HandleTypeDef *SPIx, MPU9250_t *imu, float dt) 
{
    MPU_readProcessedData(SPIx, imu);

    KalmanFilter_Update(imu, dt);

    float target_roll = 0.0f;
    float target_pitch = 0.0f;
    float target_yaw = 0.0f;

    float roll_output  = PID_Compute(&pid_roll,  target_roll,  imu->attitude.r);
    float pitch_output = PID_Compute(&pid_pitch, target_pitch, imu->attitude.p);
    float yaw_output   = PID_Compute(&pid_yaw,   target_yaw,   imu->attitude.y);

    int throttle = 1500;
    int motor1 = throttle + roll_output - pitch_output + yaw_output;
    int motor2 = throttle - roll_output - pitch_output - yaw_output;
    int motor3 = throttle - roll_output + pitch_output + yaw_output;
    int motor4 = throttle + roll_output + pitch_output - yaw_output;

    Motor_SetPWM(1, motor1);
    Motor_SetPWM(2, motor2);
    Motor_SetPWM(3, motor3);
    Motor_SetPWM(4, motor4);
} 

