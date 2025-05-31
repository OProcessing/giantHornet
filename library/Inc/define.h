#ifndef INC_DEFINE_H_
#define INC_DEFINE_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TRUE  1
#define FALSE 0

/**
 * 로그 레벨
 * LOG DEBUG    : 디버그 정보 
 * LOG INFO     : 정보 메시지
 * LOG TARCE    : 실행 이력
 * LOG WARN     : 경고 메시지 
 * LOG ERROR    : 오류 메시지
 * LOG FATAL    : 심각한 오류 메시지
 */
typedef enum
{
    LOG_DEBUG,
    LOG_INFO,
    LOG_TRACE,
    LOG_WARN,   //default
    LOG_ERROR,
    LOG_FATAL,
}LOG_LEVEL_e;
#define LOG_LEVEL   LOG_TRACE

void Log_message(LOG_LEVEL_e level, const char *file, const char *func, int line, const char *fmt, ...);
/**
 * DEBUG    : [level] [file:line->func] [debug message]   [fmt] [time]
 * INFO     : [level]            [func] [info  message]   [fmt]
 * TRACE    : [level]            [func]
 * WARN     : [level]            [func] [warn  message]   [fmt]
 * ERROR    : [level] [file:line->func] [error message]   [fmt] [time]
 * FATAL    : [level] [file:line->func] [fatal message]   [fmt] [time]
 */
#define LOG_DEBUG(fmt, ...) Log_message(LOG_DEBUG, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...)  Log_message(LOG_INFO,      NULL, __func__,       -1, fmt, ##__VA_ARGS__)
#define LOG_TRACE(fmt, ...) Log_message(LOG_TRACE,     NULL, __func__,       -1, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  Log_message(LOG_WARN,      NULL, __func__,       -1, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) Log_message(LOG_ERROR,     NULL, __func__,       -1, fmt, ##__VA_ARGS__)
#define LOG_FATAL(fmt, ...) Log_message(LOG_FATAL, __FILE__, __func__, __LINE__, fmt, ##__VA_ARGS__)

#define PI 3.141592653589793

// 정의된 이름은 추후 변경. 현재는 임시로 정의함.
// 사용자 정의 에러 (STM 권장 형식)
typedef enum
{
    // General 0x00~0x0F
    USER_RET_OK                      = 0x00U,
    USER_RET_ERR_FAILED              = 0x01U,
    USER_RET_ERR_UNKNOWN             = 0x0FU,

    // Parameter 0x10~0x1F
    USER_RET_ERR_INVALID_PARAM       = 0x10U,
    USER_RET_ERR_NULL_POINTER        = 0x11U,
    USER_RET_ERR_INVALID_STATE       = 0x12U,
    USER_RET_ERR_OUT_OF_RANGE        = 0x13U,

    // Memory 0x20~0x2F
    USER_RET_ERR_NO_RESOURCE         = 0x20U,
    USER_RET_ERR_MEMORY_ALLOC        = 0x21U,
    USER_RET_ERR_MEMORY_FREE         = 0x22U,
    USER_RET_ERR_RESOURCE_BUSY       = 0x23U,

    // Communication 0x30~0x3F
    USER_RET_ERR_DISCONNECTED        = 0x30U,
    USER_RET_ERR_INVALID             = 0x31U,
    USER_RET_ERR_CHECKSUM            = 0x32U,
    USER_RET_ERR_COMM_FAILED         = 0x33U,

    // Hardware 0x40~0x4F
    USER_RET_ERR_HW_FAILURE          = 0x40U,
    USER_RET_ERR_UNSUPPORTED_HW      = 0x41U,
    USER_RET_ERR_HW_NOT_FOUND        = 0x42U,
    USER_RET_ERR_HW_BUSY             = 0x43U,

    // Sensor 0x50~0x5F
    USER_RET_ERR_IMU_FAILURE         = 0x50U,
    USER_RET_ERR_GPS_FAILURE         = 0x51U,
    USER_RET_ERR_TOF_FAILURE         = 0x52U,
    USER_RET_ERR_SD_FAILURE          = 0x53U,
    USER_RET_ERR_MOTOR_FAILURE       = 0x54U,

    // Storage 0x60~0x6F
    USER_RET_ERR_FILE_NOT_FOUND      = 0x60U,
    USER_RET_ERR_FILE_READ           = 0x61U,
    USER_RET_ERR_FILE_WRITE          = 0x62U,
    USER_RET_ERR_STORAGE_FULL        = 0x63U,

    // 0xF0~0xFF
    USER_RET_ERR_UNSUPPORTED         = 0xF0U,
    USER_RET_ERR_USER_DEFINED        = 0xF1U,
} USER_StatusTypeDef;
/* USER CODE END PTD */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

#endif /* INC_DEFINE_H_ */
