#include "define.h"
#include "function_motor_control.h"
#include "stm32f4xx_hal_tim.h"

extern TIM_HandleTypeDef htim3;

#define UINT32_MAX          4294967295

#define TIM_CLOCK_FREQ      90000000
#define TIM_PRESCALER       1000

#define TIMER_TICKS         (TIM_CLOCK_FREQ / TIM_PRESCALER)
#define PWM_DUTY_PERCENT    50
#define PWM_PERIOD_TICKS    (TIMER_TICKS / PWM_DUTY_PERCENT)

#define CALIBRATION_TIME_MS 1000
#define CALIBRATION_TICKS   (TIMER_TICKS * CALIBRATION_TIME_MS / 1000)



uint16_t Throttle(uint16_t value)
{
    return (value * 1000) / 2000;
}


void ESC_calibration(void)
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); 
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3); 
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
    HAL_Delay(100);

    htim3.Instance->CCR1 = Throttle(1000); 
    htim3.Instance->CCR2 = Throttle(1000);
    htim3.Instance->CCR3 = Throttle(1000); 
    htim3.Instance->CCR4 = Throttle(1000);
    HAL_Delay(CALIBRATION_TIME);

    htim3.Instance->CCR1 = Throttle(0);
    htim3.Instance->CCR2 = Throttle(0);
    htim3.Instance->CCR3 = Throttle(0);
    htim3.Instance->CCR4 = Throttle(0);
    HAL_Delay(CALIBRATION_TIME);
}