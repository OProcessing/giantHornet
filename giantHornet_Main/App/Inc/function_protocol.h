#ifndef INC_FUNCTION_PROTOCOL_H
#define INC_FUNCTION_PROTOCOL_H

#include "usart.h"
#include "protocol.h"
#include "define.h"

void protocol_init(UART_HandleTypeDef* uart);
volatile void protocol_rx(void);
void protocol_tx(uint8_t* data, size_t len);
void protocol_parser(void);
void protocol_handler(void);

#endif /* INC_FUNCTION_PROTOCOL_H */
