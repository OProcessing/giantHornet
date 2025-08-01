#include "hardware_imu.h"
#include "filter_Kalman.h"
#include "define.h"
#include <math.h>

#define Q_ANGLE 0.001f   //  노이즈 공분산 (각도)
#define Q_BIAS  0.003f   //  노이즈 공분산 (바이어스)
#define R_MEASURE 0.03f  // 측정 노이즈 공분산

// Roll/Pitch용 칼만필터 상태 변수
typedef struct {
    float angle; // 추정 각도
    float bias;  // 자이로 바이어스
    float rate;  // 자이로로부터의 각속도
    float P[2][2]; // 오차 공분산 행렬
} Kalman_t;

static Kalman_t kalmanRoll = {0};
static Kalman_t kalmanPitch = {0};

// 칼만필터 업데이트 함수 (1축)
static float Kalman_getAngle(Kalman_t *kalman, float newAngle, float newRate) {
    // 예측 단계
    kalman->rate = newRate - kalman->bias;
    kalman->angle += IMU_DT * kalman->rate;

    // 오차 공분산 예측
    kalman->P[0][0] += IMU_DT * (IMU_DT*kalman->P[1][1] - kalman->P[0][1] - kalman->P[1][0] + Q_ANGLE);
    kalman->P[0][1] -= IMU_DT * kalman->P[1][1];
    kalman->P[1][0] -= IMU_DT * kalman->P[1][1];
    kalman->P[1][1] += Q_BIAS * IMU_DT;

    // 측정 업데이트
    float S = kalman->P[0][0] + R_MEASURE;
    float K[2];
    K[0] = kalman->P[0][0] / S;
    K[1] = kalman->P[1][0] / S;
    float y = newAngle - kalman->angle;
    kalman->angle += K[0] * y;
    kalman->bias  += K[1] * y;
    float P00_temp = kalman->P[0][0];
    float P01_temp = kalman->P[0][1];
    kalman->P[0][0] -= K[0] * P00_temp;
    kalman->P[0][1] -= K[0] * P01_temp;
    kalman->P[1][0] -= K[1] * P00_temp;
    kalman->P[1][1] -= K[1] * P01_temp;
    return kalman->angle;
}

/**
 * @brief Kalman filter update
 * @details Update Attitude_t structure
 * @param pMPU9250 MPU9250 structure pointer
 * @param attitude Attitude_t structure pointer
 * @return Attitude_t structure
 */
void KalmanFilter_Update(MPU9250_t *pMPU9250, Filtered_t *filtered)
{
    // 가속도 기반 roll, pitch 계산
    MPU_calcAccelAngles(pMPU9250, filtered);

    // 자이로 각속도 (deg/s)
    filtered->gyro.roll  = pMPU9250->sensorData.gy;
    filtered->gyro.pitch = pMPU9250->sensorData.gx;
    filtered->gyro.yaw   = pMPU9250->sensorData.gz;

    // 칼만필터 적용하여 필터링된 attitude 계산
    filtered->attitude.roll  = Kalman_getAngle(&kalmanRoll,  filtered->accel.roll,  filtered->gyro.roll);
    filtered->attitude.pitch = Kalman_getAngle(&kalmanPitch, filtered->accel.pitch, filtered->gyro.pitch);
    filtered->attitude.yaw   = pMPU9250->attitude.yaw;  // Yaw는 자이로 적분값 사용
}
