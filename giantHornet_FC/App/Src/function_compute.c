#include "hardware_imu.h"
#include "filter_Kalman.h"
#include "function_pid.h"
#include "function_compute.h"
#include "define.h"

static PID_t pid_roll, pid_pitch, pid_yaw;

void control_loop_init(float dt) 
{
    PID_Init(&pid_roll,  10.0f, 0.0f, 0.05f, dt, -500, 500);
    PID_Init(&pid_pitch, 10.0f, 0.0f, 0.05f, dt, -500, 500);
    PID_Init(&pid_yaw,   10.0f, 0.0f, 0.00f, dt, -500, 500);
}

void motor_SetPWM(int motor, int pwm)
{
    int prev_pwm = 0;
    if (motor == 1 && prev_pwm != pwm)
    {
        prev_pwm = pwm;
        LOG_INFO("Setting motor %d PWM to %d", motor, pwm);
    }
    return;
}

void control_loop(SPI_HandleTypeDef *SPIx, MPU9250_t *imu) 
{
    LOG_TRACE("control_loop() called");

    MPU_readProcessedData(SPIx, imu);
    // LOG_DEBUG("Raw Data: ax=%d, ay=%d, az=%d, gx=%d, gy=%d, gz=%d",
        // imu->rawData.ax, imu->rawData.ay, imu->rawData.az,
        // imu->rawData.gx, imu->rawData.gy, imu->rawData.gz);

    KalmanFilter_Update(imu, imu->attitude.dt);
    // LOG_DEBUG("Sensor Data: ax=%.2f, ay=%.2f, az=%.2f, gx=%.2f, gy=%.2f, gz=%.2f",
        // imu->sensorData.ax, imu->sensorData.ay, imu->sensorData.az,
        // imu->sensorData.gx, imu->sensorData.gy, imu->sensorData.gz);

    float target_roll = 0.0f;
    float target_pitch = 0.0f;
    float target_yaw = 0.0f;

    float roll_output  = PID_Compute(&pid_roll,  target_roll,  imu->attitude.r);
    float pitch_output = PID_Compute(&pid_pitch, target_pitch, imu->attitude.p);
    float yaw_output   = PID_Compute(&pid_yaw,   target_yaw,   imu->attitude.y);
    LOG_DEBUG("PID Outputs: roll=%.2f, pitch=%.2f, yaw=%.2f", 
        roll_output, pitch_output, yaw_output);

    int throttle = 1500;    
    int motor1 = throttle + roll_output - pitch_output + yaw_output;
    int motor2 = throttle - roll_output - pitch_output - yaw_output;
    int motor3 = throttle - roll_output + pitch_output + yaw_output;
    int motor4 = throttle + roll_output + pitch_output - yaw_output;

    motor_SetPWM(1, motor1);
    motor_SetPWM(2, motor2);
    motor_SetPWM(3, motor3);
    motor_SetPWM(4, motor4);
    return;
} 

