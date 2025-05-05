#ifndef INC_HARDWARE_LORA_H_
#define INC_HARDWARE_LORA_H_

#include "stdlib.h"
#include "../../Drivers/SX1278/SX1278.h"
#include "define.h"

#define LORA_FREQ 434000000
#define LORA_PACKET_LEN 16
#define LORA_TIMEOUT 2000

USER_StatusTypeDef lora_init(uint8_t master_enable);
USER_StatusTypeDef lora_send(void* data, uint8_t* len);
USER_StatusTypeDef lora_recv(void* data, uint8_t* len);

#endif /* INC_HARDWARE_LORA_H_ */
