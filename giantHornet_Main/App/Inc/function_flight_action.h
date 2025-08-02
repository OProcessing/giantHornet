#if 0

#ifndef INC_FUNCTION_FLIGHT_ACTION_H
#define INC_FUNCTION_FLIGHT_ACTION_H

#include "define.h"
#include "function_motor_control.h"

// 비행 상태 정의
typedef enum {
    FLIGHT_STATE_IDLE,
    FLIGHT_STATE_TAKEOFF,
    FLIGHT_STATE_HOVER,
    FLIGHT_STATE_ASCEND,
    FLIGHT_STATE_DESCEND,
    FLIGHT_STATE_LAND,
    FLIGHT_STATE_FLIP
} flight_state_t;

// 기본 비행 액션 함수들
int flight_action_ascend(void);
int flight_action_descend(void);
int flight_action_flip(void);
int flight_action_land(void);
int flight_action_takeoff(void);
int flight_action_hover(void);
int flight_action_circle(void);

// 비행 상태 관리 함수들
void flight_action_update(void);
flight_state_t get_flight_state(void);

#endif /* INC_FUNCTION_FLIGHT_ACTION_H */

#endif