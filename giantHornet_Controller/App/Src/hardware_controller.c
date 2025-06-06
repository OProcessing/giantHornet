#include "hardware_controller.h"
#include "adc.h"

static uint32_t adcVal[8] = {0,};

USER_StatusTypeDef controller_btn_init(struct ControllerBtn_t *controller) {
    if(!controller) {
        return ERR_NULL_POINTER;
    }

    controller->buttons = 0;
    controller->joy00_x = 0.0;
    controller->joy00_y = 0.0;

    return USER_RET_OK;
}

USER_StatusTypeDef controller_btn_read(struct ControllerBtn_t *controller) {
    if(!controller) {
        return ERR_NULL_POINTER;
    }

    // read all buttons
    controller->up          = HAL_GPIO_ReadPin(BTN_UP_GPIO_Port, BTN_UP_Pin);
    controller->down        = HAL_GPIO_ReadPin(BTN_DOWN_GPIO_Port, BTN_DOWN_Pin);
    controller->cw          = HAL_GPIO_ReadPin(BTN_CW_GPIO_Port, BTN_CW_Pin);
    controller->ccw         = HAL_GPIO_ReadPin(BTN_CCW_GPIO_Port, BTN_CCW_Pin);
    controller->joy00_btn   = HAL_GPIO_ReadPin(BTN_JOY00_GPIO_Port, BTN_JOY00_Pin);
    controller->emergency   = HAL_GPIO_ReadPin(BTN_EMERGENCY_GPIO_Port, BTN_EMERGENCY_Pin);
    controller->disconnect  = HAL_GPIO_ReadPin(BTN_DISCONNECT_GPIO_Port, BTN_DISCONNECT_Pin);

    // read joystick
    uint32_t joy_temp;

    joy_temp = adcVal[0];
    joy_temp += adcVal[1];
    joy_temp += adcVal[2];
    joy_temp += adcVal[3];

    controller->joy00_x = ((float)joy_temp / 16380); // 4095*4 = 16380

    joy_temp = adcVal[4];
    joy_temp += adcVal[5];
    joy_temp += adcVal[6];
    joy_temp += adcVal[7];

    controller->joy00_y = ((float)joy_temp / 16380); // 4095*4 = 16380

    // start next conversion
    HAL_ADC_Start_DMA(&hadc1, adcVal, 8);

    return USER_RET_OK;
}
