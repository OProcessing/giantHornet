#include "hardware_imu.h"
#include "filter_Kalman.h"
#include "function_pid.h"
#include "function_compute.h"
#include "define.h"
#include "function_compute.h"
#include "define.h"

static PID_t pid_roll, pid_pitch, pid_yaw;

void control_loop_init(void) 
{
    PID_Init(&pid_roll,  10.0f, 0.0f, 0.05f, -500, 500);
    PID_Init(&pid_pitch, 10.0f, 0.0f, 0.05f, -500, 500);
    PID_Init(&pid_yaw,   10.0f, 0.0f, 0.00f, -500, 500);
}

void motor_SetPWM(int motor, int pwm)
{
    motor_debug(motor, pwm);
}

void motor_debug(int motor, int pwm)
{
    int prev_pwm = 0;
    static int count = 0;
    if (count++ % 10 == 0)
    {
        if (motor == 1 && prev_pwm != pwm)
        {
            prev_pwm = pwm;
            LOG_DEBUG("Setting motor %d PWM to %d", motor, pwm);
        }
    }
    return;
}

void control_loop(SPI_HandleTypeDef *SPIx, MPU9250_t *pMPU9250, Filtered_t *filtered) 
{
    // LOG_TRACE("control_loop() called");

    MPU_getData(SPIx, pMPU9250);
    // LOG_DEBUG("Raw Data: ax=%d, ay=%d, az=%d, gx=%d, gy=%d, gz=%d",
        // pMPU9250->rawData.ax, pMPU9250->rawData.ay, pMPU9250->rawData.az,
        // pMPU9250->rawData.gx, pMPU9250->rawData.gy, pMPU9250->rawData.gz);

    KalmanFilter_Update(pMPU9250, filtered);

    // LOG_DEBUG("Sensor Data: ax=%.2f, ay=%.2f, az=%.2f, gx=%.2f, gy=%.2f, gz=%.2f",
        // pMPU9250->sensorData.ax, pMPU9250->sensorData.ay, pMPU9250->sensorData.az,
        // pMPU9250->sensorData.gx, pMPU9250->sensorData.gy, pMPU9250->sensorData.gz);

    float target_roll = 0.0f;
    float target_pitch = 0.0f;
    float target_yaw = 0.0f;

    float roll_output  = PID_Compute(&pid_roll,  target_roll,  filtered->attitude.roll);
    float pitch_output = PID_Compute(&pid_pitch, target_pitch, filtered->attitude.pitch);
    float yaw_output   = PID_Compute(&pid_yaw,   target_yaw,   filtered->attitude.yaw);
    // LOG_DEBUG("PID Outputs: roll=%.2f, pitch=%.2f, yaw=%.2f", 
        // roll_output, pitch_output, yaw_output);

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
