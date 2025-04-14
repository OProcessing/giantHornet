#ifndef INC_HARDWARE_LORA_H_
#define INC_HARDWARE_LORA_H_

#include "define.h"

USER_StatusTypeDef lora_init(void);
USER_StatusTypeDef lora_send(void);
USER_StatusTypeDef lora_recv(void);

#endif /* INC_HARDWARE_LORA_H_ */
