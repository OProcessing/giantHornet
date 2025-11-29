/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usart.h"
#include "hardware_imu.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef StaticTask_t osStaticThreadDef_t;
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern uint8_t serialBuf[100];
extern MPU9250_t MPU9250;
/* USER CODE END Variables */
/* Definitions for FakeCremmy */
osThreadId_t FakeCremmyHandle;
const osThreadAttr_t FakeCremmy_attributes = {
  .name = "FakeCremmy",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for log_system_ */
osThreadId_t log_system_Handle;
uint32_t log_system_Buffer[ 256 ];
osStaticThreadDef_t log_system_ControlBlock;
const osThreadAttr_t log_system__attributes = {
  .name = "log_system_",
  .cb_mem = &log_system_ControlBlock,
  .cb_size = sizeof(log_system_ControlBlock),
  .stack_mem = &log_system_Buffer[0],
  .stack_size = sizeof(log_system_Buffer),
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for count_task_ */
osThreadId_t count_task_Handle;
uint32_t count_task_Buffer[ 128 ];
osStaticThreadDef_t count_task_ControlBlock;
const osThreadAttr_t count_task__attributes = {
  .name = "count_task_",
  .cb_mem = &count_task_ControlBlock,
  .cb_size = sizeof(count_task_ControlBlock),
  .stack_mem = &count_task_Buffer[0],
  .stack_size = sizeof(count_task_Buffer),
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);
void log_system_task(void *argument);
void count_task_entry(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of FakeCremmy */
  FakeCremmyHandle = osThreadNew(StartDefaultTask, NULL, &FakeCremmy_attributes);

  /* creation of log_system_ */
  log_system_Handle = osThreadNew(log_system_task, NULL, &log_system__attributes);

  /* creation of count_task_ */
  count_task_Handle = osThreadNew(count_task_entry, NULL, &count_task__attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
	// Calibrate the IMU
	sprintf((char *)serialBuf, "CALIBRATING...\r\n");
	HAL_UART_Transmit(&huart2, serialBuf, strlen((char *)serialBuf), HAL_MAX_DELAY);
	MPU_calibrateGyro(&hspi2, &MPU9250, 1500);
  /* Infinite loop */
  for(;;)
  {
	  printf("default\n");
	  osDelay(1000);
  }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_log_system_task */
/**
* @brief Function implementing the log_system_ thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_log_system_task */
void log_system_task(void *argument)
{
  /* USER CODE BEGIN log_system_task */
  /* Infinite loop */
  for(;;)
  {
	  printf("log\n");
	  osDelay(1000);
  }
  /* USER CODE END log_system_task */
}

/* USER CODE BEGIN Header_count_task_entry */
/**
* @brief Function implementing the count_task_ thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_count_task_entry */
void count_task_entry(void *argument)
{
  /* USER CODE BEGIN count_task_entry */
  /* Infinite loop */
  for(;;)
  {
	printf("count\n");
    osDelay(1000);
  }
  /* USER CODE END count_task_entry */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

