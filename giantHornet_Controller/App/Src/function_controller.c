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

        // calibration
        controller_btn.joy00_x += 1;
        controller_btn.joy00_y += 3;

        int16_t joy_x = ((int16_t)(controller_btn.joy00_x >> 2) - 127);
        joy_x = (joy_x > 127) ? 127 : joy_x;
        joy_x = (joy_x < -128) ? -128 : joy_x;
        int16_t joy_y = ((int16_t)(controller_btn.joy00_y >> 2) - 127);
        joy_y = (joy_y > 127) ? 127 : joy_y;
        joy_y = (joy_y < -128) ? -128 : joy_y;

        uint8_t remote_data[6] = {
            (~controller_btn.buttons & 0xFF),
            0x00,
            (int8_t)joy_x,
            (int8_t)joy_y,
            0x00,
            0x00
        };

        printf("x,y = %d, %d\n", controller_btn.joy00_x, controller_btn.joy00_y);
        printf("remote_data = %d, %d\n", (int8_t)remote_data[2], (int8_t)remote_data[3]);

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
