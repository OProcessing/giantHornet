#ifndef INC_FUNCTION_PROTOCOL_H
#define INC_FUNCTION_PROTOCOL_H

#include "usart.h"
#include "protocol.h"
#include "define.h"

#define PACKET_TIMEOUT 1000

void protocol_init(UART_HandleTypeDef *uart);
volatile void protocol_rx(void);
void protocol_tx(uint8_t *data, size_t len);
void protocol_parser(void);
void protocol_handler(packet_comm_t *packet_comm_data);

/** 모터 제어값 4개를 패킷(TYPE_MOTOR_CONTROL)으로 다른 MCU에 전송. payload: M1,M2,M3,M4 각 2바이트 빅엔디안 */
void protocol_send_motor_control(const uint16_t motor_inputs[4]);

#endif /* INC_FUNCTION_PROTOCOL_H */
