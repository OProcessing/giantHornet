#ifndef CONTROL_LOOP_H
#define CONTROL_LOOP_H

#include "hardware_imu.h"

// 제어 루프 초기화 (PID 등)
void control_loop_init(void);
void motor_SetPWM(int motor, int pwm);
void motor_debug(int motor, int pwm);

// 1kHz 제어 루프 (IMU→칼만→PID→모터)
void control_loop(SPI_HandleTypeDef *SPIx, MPU9250_t *pMPU9250, Filtered_t *filtered);

#endif // CONTROL_LOOP_H 