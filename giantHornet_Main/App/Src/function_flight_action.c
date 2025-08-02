#if 0

#include "function_flight_action.h"
#include "tim.h"
#include "gpio.h"

extern TIM_HandleTypeDef htim2;

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

static flight_state_t current_state = FLIGHT_STATE_IDLE;
static uint32_t action_start_time = 0;
static uint16_t base_throttle = 11000;  // 기본 호버링 스로틀

int flight_action_ascend(void)
{
    LOG_DEBUG("Ascending action started");
    
    if (current_state != FLIGHT_STATE_HOVER) {
        LOG_WARN("Cannot ascend - not in hover state");
        return -1;
    }
    
    current_state = FLIGHT_STATE_ASCEND;
    action_start_time = HAL_GetTick();
    
    // 모든 모터의 스로틀을 증가시켜 상승
    uint16_t ascend_throttle = base_throttle + 2000;  // 2000 증가
    
    htim2.Instance->CCR1 = Throttle(ascend_throttle);
    htim2.Instance->CCR2 = Throttle(ascend_throttle);
    htim2.Instance->CCR3 = Throttle(ascend_throttle);
    htim2.Instance->CCR4 = Throttle(ascend_throttle);
    
    LOG_DEBUG("Ascend throttle set to: %d", ascend_throttle);
    return 0;
}

int flight_action_descend(void)
{
    LOG_DEBUG("Descending action started");
    
    if (current_state != FLIGHT_STATE_HOVER) {
        LOG_WARN("Cannot descend - not in hover state");
        return -1;
    }
    
    current_state = FLIGHT_STATE_DESCEND;
    action_start_time = HAL_GetTick();
    
    // 모든 모터의 스로틀을 감소시켜 하강
    uint16_t descend_throttle = base_throttle - 1500;  // 1500 감소
    
    // 최소 스로틀 보장
    if (descend_throttle < 9000) {
        descend_throttle = 9000;
    }
    
    htim2.Instance->CCR1 = Throttle(descend_throttle);
    htim2.Instance->CCR2 = Throttle(descend_throttle);
    htim2.Instance->CCR3 = Throttle(descend_throttle);
    htim2.Instance->CCR4 = Throttle(descend_throttle);
    
    LOG_DEBUG("Descend throttle set to: %d", descend_throttle);
    return 0;
}

int flight_action_flip(void)
{
    LOG_DEBUG("Flip action started");
    
    if (current_state != FLIGHT_STATE_HOVER) {
        LOG_WARN("Cannot flip - not in hover state");
        return -1;
    }
    
    current_state = FLIGHT_STATE_FLIP;
    action_start_time = HAL_GetTick();
    
    // 플립 동작: 전진 플립 (앞쪽 모터 증가, 뒤쪽 모터 감소)
    uint16_t front_throttle = base_throttle + 3000;
    uint16_t back_throttle = base_throttle - 2000;
    
    // 최소 스로틀 보장
    if (back_throttle < 8000) {
        back_throttle = 8000;
    }
    
    // 모터 1, 2 (앞쪽) 증가, 모터 3, 4 (뒤쪽) 감소
    htim2.Instance->CCR1 = Throttle(front_throttle);
    htim2.Instance->CCR2 = Throttle(front_throttle);
    htim2.Instance->CCR3 = Throttle(back_throttle);
    htim2.Instance->CCR4 = Throttle(back_throttle);
    
    LOG_DEBUG("Flip throttle - Front: %d, Back: %d", front_throttle, back_throttle);
    return 0;
}

int flight_action_land(void)
{
    LOG_DEBUG("Landing action started");
    
    current_state = FLIGHT_STATE_LAND;
    action_start_time = HAL_GetTick();
    
    // 점진적으로 스로틀을 감소시켜 착륙
    uint16_t land_throttle = base_throttle - 1000;
    
    // 최소 스로틀 보장
    if (land_throttle < 9500) {
        land_throttle = 9500;
    }
    
    htim2.Instance->CCR1 = Throttle(land_throttle);
    htim2.Instance->CCR2 = Throttle(land_throttle);
    htim2.Instance->CCR3 = Throttle(land_throttle);
    htim2.Instance->CCR4 = Throttle(land_throttle);
    
    LOG_DEBUG("Land throttle set to: %d", land_throttle);
    return 0;
}

