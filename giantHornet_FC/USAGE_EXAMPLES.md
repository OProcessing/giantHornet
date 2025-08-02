# 중앙 집중식 시간 관리 시스템 사용 가이드

## 📋 개요

모든 센서 데이터 로깅이 SD 카드 모듈에서 중앙 집중적으로 관리됩니다. GPS 시간 정보를 기준으로 모든 센서 데이터가 동기화되어 로깅됩니다.

## 🔧 사용 방법

### 1. **헤더 파일 포함**
```c
#include "hardware_sd_card.h"
```

### 2. **센서 데이터 로깅**

#### **IMU 센서**
```c
// IMU 데이터 처리 후 로깅
SD_LogInternalIMU(&filtered_attitude);
```

#### **압력 센서**
```c
// 압력 데이터 처리 후 로깅
SD_LogInternalPressure(pressure, temperature);
```

#### **모터 제어**
```c
// 모터 제어 데이터 로깅
SD_LogMotorControl(throttle_value, motor_rpm);
```

#### **거리 센서**
```c
// 거리 센서 데이터 로깅
SD_LogDistanceSensor(distance);
```

#### **온도 센서**
```c
// 온도 센서 데이터 로깅
SD_LogTemperatureSensor(temperature);
```

#### **배터리 전압**
```c
// 배터리 전압 로깅
SD_LogBatteryVoltage(voltage);
```

#### **시스템 상태**
```c
// 시스템 상태 로깅
SD_LogSystemStatus(status_code, "Status message");
```

#### **FC 내부 센서 데이터**
```c
// Raw IMU 데이터 로깅
SD_LogRawIMU(&MPU9250);

// 칼만 필터링된 데이터 로깅
SD_LogKalmanFiltered(&filtered_attitude);

// 모터 입력값 로깅
uint16_t motor_inputs[4] = {1500, 1500, 1500, 1500};
SD_LogMotorInputs(motor_inputs);

// PID 컨트롤러 출력 로깅
float pid_outputs[4] = {0.0f, 0.0f, 0.0f, 0.5f}; // roll, pitch, yaw, throttle
SD_LogPIDOutputs(pid_outputs);

// 비행 제어기 상태 로깅
SD_LogFlightControllerState(flight_mode, armed, altitude, vertical_speed);
```

#### **빈도 제어 로깅 **
```c
// 고빈도 로깅 (1kHz) - IMU 데이터 (실제 샘플링 레이트)
SD_LogRawIMU_Freq(&MPU9250, 1000);
SD_LogKalmanFiltered_Freq(&filtered_attitude, 1000);

// 중빈도 로깅 (500Hz) - 제어 데이터 (모터 PWM 주기와 동기화)
SD_LogMotorInputs_Freq(motor_inputs, 500);
SD_LogPIDOutputs_Freq(pid_outputs, 500);

// 저빈도 로깅 (1Hz) - 시스템 상태
SD_LogFlightControllerState_Freq(flight_mode, armed, altitude, vertical_speed, 1);
```

### 3. **시간 정보 활용**

#### **현재 시간 가져오기**
```c
uint32_t timestamp = SD_GetCurrentTimestamp();      // 초 단위
uint16_t ms = SD_GetCurrentTimestampMS();           // 밀리초 단위
```

#### **시간 정보 (최적화됨)**
```c
uint32_t timestamp = SD_GetCurrentTimestamp();      // 초 단위
uint16_t ms = SD_GetCurrentTimestampMS();           // 밀리초 단위

// 로그 분석 시에만 포맷팅 필요
// 실제 센서 로깅에서는 timestamp와 ms만 사용하여 오버헤드 최소화
```

### 4. **메인 루프에서 사용 예시**

```c
while (1) {
    // IMU 데이터 처리 및 로깅 (control_loop 내부에서 자동 로깅)
    control_loop(&hspi2, &MPU9250, &filtered_attitude);
    
    // 시스템 상태 로깅 (1Hz)
    SD_LogFlightControllerState_Freq(flight_mode, armed, altitude, vertical_speed, 1);
    
    // 압력 데이터 처리 및 로깅 (BMP280 초기화 후)
    if (pressure_available) {
        SD_LogInternalPressure(pressure, temperature);
    }
    
    HAL_Delay(10);
}
```

