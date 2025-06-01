#ifndef INC_PROTOCOL_H_
#define INC_PROTOCOL_H_

#include <stdint.h>
#include "define.h"

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
    const uint8_t       *data_ptr;
    uint8_t             tail;
} packet_log_t;

typedef struct {
    uint16_t            header;
    packet_type_t       type : 8;
    packet_action_t     action : 8;
    uint8_t             data_length;
    const uint8_t       *data_ptr;
    uint8_t             checksum;
    uint8_t             tail;
} packet_comm_t;
#pragma pack(pop)

/**
 * @brief calculate checksum
 * 
 * @param data new data
 * @param checksum old checksum & calculated checksum
 * @return uint8_t calculated checksum
 */
extern uint8_t calculate_checksum(const uint8_t *data, uint8_t *checksum);

/**
 * @brief Create a packet_log_t object
 * 
 * @param packet packet_comm_t
 * @param timestamp timestamp (unix)
 * @param timestamp_ms timestamp_ms
 * @param type packet_type_t
 * @param action packet_action_t
 * @param data data buffer
 * @param length length of data
 */
extern void create_packet_log(
    packet_log_t *packet,
    const uint32_t timestamp,
    const uint16_t timestamp_ms,
    const packet_type_t type,
    const packet_action_t action,
    const uint8_t *data,
    const uint8_t length
);

/**
 * @brief make log packet buffer
 * 
 * @param buffer data buffer (must be 64+ bytes)
 * @param packet packet_comm_t
 * @return uint8_t length of packet buffer
 */
extern uint8_t make_packet_log(uint8_t *buffer, const packet_log_t *packet);

/**
 * @brief Create a packet_comm_t object
 * 
 * @param packet packet_comm_t
 * @param type packet_type_t
 * @param action packet_action_t
 * @param data data buffer
 * @param length length of data
 */
extern void create_packet_comm(
    packet_comm_t *packet,
    const packet_type_t type,
    const packet_action_t action,
    const uint8_t *data,
    const uint8_t length
);

/**
 * @brief make communication packet buffer
 * 
 * @param buffer data buffer (must be 64+ bytes)
 * @param packet packet_comm_t
 * @return uint8_t length of packet buffer
 */
extern uint8_t make_packet_comm(uint8_t *buffer, const packet_comm_t *packet);

/**
 * @brief parse communication packet
 * 
 * @param buffer data buffer
 * @param buffer_len length of data
 * @param out_packet parsed packet
 * @param payload_buf data of packet
 * @return USER_StatusTypeDef 
 */
extern USER_StatusTypeDef parse_packet_comm(
    const uint8_t *buffer,
    const size_t buffer_len,
    packet_comm_t *out_packet,
    uint8_t *payload_buf
);

#endif /* INC_PROTOCOL_H_ */
