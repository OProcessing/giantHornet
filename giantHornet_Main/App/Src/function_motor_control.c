#include "define.h"
#include "function_motor_control.h"
#include "tim.h"
#include "gpio.h"

extern TIM_HandleTypeDef htim2;
// 500 Hz 
// 20000 ticks per 1 second
// so if you wanna make 1ms pulse than High 10000ticks
uint16_t Throttle(uint16_t value)
{
    value += 10000;
    if(value > 20000) return 20000;
    else return value;
}

void ESC_power_set(uint8_t value)
{
    HAL_GPIO_WritePin(ESC_PORT, ESC_PIN, value);
}

void ESC_calibration(void)
{
    LOG_DEBUG("TIM2 PWM start");
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1); 
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3); 
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_4);
    HAL_Delay(100);

    htim2.Instance->CCR1 = Throttle(THROTTLE_100); 
    htim2.Instance->CCR2 = Throttle(THROTTLE_100);
    htim2.Instance->CCR3 = Throttle(THROTTLE_100); 
    htim2.Instance->CCR4 = Throttle(THROTTLE_100);
    LOG_DEBUG("set throttle 100, wait til cal, CCR1 : %d", htim2.Instance->CCR1);
    HAL_Delay(CALIBRATION_TIME_MS);
    LOG_DEBUG("calibration Done");

    htim2.Instance->CCR1 = Throttle(THROTTLE_0);
    htim2.Instance->CCR2 = Throttle(THROTTLE_0);
    htim2.Instance->CCR3 = Throttle(THROTTLE_0);
    htim2.Instance->CCR4 = Throttle(THROTTLE_0);
    LOG_DEBUG("set throttle 0, wait til cal, CCR1 : %d", htim2.Instance->CCR1);
    HAL_Delay(CALIBRATION_TIME_MS);
    LOG_DEBUG("calibration Done");
}