#### **control_loop 함수 내부 로깅 (자동)**
```c
void control_loop(SPI_HandleTypeDef *SPIx, MPU9250_t *pMPU9250, Filtered_t *filtered) 
{
    // IMU 데이터 처리
    MPU_getData(SPIx, pMPU9250);
    KalmanFilter_Update(pMPU9250, filtered);
    
    // PID 제어
    float roll_output = PID_Compute(&pid_roll, target_roll, filtered->attitude.roll);
    float pitch_output = PID_Compute(&pid_pitch, target_pitch, filtered->attitude.pitch);
    float yaw_output = PID_Compute(&pid_yaw, target_yaw, filtered->attitude.yaw);
    
    // 모터 출력 계산
    int motor1 = throttle + roll_output - pitch_output + yaw_output;
    int motor2 = throttle - roll_output - pitch_output - yaw_output;
    int motor3 = throttle - roll_output + pitch_output + yaw_output;
    int motor4 = throttle + roll_output + pitch_output - yaw_output;
    
    // 모터 제어
    motor_SetPWM(1, motor1);
    motor_SetPWM(2, motor2);
    motor_SetPWM(3, motor3);
    motor_SetPWM(4, motor4);
    
    // SD 카드 로깅 (자동)
    SD_LogRawIMU_Freq(pMPU9250, 10);           // Raw IMU (10Hz)
    SD_LogKalmanFiltered_Freq(filtered, 10);    // 필터링된 데이터 (10Hz)
    
    float pid_outputs[4] = {roll_output, pitch_output, yaw_output, (float)throttle};
    SD_LogPIDOutputs_Freq(pid_outputs, 5);     // PID 출력 (5Hz)
    
    uint16_t motor_inputs[4] = {(uint16_t)motor1, (uint16_t)motor2, (uint16_t)motor3, (uint16_t)motor4};
    SD_LogMotorInputs_Freq(motor_inputs, 5);   // 모터 입력값 (5Hz)
}
```

## 📊 로그 파일 형식

### CSV 헤더
```
timestamp,timestamp_ms,sensor_type,data
```

### 로그 예시
```
1704067200,123,IMU,1.234,0.567,45.789,0.123,0.456,0.789,0.012,0.345,0.678
1704067200,124,PRESSURE,1013.25,25.60
1704067200,125,MOTOR,75,1200
1704067200,126,DISTANCE,2.50
1704067200,127,TEMPERATURE,25.30
1704067200,128,BATTERY,12.45
1704067200,129,SYSTEM,0,System running normally
1704067200,130,RAW_IMU,0.123,0.456,0.789,0.012,0.345,0.678,1.234,0.567,45.789
1704067200,131,KALMAN,1.234,0.567,45.789,0.123,0.456,0.789,0.012,0.345,0.678
1704067200,132,MOTOR_INPUT,1500,1500,1500,1500
1704067200,133,PID_OUTPUT,0.000,0.000,0.000,0.500
1704067200,134,FC_STATE,0,1,0.00,0.00
```

## 🎯 장점

1. **중앙 집중식 시간 관리**: 모든 시간 계산이 SD 카드 모듈에서 처리
2. **GPS 시간 동기화**: GPS 시간을 기준으로 모든 센서 데이터 동기화
3. **간단한 사용법**: 각 센서에서는 로깅 함수만 호출
4. **일관된 로그 형식**: 모든 센서 데이터가 동일한 형식으로 로깅
5. **확장성**: 새로운 센서 추가 시 SD 카드 모듈에 로깅 함수만 추가
6. **최적화된 성능**: 불필요한 시간 포맷팅 제거로 오버헤드 최소화
7. **빈도 제어**: 정적변수 카운터로 효율적인 로깅 빈도 관리
8. **리소스 최적화**: 중요도별로 다른 로깅 빈도 설정 가능

## 🔄 데이터 흐름

```
GPS 센서 → Main 보드 → FC 보드 → SD_SetGPSTime() → 모든 센서 로깅
```

1. GPS에서 시간 정보 획득
2. Main 보드에서 시간 패킷 전송
3. FC 보드에서 시간 정보 수신 및 설정
4. 모든 센서 로깅에서 GPS 보정된 시간 사용
5. SD 카드에 시간과 함께 저장

## ⚠️ 주의사항

1. **SD 카드 초기화**: `SD_Init()` 호출 후 로깅 함수 사용
2. **GPS 시간 설정**: GPS 연결 후 자동으로 시간 설정됨
3. **버퍼 관리**: 주기적으로 `SD_FlushBuffer()` 호출 권장
4. **파일 닫기**: 시스템 종료 시 `SD_CloseLogFile()` 호출 