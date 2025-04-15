/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "define.h"
#include "protocol.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define JOY00_Y_Pin GPIO_PIN_0
#define JOY00_Y_GPIO_Port GPIOC
#define JOY00_X_Pin GPIO_PIN_1
#define JOY00_X_GPIO_Port GPIOC
#define SW_RX_Pin GPIO_PIN_0
#define SW_RX_GPIO_Port GPIOA
#define SW_TX_Pin GPIO_PIN_1
#define SW_TX_GPIO_Port GPIOA
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define A2_Pin GPIO_PIN_4
#define A2_GPIO_Port GPIOA
#define D13_Pin GPIO_PIN_5
#define D13_GPIO_Port GPIOA
#define D12_Pin GPIO_PIN_6
#define D12_GPIO_Port GPIOA
#define BTN_JOY00_Pin GPIO_PIN_7
#define BTN_JOY00_GPIO_Port GPIOA
#define A3_Pin GPIO_PIN_0
#define A3_GPIO_Port GPIOB
#define BTN_DOWN_Pin GPIO_PIN_10
#define BTN_DOWN_GPIO_Port GPIOB
#define BTN_EMERGENCY_Pin GPIO_PIN_7
#define BTN_EMERGENCY_GPIO_Port GPIOC
#define BTN_CCW_Pin GPIO_PIN_8
#define BTN_CCW_GPIO_Port GPIOA
#define BTN_CW_Pin GPIO_PIN_9
#define BTN_CW_GPIO_Port GPIOA
#define D2_Pin GPIO_PIN_10
#define D2_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define D3_Pin GPIO_PIN_3
#define D3_GPIO_Port GPIOB
#define BTN_UP_Pin GPIO_PIN_4
#define BTN_UP_GPIO_Port GPIOB
#define D4_Pin GPIO_PIN_5
#define D4_GPIO_Port GPIOB
#define BTN_DISCONNECT_Pin GPIO_PIN_6
#define BTN_DISCONNECT_GPIO_Port GPIOB
#define D15_Pin GPIO_PIN_8
#define D15_GPIO_Port GPIOB
#define D14_Pin GPIO_PIN_9
#define D14_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
