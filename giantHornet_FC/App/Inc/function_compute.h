#ifndef CONTROL_LOOP_H
#define CONTROL_LOOP_H

#include "hardware_imu.h"

// 제어 루프 초기화 (PID 등)
void control_loop_init(float dt);

// 1kHz 제어 루프 (IMU→칼만→PID→모터)
void control_loop_1khz(SPI_HandleTypeDef *SPIx, MPU9250_t *imu, float dt);

#endif // CONTROL_LOOP_H 