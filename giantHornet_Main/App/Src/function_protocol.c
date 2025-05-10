#include "function_protocol.h"
#include "ring_buffer.h"

static UART_HandleTypeDef* huart;
static uint8_t rx_temp;
static uint8_t rx_buf[256];
static RingBuffer rx_rb;

void protocol_init(UART_HandleTypeDef* uart) {
  huart = uart;
  RingBuffer_Init(&rx_rb, rx_buf, 256);
  HAL_UART_Receive_IT(huart, &rx_temp, 1);
}

volatile void protocol_rx(void) {
  if (HAL_UART_Receive_IT(huart, &rx_temp, 1) == HAL_OK)
  {
    RingBuffer_Put(&rx_rb, rx_temp);
    
  }
}

void protocol_tx(uint8_t* data, size_t len) {
  HAL_UART_Transmit(huart, (const uint8_t *)data, len, 100);
}

void protocol_parser(void) {
  // until read all data
  uint8_t buf_temp;
  while(RingBuffer_Get(&rx_rb, &buf_temp)) {
    protocol_tx(&buf_temp, 1);
  }
}

void protocol_handler(void) {

}
