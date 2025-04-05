/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define TRUE  1
#define FALSE 0

#define LOG_TRACE   0
#define LOG_DEBUG   1
#define LOG_INFO    2
#define LOG_WARN    3  
#define LOG_ERROR   4
#define LOG_FATAL   5
#define LOG_LEVEL   LOG_TRACE


#define LOG_LEVEL   LOG_TRACE

#define LOG_LEVEL_STR(level)  \
    ((level) == LOG_TRACE ? "TRACE" : \
     (level) == LOG_DEBUG ? "DEBUG" : \
     (level) == LOG_INFO  ? "INFO"  : \
     (level) == LOG_WARN  ? "WARN"  : \
     (level) == LOG_ERROR ? "ERROR" : \
     (level) == LOG_FATAL ? "FATAL" : "UNKNOWN")
/*
#define log(level, fmt, ...)                                          \
    do {                                                              \
        if (level >= LOG_LEVEL) {                                     \
            printf("[%s] [%s:%d] " fmt " (%s %s)\n",                \
                   LOG_LEVEL_STR(level), __func__, __LINE__,          \
                   ##__VA_ARGS__, __DATE__, __TIME__);                \
        }                                                             \
    } while(0)
*/
/*
int calculate_sum(int *arr, int size) {
    int sum = 0;
    if (arr == NULL || size <= 0) {
        log(LOG_ERROR, "Invalid parameters: arr=%p, size=%d", arr, size);
        return -1;
    }
    log(LOG_DEBUG, "Starting sum calculation");
    for (int i = 0; i < size; i++) {
        sum += arr[i];
        log(LOG_TRACE, "Added arr[%d]=%d, sum=%d", i, arr[i], sum);
    }
    log(LOG_INFO, "Sum calculation completed, sum=%d", sum);
    return sum;
}
*/
#define PI 3.141592653589793



// 정의된 이름은 추후 변경. 현재는 임시로 정의함.
// 사용자 정의 에러 (STM 권장 형식)
typedef enum
{
    USER_RET_OK                		    = 0x00U,
    USER_RET_ERR_INVALID_PARAM       	= 0x01U << 0,
    USER_RET_ERR_NULL_POINTER        	= 0x01U << 1,
    USER_RET_ERR_COMMUNICATION_FAIL  	= 0x01U << 2,
    USER_RET_ERR_TIMEOUT             	= 0x01U << 3,
    USER_RET_ERR_HW_FAILURE          	= 0x01U << 4,
    USER_RET_ERR_MEMORY_FAILURE      	= 0x01U << 5,
    USER_RET_ERR_UNSUPPORTED         	= 0x01U << 6,
    USER_RET_ERR_NO_RESOURCE         	= 0x01U << 7,
    USER_RET_ERR_UNKNOWN             	= 0x01U << 8,
    
    USER_RET_HARDWARE_IMU             = 0x01U << 9,
    USER_RET_HARDWARE_GPS             = 0x02U << 10,
    USER_RET_HARDWARE_TOF             = 0x04U << 11,
} USER_StatusTypeDef;
/* USER CODE END PTD */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */
