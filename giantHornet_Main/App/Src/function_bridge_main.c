#include "function_bridge_main.h"
#include "function_protocol.h"
#include "hardware_lora.h"

static uint8_t lora_buf[256];
static uint8_t lora_len;

static uint32_t protocol_time;

USER_StatusTypeDef bridge_init(void) {
    USER_StatusTypeDef ret;

    protocol_init(&huart3);
    protocol_time = 0;

    ret = lora_init(&hspi2, 0); // 0 : slave mode

    return ret;
}

void bridge_task(void) {
	USER_StatusTypeDef ret;

    if((HAL_GetTick() - protocol_time) > BRIDGE_PERIOD) {
        protocol_time = HAL_GetTick();
        protocol_parser();

        ret = lora_recv(lora_buf, &lora_len);
        if(ret == USER_RET_OK && lora_len > 0) {
            /*
            printf("data[%d] : {", lora_test_len);
            for(int i=0; i<lora_test_len; i++) {
                printf("%02X, ", lora_test_buf[i]);
            }
            printf("}\n");
            */

            packet_comm_t packet_comm;
            uint8_t packet_data[64];
            ret = parse_packet_comm(lora_buf, lora_len, &packet_comm, packet_data);
            if(ret == USER_RET_OK) {
                printf("packet_data[%d] : {", packet_comm.data_length);
                for(int i=0; i<packet_comm.data_length; i++) {
                printf("%02X, ", packet_data[i]);
                }
                printf("}\n");
            } else {
                printf("parse error! %d\n", ret);
            }
        }
    }
}
