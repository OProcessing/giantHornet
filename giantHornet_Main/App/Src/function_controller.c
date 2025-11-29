#include "function_controller.h"
#include "hardware_lora.h"

static uint8_t lora_buf[256];
static uint8_t lora_len;
remote_control_t remote_data;

bool controller_connected;
static uint8_t controller_failcount;

USER_StatusTypeDef controller_init(void) {
    memset(&remote_data, 0, sizeof(remote_data));
    controller_connected = false;
    controller_failcount = 0;

    USER_StatusTypeDef ret;
    ret = lora_init(&hspi2, 0); // 0 : slave mode

    return ret;
}

void controller_task(void) {
    USER_StatusTypeDef ret;

    ret = lora_recv(lora_buf, &lora_len);

    if(ret == USER_RET_OK && lora_len > 0) {
        packet_comm_t packet_comm;
        uint8_t packet_data[64];

        ret = parse_packet_comm(lora_buf, lora_len, &packet_comm, packet_data);
        if(ret == USER_RET_OK) {
            controller_connected = true;

            if(packet_comm.type == TYPE_REMOTE && packet_comm.action == ACTION_PACKET) {
                memcpy(&remote_data, packet_data, sizeof(remote_data));
            }
        } else {
            printf("parse error! %d\n", ret);
        }
    } else {
        if(!controller_connected) {
            return;
        }

        controller_failcount++;
        if(controller_failcount > 5) {
            controller_connected = false;
        }
    }
}