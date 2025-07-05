#ifndef INC_FUNCTION_MOTOR_CONTROL_H
#define INC_FUNCTION_MOTOR_CONTROL_H


#endif /* INC_FUNCTION_MOTOR_CONTROL_H */

// TIM2 maximum output 90MHz
// TIM_PRESCALER 9, TIM2 output 10MHz 
// -> 0.1us period, 10000 ticks per ms
// TIM_PERIOD 20000, 2ms period, 500Hz
#define TIM_PRESCALER       (9 - 1)
#define TIM_PERIOD          (20000 - 1)
#define CALIBRATION_TIME_MS (2000)

#define ESC_PORT    GPIOA
#define ESC_PIN     0

/**
 *      return 0 ~ 20000
 *      100% -> 20000
 *       80% -> 16000
 *       50% -> 10000
 *       20% ->  4000
 *        0% ->     0
 */
#define THROTTLE_100 10000
#define THROTTLE_50   5000
#define THROTTLE_0       0

uint16_t Throttle(uint16_t value);
void ESC_power_set(uint8_t value);
void ESC_calibration(void);
