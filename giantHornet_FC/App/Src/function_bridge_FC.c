#include "function_bridge_FC.h"
#include "function_protocol.h"

static uint32_t protocol_time;

USER_StatusTypeDef bridge_init(void) {
    USER_StatusTypeDef ret;

    protocol_init(&huart3);
    protocol_time = 0;

    return ret;
}

void bridge_task(void) {
	USER_StatusTypeDef ret;

    if((HAL_GetTick() - protocol_time) > BRIDGE_PERIOD) {
        protocol_time = HAL_GetTick();
        // Bridge
        protocol_parser();
    }
}