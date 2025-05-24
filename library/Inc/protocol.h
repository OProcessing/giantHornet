#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include <stdint.h>

#define MAX_PAYLOAD_LENGTH 50
#define PACKET_HEADER  0xA840  // '¿' '@'
#define PACKET_TAIL    0x3F    // '?'

extern uint8_t* data_ptr;

// packet sequence
typedef enum {
    SEQ_HEADER_1,
    SEQ_HEADER_2,
    SEQ_TYPE,
    SEQ_ACTION,
    SEQ_LEN,
    SEQ_DATA,
    SEQ_CHECKSUM,
    SEQ_TAIL,
} packet_sequence_t; 

// packet data type
typedef enum {
    TYPE_NONE,
    TYPE_GPS_COORDINATES,
    TYPE_PID_RESULTS,
    TYPE_IMU_ALT_GPS,
    TYPE_PID_CALC_DATA,
    TYPE_MOTOR_CONTROL,
    TYPE_REMOTE,
    // ...
} packet_type_t;

// packet action type
typedef enum {
    ACTION_NONE,
    ACTION_PACKET,
    ACTION_EVENT,
    ACTION_REQUEST,
    ACTION_DEBUG,
    // ...
} packet_action_t;

// packet
#pragma pack(push, 1)
typedef struct {
    uint16_t            header;
    uint32_t            timestamp;
    uint16_t            timestamp_ms;
    packet_type_t       type : 8;
    packet_action_t     action : 8;
    uint8_t             data_length;
    uint8_t             *data_ptr;
    uint8_t             tail;
} packet_log_t;

typedef struct {
    uint16_t            header;
    packet_type_t       type : 8;
    packet_action_t     action : 8;
    uint8_t             data_length;
    uint8_t             *data_ptr;
    uint8_t             checksum;
    uint8_t             tail;
} packet_comm_t;
#pragma pack(pop)

extern uint8_t calculate_checksum(uint8_t *data, uint8_t *checksum);
extern void create_packet_log(packet_log_t *packet, uint32_t timestamp, uint16_t timestamp_ms, uint8_t type, uint8_t action, uint8_t *data, uint8_t length);
extern uint8_t make_packet_log(uint8_t *data, packet_log_t *packet);
extern void create_packet_comm(packet_comm_t *packet, uint8_t type, uint8_t action, uint8_t *data, uint8_t length);
extern uint8_t make_packet_comm(uint8_t *data, packet_comm_t *packet);

#endif /* INC_PROTOCOL_H_ */
