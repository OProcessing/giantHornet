#include <stdio.h>
#include <stdlib.h>
#include "stm32f4xx_hal.h"
#include "main.h"
#include "function_controller.h"
#include "protocol.h"

static uint32_t led_toggle_time = 0;
static uint32_t controller_time = 0;
static struct ControllerBtn_t controller_btn;

static uint8_t lora_data[256];

USER_StatusTypeDef controller_init(SPI_HandleTypeDef* spi) {
    USER_StatusTypeDef ret;
    
    ret = controller_btn_init(&controller_btn);
    if(ret != USER_RET_OK) {
        printf("controller init error! %d", ret);
        return ret;
    }

    ret = lora_init(spi, 1);
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

    if ((HAL_GetTick() - controller_time) > 50) {
        controller_time = HAL_GetTick();

        controller_btn_read(&controller_btn);

        uint8_t remote_data[6] = {
            (~controller_btn.buttons & 0xFF),
            0x00,
            (int8_t)((controller_btn.joy00_x - 0.5) * 127),
            (int8_t)((controller_btn.joy00_y - 0.5) * 127),
            0x00,
            0x00
        };

        packet_comm_t packet_comm;
        create_packet_comm(&packet_comm, TYPE_REMOTE, ACTION_PACKET, remote_data, 6);
        uint8_t len = make_packet_comm(lora_data, &packet_comm);

        /*
        printf("data[%d] : {", len);
        for(int i=0; i<len; i++) {
            printf("%02X, ", ((uint8_t *)lora_data)[i]);
        }
        printf("}\n");
        */

        ret = lora_send(lora_data, &len);
        if(ret != USER_RET_OK) {
            printf("lora send error! %d\n", ret);
        }
        //HAL_GPIO_TogglePin(D13_GPIO_Port, D13_Pin);
    }
}
