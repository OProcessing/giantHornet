#ifndef INC_FUNCTION_FLIGHT_CONTROL_H_
#define INC_FUNCTION_FLIGHT_CONTROL_H_

#include "tim.h"

#define Motor_1     TIM2->CCR1
#define Motor_2     TIM2->CCR2
#define Motor_3     TIM2->CCR3
#define Motor_4     TIM2->CCR4

#define PWM_MAX 1
#define PWM_MIN 0
#define PWM_OFFSET 0 

#define PERIOD 20000 // 20ms period for 50Hz frequency

extern TIM_HandleTypeDef htim2;

#endif /* INC_FUNCTION_FLIGHT_CONTROL_H_ */
