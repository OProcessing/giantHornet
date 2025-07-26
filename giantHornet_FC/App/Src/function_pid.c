#include "function_pid.h"
#include "define.h"

/**
 * @brief PID Init
 * @param pid PID_t structure pointer
 * @param Kp Proportional gain
 * @param Ki Integral gain
 * @param Kd Derivative gain
 * @param output_min Output minimum value
 * @param output_max Output maximum value
 */
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

/**
 * @brief PID Compute
 * @param pid PID_t structure pointer
 * @param setpoint Setpoint value
 * @param measurement Measurement value
 * @return PID output
 */
float PID_Compute(PID_t *pid, float setpoint, float measurement) {
    float error = setpoint - measurement;
    float derivative = (error - pid->prev_error) / pid->dt;
    LOG_DEBUG("PID Compute: setpoint=%.2f, measurement=%.2f, error=%.2f, derivative=%.2f", 
        setpoint, measurement, error, derivative);

    float P_term = pid->Kp * error;
    float I_term = pid->Ki * pid->integral;
    float D_term = pid->Kd * derivative;

    float output = P_term + I_term + D_term;
    float output_sat = PID_clamp(output, pid->output_max, pid->output_min);
    LOG_DEBUG("PID Output after clamp: %.2f", output_sat);

    // Back-calculation anti-windup
    float error_sat = output - output_sat;
    pid->integral += (error * pid->dt) - (pid->Kaw * error_sat * pid->dt);

    pid->prev_error = error;
    return output_sat;
}

/**
 * @brief PID Clamp
 * @param value Value to clamp
 * @param clamp_upper Clamp upper value
 * @param clamp_lower Clamp lower value
 * @return Clamped value
 */
float PID_clamp(float value, float clamp_upper, float clamp_lower) 
{
    if (value > clamp_upper) return clamp_upper;
    else if (value < clamp_lower) return clamp_lower;
    else return value;
}