int flight_action_takeoff(void)
{
    LOG_DEBUG("Takeoff action started");
    
    if (current_state != FLIGHT_STATE_IDLE) {
        LOG_WARN("Cannot takeoff - not in idle state");
        return -1;
    }
    
    current_state = FLIGHT_STATE_TAKEOFF;
    action_start_time = HAL_GetTick();
    
    // 이륙을 위한 스로틀 증가
    uint16_t takeoff_throttle = base_throttle + 1500;
    
    htim2.Instance->CCR1 = Throttle(takeoff_throttle);
    htim2.Instance->CCR2 = Throttle(takeoff_throttle);
    htim2.Instance->CCR3 = Throttle(takeoff_throttle);
    htim2.Instance->CCR4 = Throttle(takeoff_throttle);
    
    LOG_DEBUG("Takeoff throttle set to: %d", takeoff_throttle);
    return 0;
}

int flight_action_hover(void)
{
    LOG_DEBUG("Hover action started");
    
    current_state = FLIGHT_STATE_HOVER;
    action_start_time = HAL_GetTick();
    
    // 호버링을 위한 기본 스로틀 설정
    htim2.Instance->CCR1 = Throttle(base_throttle);
    htim2.Instance->CCR2 = Throttle(base_throttle);
    htim2.Instance->CCR3 = Throttle(base_throttle);
    htim2.Instance->CCR4 = Throttle(base_throttle);
    
    LOG_DEBUG("Hover throttle set to: %d", base_throttle);
    return 0;
}

int flight_action_circle(void)
{
    LOG_DEBUG("Circle action started");
    
    if (current_state != FLIGHT_STATE_HOVER) {
        LOG_WARN("Cannot circle - not in hover state");
        return -1;
    }
    
    // 원형 비행을 위한 모터 믹싱
    // 좌회전 원형 비행 (왼쪽 모터 감소, 오른쪽 모터 증가)
    uint16_t left_throttle = base_throttle - 500;
    uint16_t right_throttle = base_throttle + 500;
    
    // 최소 스로틀 보장
    if (left_throttle < 10000) {
        left_throttle = 10000;
    }
    
    // 모터 1, 3 (왼쪽) 감소, 모터 2, 4 (오른쪽) 증가
    htim2.Instance->CCR1 = Throttle(left_throttle);
    htim2.Instance->CCR2 = Throttle(right_throttle);
    htim2.Instance->CCR3 = Throttle(left_throttle);
    htim2.Instance->CCR4 = Throttle(right_throttle);
    
    LOG_DEBUG("Circle throttle - Left: %d, Right: %d", left_throttle, right_throttle);
    return 0;
}

// 비행 상태 업데이트 함수 (주기적으로 호출)
void flight_action_update(void)
{
    uint32_t current_time = HAL_GetTick();
    uint32_t elapsed_time = current_time - action_start_time;
    
    switch (current_state) {
        case FLIGHT_STATE_TAKEOFF:
            // 이륙 후 2초 후 호버링으로 전환
            if (elapsed_time > 2000) {
                flight_action_hover();
            }
            break;
            
        case FLIGHT_STATE_ASCEND:
            // 상승 후 3초 후 호버링으로 전환
            if (elapsed_time > 3000) {
                flight_action_hover();
            }
            break;
            
        case FLIGHT_STATE_DESCEND:
            // 하강 후 3초 후 호버링으로 전환
            if (elapsed_time > 3000) {
                flight_action_hover();
            }
            break;
            
        case FLIGHT_STATE_FLIP:
            // 플립 후 1초 후 호버링으로 전환
            if (elapsed_time > 1000) {
                flight_action_hover();
            }
            break;
            
        case FLIGHT_STATE_LAND:
            // 착륙 후 2초 후 정지
            if (elapsed_time > 2000) {
                current_state = FLIGHT_STATE_IDLE;
                // 모든 모터 정지
                htim2.Instance->CCR1 = Throttle(0);
                htim2.Instance->CCR2 = Throttle(0);
                htim2.Instance->CCR3 = Throttle(0);
                htim2.Instance->CCR4 = Throttle(0);
                LOG_DEBUG("Landing completed - motors stopped");
            }
            break;
            
        default:
            break;
    }
}

// 현재 비행 상태 반환
flight_state_t get_flight_state(void)
{
    return current_state;
}

#endif