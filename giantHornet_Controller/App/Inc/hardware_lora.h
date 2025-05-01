#ifndef INC_HARDWARE_LORA_H_
#define INC_HARDWARE_LORA_H_

#include "stdlib.h"
#include "../../Drivers/SX1278/SX1278.h"
#include "define.h"

USER_StatusTypeDef lora_init(uint8_t master_enable);
USER_StatusTypeDef lora_send(void* data, size_t* len);
USER_StatusTypeDef lora_recv(void* data, size_t* len);

#endif /* INC_HARDWARE_LORA_H_ */
