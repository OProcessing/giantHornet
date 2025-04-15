#ifndef INC_HARDWARE_LORA_H_
#define INC_HARDWARE_LORA_H_

#include <cstddef>
#include "define.h"

USER_StatusTypeDef lora_init(void);
USER_StatusTypeDef lora_send(void* data, size_t* len);
USER_StatusTypeDef lora_recv(void* data, size_t* len);

#endif /* INC_HARDWARE_LORA_H_ */
