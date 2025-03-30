#include "stm32f4xx_hal.h"
#include "main.h"
#include "function_controller.h"

static uint32_t led_toggle_time = 0;

void controller_task(void) {
  if ((HAL_GetTick() - led_toggle_time) > 500) {
    led_toggle_time = HAL_GetTick();
    HAL_GPIO_TogglePin(D13_GPIO_Port, D13_Pin);
  }
}
