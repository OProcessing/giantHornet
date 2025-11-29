#ifndef INC_FUNCTION_CONTROLLER_H_
#define INC_FUNCTION_CONTROLLER_H_

#include "define.h"
#include "protocol.h"

extern remote_control_t remote_data;
extern bool controller_connected;

USER_StatusTypeDef controller_init(void);
void controller_task(void);

#endif /* INC_FUNCTION_CONTROLLER_H_ */
