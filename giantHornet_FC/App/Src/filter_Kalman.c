#include "hardware_imu.h"
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
static float Kalman_getAngle(Kalman_t *kalman, float newAngle, float newRate, float dt) {
    // 예측 단계
    kalman->rate = newRate - kalman->bias;
    kalman->angle += dt * kalman->rate;

    // 오차 공분산 예측
    kalman->P[0][0] += dt * (dt*kalman->P[1][1] - kalman->P[0][1] - kalman->P[1][0] + Q_ANGLE);
    kalman->P[0][1] -= dt * kalman->P[1][1];
    kalman->P[1][0] -= dt * kalman->P[1][1];
    kalman->P[1][1] += Q_BIAS * dt;

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

void KalmanFilter_Update(MPU9250_t *pMPU9250, float dt)
{
    // 가속도 기반 roll, pitch 계산
    float accelRoll  = atan2f(pMPU9250->sensorData.ay, sqrtf(pMPU9250->sensorData.az * pMPU9250->sensorData.az + pMPU9250->sensorData.ax * pMPU9250->sensorData.ax)) * 180.0f / PI;
    float accelPitch = atan2f(-pMPU9250->sensorData.ax, sqrtf(pMPU9250->sensorData.ay * pMPU9250->sensorData.ay + pMPU9250->sensorData.az * pMPU9250->sensorData.az)) * 180.0f / PI;

    // 자이로 각속도 (deg/s)
    float gyroRollRate  = pMPU9250->sensorData.gy;
    float gyroPitchRate = pMPU9250->sensorData.gx;
    float gyroYawRate   = pMPU9250->sensorData.gz;

    // 칼만필터 적용
    float roll  = Kalman_getAngle(&kalmanRoll,  accelRoll,  gyroRollRate,  dt);
    float pitch = Kalman_getAngle(&kalmanPitch, accelPitch, gyroPitchRate, dt);

    // Yaw는 자이로 적분
    static float yaw = 0.0f;
    yaw += gyroYawRate * dt;

    pMPU9250->attitude.r = roll;
    pMPU9250->attitude.p = pitch;
    pMPU9250->attitude.y = yaw;
}
