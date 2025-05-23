#ifndef INC_FUNCTION_CONTROLLER_H_
#define INC_FUNCTION_CONTROLLER_H_

#include "spi.h"
#include "define.h"
#include "hardware_controller.h"
#include "hardware_lora.h"

USER_StatusTypeDef controller_init(SPI_HandleTypeDef* spi);
void controller_task(void);

#endif /* INC_FUNCTION_CONTROLLER_H_ */
