#include "stm32f4xx_hal.h"
#include "main.h"
#include "function_controller.h"

static uint32_t led_toggle_time = 0;
static uint32_t controller_time = 0;
static struct ControllerBtn_t controller_btn;

USER_StatusTypeDef controller_init(void) {
    USER_StatusTypeDef ret;
    
    ret = controller_btn_init(&controller_btn);
    if(ret != USER_RET_OK) {
        return ret;
    }

    ret = lora_init();
    return ret;
}

void controller_task(void) {
    if ((HAL_GetTick() - led_toggle_time) > 500) {
        led_toggle_time = HAL_GetTick();

        HAL_GPIO_TogglePin(D13_GPIO_Port, D13_Pin);
    }

    if ((HAL_GetTick() - controller_time) > 100) {
        controller_time = HAL_GetTick();

        controller_btn_read(&controller_btn);
        lora_send(); // template
    }
}
