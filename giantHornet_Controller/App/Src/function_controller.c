#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include "main.h"
#include "function_controller.h"

static uint32_t led_toggle_time = 0;
static uint32_t controller_time = 0;
static struct ControllerBtn_t controller_btn;

static uint8_t lora_data[256];

USER_StatusTypeDef controller_init(void) {
    USER_StatusTypeDef ret;
    
    ret = controller_btn_init(&controller_btn);
    if(ret != USER_RET_OK) {
        printf("controller init error! %d", ret);
        return ret;
    }

    ret = lora_init(1);
    if(ret != USER_RET_OK) {
        printf("lora init error! %d", ret);
    }
    return ret;
}

void controller_task(void) {
	USER_StatusTypeDef ret;

    if ((HAL_GetTick() - led_toggle_time) > 500) {
        led_toggle_time = HAL_GetTick();

        HAL_GPIO_TogglePin(D13_GPIO_Port, D13_Pin);
    }

    if ((HAL_GetTick() - controller_time) > 1000) {
        controller_time = HAL_GetTick();

        controller_btn_read(&controller_btn);

        packet_comm_t *lora_packet = (packet_comm_t*)lora_data;
        lora_packet->header = PACKET_HEADER;
        lora_packet->type = TYPE_NONE;
        lora_packet->action = ACTION_PACKRT;
        lora_packet->data_length = sizeof(struct ControllerBtn_t);
        lora_packet->data_ptr = (uint8_t*)&controller_btn;
        lora_packet->tail = PACKET_TAIL;

        uint8_t len = sizeof(packet_comm_t);
        ret = lora_send(lora_packet, &len); 
        if(ret != USER_RET_OK) {
            printf("lora send error! %d\n", ret);
        }
    }
}
