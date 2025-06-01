#ifndef INC_FILTER_KALMAN_H_
#define INC_FILTER_KALMAN_H_

#include "hardware_imu.h"

// 필터링된 자세 데이터를 저장할 구조체
typedef struct {
    float roll;    // 칼만필터로 필터링된 Roll 각도
    float pitch;   // 칼만필터로 필터링된 Pitch 각도
    float yaw;     // 자이로 적분으로 계산된 Yaw 각도
} FilteredAttitude_t;

void KalmanFilter_Update(MPU9250_t *pMPU9250, FilteredAttitude_t *filtered);

#endif /* INC_FILTER_KALMAN_H_ */
