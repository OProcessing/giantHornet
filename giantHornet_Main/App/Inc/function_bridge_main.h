#ifndef INC_FUNCTION_BRIDGE_MAIN_H
#define INC_FUNCTION_BRIDGE_MAIN_H

#include "define.h"

#define BRIDGE_PERIOD 50

USER_StatusTypeDef bridge_init(void);
void bridge_task(void);

#endif /* INC_FUNCTION_BRIDGE_MAIN_H */
