#include "hardware_imu.h"
#include "filter_Kalman.h"
#include "function_pid.h"

static PID_t pid_roll, pid_pitch, pid_yaw;
static FilteredAttitude_t filtered_attitude;

void control_loop_init(void) 
{
    PID_Init(&pid_roll,  10.0f, 0.0f, 0.05f, -500, 500);
    PID_Init(&pid_pitch, 10.0f, 0.0f, 0.05f, -500, 500);
    PID_Init(&pid_yaw,   1.0f, 0.0f, 0.00f, -500, 500);
}

void Motor_SetPWM(int motor, int pwm)
{
    if (motor < 1)
    {
        printf("Motor %d: %d\t", motor, pwm);
    }
    else
    {
        printf("Motor %d: %d\r\n", motor, pwm);
    }
}

void control_loop_1khz(SPI_HandleTypeDef *SPIx, MPU9250_t *imu) 
{
    MPU_readProcessedData(SPIx, imu);
    KalmanFilter_Update(imu, &filtered_attitude);

    float target_roll = 0.0f;
    float target_pitch = 0.0f;
    float target_yaw = 0.0f;

    float roll_output  = PID_Compute(&pid_roll,  target_roll,  filtered_attitude.roll);
    float pitch_output = PID_Compute(&pid_pitch, target_pitch, filtered_attitude.pitch);
    float yaw_output   = PID_Compute(&pid_yaw,   target_yaw,   filtered_attitude.yaw);

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
