#ifndef INC_FUNCTION_CONTROLLER_H_
#define INC_FUNCTION_CONTROLLER_H_

#include "define.h"
#include "hardware_controller.h"
#include "hardware_lora.h"

USER_StatusTypeDef controller_init(void);
void controller_task(void);

#endif /* INC_FUNCTION_CONTROLLER_H_ */
