#ifndef INC_FUNCTION_PID_H_
#define INC_FUNCTION_PID_H_

typedef struct {
    float Kp;       
    float Ki;       
    float Kd;
    float Kaw;  //  1/Ki

    float prev_error;
    float integral;

    float output_max;
    float output_min;

    float dt;
} PID_t;


void PID_Init(PID_t *pid, float Kp, float Ki, float Kd, float output_min, float output_max);
float PID_Compute(PID_t *pid, float setpoint, float measurement);
float PID_clamp(float value, float clamp_upper, float clamp_lower);

#endif /* INC_FUNCTION_PID_H_ */
