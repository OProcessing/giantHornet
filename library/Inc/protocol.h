#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include <stdint.h>

#define MAX_PAYLOAD_LENGTH 50
#define PACKET_HEADER  0xA840  // '¿' '@'
#define PACKET_TAIL    0x3F    // '?'

#pragma pack(push, 1)

uint8_t data_ptr[MAX_PAYLOAD_LENGTH] = {0};
typedef struct {
    uint16_t    header;
    uint32_t    timestamp;
    uint16_t    timestamp_ms;
    uint8_t     type;
    uint8_t     action;
    uint8_t     data_length;
    uint8_t     *data_ptr;
    uint8_t     tail;
} packet_log_t;

typedef struct {
    uint16_t    header;
    uint8_t     type;
    uint8_t     action;
    uint8_t     data_length;
    uint8_t     *data_ptr;
    uint8_t     checksum;
    uint8_t     tail;
} packet_comm_t;
#pragma pack(pop)

// 센서 type, 
enum packet_type {
    TYPE_NONE,
    TYPE_GPS_COORDINATES,
    TYPE_PID_RESULTS,
    TYPE_IMU_ALT_GPS,
    TYPE_PID_CALC_DATA,
    // ...
}; 

// action
enum packet_action {
    ACTION_NONE,
    ACTION_PACKRT,
    ACTION_EVENT,
    ACTION_REQUEST,
    ACTION_DEBUG,
    // ...
};

extern uint8_t calculate_checksum(uint8_t *data, uint8_t length);
extern void create_packet_log(packet_log_t *packet, uint8_t type, uint8_t action, uint8_t *data, uint8_t length);
extern void create_packet_Tx(packet_log_t *packet, uint8_t type, uint8_t action, uint8_t *data, uint8_t length);

#endif /* INC_PROTOCOL_H_ */
