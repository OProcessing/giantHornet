#ifndef INC_HARDWARE_CONTROLLER_H_
#define INC_HARDWARE_CONTROLLER_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "define.h"
#include "main.h"

struct ControllerBtn_t { // 20 bytes
    union {
        struct {
            uint32_t up             : 1;
            uint32_t down           : 1;
            uint32_t cw             : 1;
            uint32_t ccw            : 1;
            uint32_t joy00_btn      : 1;
            uint32_t emergency      : 1;
            uint32_t disconnect     : 1;
            uint32_t reserved_btn   : 25;
        };
        uint32_t buttons;
    };
    float joy00_x;
    float joy00_y;
    uint32_t reserved1;
    uint32_t reserved2;
};

USER_StatusTypeDef controller_btn_init(struct ControllerBtn_t *controller);
USER_StatusTypeDef controller_btn_read(struct ControllerBtn_t *controller);

#endif /* INC_HARDWARE_CONTROLLER_H_ */
