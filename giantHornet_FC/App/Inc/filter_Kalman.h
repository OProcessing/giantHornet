#ifndef INC_FILTER_KALMAN_H_
#define INC_FILTER_KALMAN_H_

#include "hardware_imu.h"

void KalmanFilter_Update(MPU9250_t *pMPU9250, Filtered_t *filtered);

#endif /* INC_FILTER_KALMAN_H_ */
