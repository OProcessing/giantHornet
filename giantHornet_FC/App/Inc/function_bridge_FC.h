#ifndef INC_FUNCTION_BRIDGE_FC_H_
#define INC_FUNCTION_BRIDGE_FC_H_

#include "define.h"

#define BRIDGE_PERIOD 50

USER_StatusTypeDef bridge_init(void);
void bridge_task(void);

#endif /* INC_FUNCTION_BRIDGE_FC_H_ */
