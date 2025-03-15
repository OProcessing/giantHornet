#ifndef GPS_H
#define GPS_H

#include "stm32f4xx_hal.h"

// 버퍼 크기 정의
#define GPS_BUFFER_SIZE 128

// GPS 데이터 구조체
typedef struct {
    float latitude;      // 위도 (십진수 형식)
    float longitude;     // 경도 (십진수 형식)
    char ns;             // 북/남위 표시
    char ew;             // 동/서경 표시
    float utc_time;      // UTC 시간
    float altitude;      // 고도 (미터)
    int satellites;      // 사용 중인 위성 수
    int fix_status;      // GPS 잠금 상태 (1: 잠금, 0: 잠금 해제)
} GPS_t;

// 함수 선언
void GPS_Init(UART_HandleTypeDef *huart);
void GPS_UART_Callback(void);
int GPS_ValidateChecksum(char *nmea);
void GPS_ParseNMEA(char *nmea);
float GPS_ConvertToDecimal(float nmea_coord, char direction);

#endif