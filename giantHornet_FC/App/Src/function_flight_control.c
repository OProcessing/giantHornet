#include "function_flight_control.h"
#include "define.h"

void FCS_ESC_Starter()
{
	TIM2->ARR = PERIOD;  // period

	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_ALL);

    // ESC Calibration action Set all motors max throttle for 3 seconds
	// FCS_USER_Motor_input(1000,1000,1000,1000);
	// Delay_ms(3000);
	// FCS_USER_Motor_input(0, 0, 0, 0);
	// Delay_ms(3000);
}
