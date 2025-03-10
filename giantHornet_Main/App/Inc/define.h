/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define log(fmt, ...) \
    printf("[%s: %d]" fmt "\t\t\t (%s, %s)\n", \
    __func__, __LINE__, __DATE__, __TIME__);

#define PI 3.141592653589793

// 정의된 이름은 추후 변경. 현재는 임시로 정의함.
// 사용자 정의 에러 (STM 권장 형식)
typedef enum
{
  USER_RET_OK                		= 0x00U,
  USER_RET_ERR_INVALID_PARAM       	= 0x01U << 0,
  USER_RET_ERR_NULL_POINTER        	= 0x01U << 1,
  USER_RET_ERR_COMMUNICATION_FAIL  	= 0x01U << 2,
  USER_RET_ERR_TIMEOUT             	= 0x01U << 3,
  USER_RET_ERR_HW_FAILURE          	= 0x01U << 4,
  USER_RET_ERR_MEMORY_FAILURE      	= 0x01U << 5
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
