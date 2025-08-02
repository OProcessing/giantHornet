#include "function_protocol.h"
#include "protocol.h"
#include "ring_buffer.h"
#include "hardware_sd_card.h"

static UART_HandleTypeDef* huart;
static uint8_t rx_temp;
static uint8_t rx_buf[256];
static RingBuffer rx_rb;

static packet_sequence_t packet_seq;
static packet_comm_t packet_comm_data;
static uint8_t packet_checksum;
static uint8_t packet_data[MAX_PAYLOAD_LENGTH];
static uint8_t packet_data_count;

static uint32_t packet_timer;

void protocol_init(UART_HandleTypeDef *uart) {
	data_ptr = packet_data;
	packet_seq = SEQ_HEADER_1;
	packet_comm_data.data_ptr = packet_data;
	packet_data_count = 0;
	packet_checksum = 0;

	RingBuffer_Init(&rx_rb, rx_buf, 256);

	if(uart) {
		huart = uart;
		HAL_UART_Receive_IT(huart, &rx_temp, 1);
	}
}

volatile void protocol_rx(void) {
	if (HAL_UART_Receive_IT(huart, &rx_temp, 1) == HAL_OK)
	{
		RingBuffer_Put(&rx_rb, rx_temp);
	}
}

void protocol_tx(uint8_t *data, size_t len) {
	HAL_UART_Transmit(huart, (const uint8_t *)data, len, 100);
}

void protocol_parser(void) {
	uint8_t buf_temp;

	if((HAL_GetTick() - packet_timer) > PACKET_TIMEOUT) { // timeout check
		packet_seq = SEQ_HEADER_1;
	}

	// until read all data
	while(RingBuffer_Get(&rx_rb, &buf_temp)) {
		// protocol_tx(&buf_temp, 1); // loopback test
		switch (packet_seq)
		{
		case SEQ_HEADER_1:
			if(buf_temp == ((PACKET_HEADER >> 8) & 0xFF)) {
				packet_comm_data.header = (uint16_t)buf_temp << 8;
				packet_timer = HAL_GetTick();
				packet_seq = SEQ_HEADER_2;
			}
			break;
		case SEQ_HEADER_2:
			if(buf_temp == (PACKET_HEADER & 0xFF)) {
				packet_comm_data.header |= (uint16_t)buf_temp;
				packet_checksum = 0;
				packet_seq = SEQ_TYPE;
			} else {
				packet_seq = SEQ_HEADER_1;
			}
			break;
		case SEQ_TYPE:
			packet_comm_data.type = buf_temp;
			calculate_checksum(&buf_temp, &packet_checksum);
			packet_seq = SEQ_ACTION;
			break;
		case SEQ_ACTION:
			packet_comm_data.action = buf_temp;
			calculate_checksum(&buf_temp, &packet_checksum);
			packet_seq = SEQ_LEN;
			break;
		case SEQ_LEN:
			packet_comm_data.data_length = buf_temp;
			packet_data_count = 0;
			calculate_checksum(&buf_temp, &packet_checksum);
			if(packet_comm_data.data_length == 0) {
				packet_seq = SEQ_CHECKSUM;
			} else {
				packet_seq = SEQ_DATA;
			}

			break;
		case SEQ_DATA:
			packet_data[packet_data_count] = buf_temp;
			packet_data_count++;
			calculate_checksum(&buf_temp, &packet_checksum);
			if(packet_data_count >= packet_comm_data.data_length) {
				packet_seq = SEQ_CHECKSUM;
			}
			break;
		case SEQ_CHECKSUM:
			packet_comm_data.checksum = buf_temp;
			if(buf_temp == packet_checksum) {
				packet_seq = SEQ_TAIL;
			} else {
				packet_seq = SEQ_HEADER_1;
			}
			break;
		case SEQ_TAIL:
			if(buf_temp == PACKET_TAIL) {
				packet_comm_data.tail = buf_temp;
				protocol_handler(&packet_comm_data);
			}
			packet_seq = SEQ_HEADER_1;
			break;
		default:
			packet_seq = SEQ_HEADER_1;
			break;
		}
	}
}

void protocol_handler(packet_comm_t *packet_comm_data) {
	if((packet_comm_data->header != PACKET_HEADER) ||
			(packet_comm_data->tail != PACKET_TAIL)) {
		return;
	}

	// Log packet to SD card
	uint8_t packet_buffer[64]; // Maximum packet size
	uint16_t packet_length = 0;
	
	// Reconstruct packet for SD logging
	packet_buffer[packet_length++] = (PACKET_HEADER >> 8) & 0xFF; // Header 1
	packet_buffer[packet_length++] = PACKET_HEADER & 0xFF;         // Header 2
	
	// Add timestamp (simplified - you might want to get actual timestamp)
	uint32_t timestamp = HAL_GetTick() / 1000;
	uint16_t timestamp_ms = HAL_GetTick() % 1000;
	packet_buffer[packet_length++] = (timestamp >> 24) & 0xFF;
	packet_buffer[packet_length++] = (timestamp >> 16) & 0xFF;
	packet_buffer[packet_length++] = (timestamp >> 8) & 0xFF;
	packet_buffer[packet_length++] = timestamp & 0xFF;
	packet_buffer[packet_length++] = (timestamp_ms >> 8) & 0xFF;
	packet_buffer[packet_length++] = timestamp_ms & 0xFF;
	
	// Add packet data
	packet_buffer[packet_length++] = packet_comm_data->type;
	packet_buffer[packet_length++] = packet_comm_data->action;
	packet_buffer[packet_length++] = packet_comm_data->data_length;
	
	// Add data
	for(int i = 0; i < packet_comm_data->data_length; i++) {
		packet_buffer[packet_length++] = packet_comm_data->data_ptr[i];
	}
	
	// Log to SD card
	SD_LogExternalPacket(packet_buffer, packet_length);

	switch (packet_comm_data->type) {
	case TYPE_NONE:
			printf("got packet!, %02X\n", packet_comm_data->data_ptr[0]);
		break;
	case TYPE_GPS_COORDINATES:
		// GPS 좌표 데이터 처리
		break;
	case TYPE_GPS_INIT_TIME:
		// GPS 시간 초기화 데이터 처리
		if (packet_comm_data->data_length >= 4) {
			uint32_t gps_time = (packet_comm_data->data_ptr[0] << 24) |
							   (packet_comm_data->data_ptr[1] << 16) |
							   (packet_comm_data->data_ptr[2] << 8) |
							   packet_comm_data->data_ptr[3];
			SD_SetGPSTime(gps_time);
			printf("GPS INIT TIME set: %lu\n", gps_time);
		}
		break;
	case TYPE_PID_RESULTS:
		break;
	case TYPE_IMU_ALT_GPS:
		break;
	case TYPE_PID_CALC_DATA:
		break;
	default:
		break;
	}
}
