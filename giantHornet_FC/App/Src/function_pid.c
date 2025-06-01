#include "function_pid.h"
#include "define.h"

void PID_Init(PID_t *pid, float Kp, float Ki, float Kd, float output_min, float output_max) 
{
    pid->Kp = Kp;
    pid->Ki = Ki;
    pid->Kd = Kd;
    pid->dt = IMU_DT;

    pid->prev_error = 0.0f;
    pid->integral = 0.0f;

    pid->output_min = output_min;
    pid->output_max = output_max;
}

float PID_Compute(PID_t *pid, float setpoint, float measurement) {
    float error = setpoint - measurement;
    float derivative = (error - pid->prev_error) / pid->dt;

    float P_term = pid->Kp * error;
    float D_term = pid->Kd * derivative;
    float I_term = pid->Ki * pid->integral;

    float output = P_term + I_term + D_term;
    float output_sat = PID_clamp(output, pid->output_max, pid->output_min);

    // Back-calculation anti-windup
    float error_sat = output - output_sat;
    pid->integral += (error * pid->dt) - (pid->Kaw * error_sat * pid->dt);

    pid->prev_error = error;
    return output_sat;
}

float PID_clamp(float value, float clamp_upper, float clamp_lower) 
{
    if (value > clamp_upper) return clamp_upper;
    else if (value < clamp_lower) return clamp_lower;
    else return value;
}