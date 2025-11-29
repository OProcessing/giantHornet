#include "function_bridge_main.h"
#include "function_protocol.h"
#include "function_controller.h"

static uint32_t protocol_time;

USER_StatusTypeDef bridge_init(void) {
    USER_StatusTypeDef ret;

    protocol_init(&huart3);
    protocol_time = 0;

    ret = controller_init();

    return ret;
}

void bridge_task(void) {
    if((HAL_GetTick() - protocol_time) > BRIDGE_PERIOD) {
        protocol_time = HAL_GetTick();
        // Bridge (receiver)
        protocol_parser();

        // controller
        controller_task();
    }
}