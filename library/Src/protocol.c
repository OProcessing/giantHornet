#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "define.h"
#include "protocol.h"

uint8_t* data_ptr;

uint8_t calculate_checksum(const uint8_t *data, uint8_t *checksum) {
    *checksum ^= *data;
    return *checksum;
}

void create_packet_log(packet_log_t *packet, const uint32_t timestamp, const uint16_t timestamp_ms, const packet_type_t type, const packet_action_t action, const uint8_t *data, const uint8_t length) {
    packet->header = PACKET_HEADER;
    packet->timestamp = timestamp;
    packet->timestamp_ms = timestamp_ms;
    packet->type = type;
    packet->action = action;
    packet->data_length = length;
    packet->data_ptr = data;
    packet->tail = PACKET_TAIL;
}

uint8_t make_packet_log(uint8_t *buffer, const packet_log_t *packet) {
    uint8_t count = 0;

    if(packet->data_length > MAX_PAYLOAD_LENGTH) {
        return 0;
    }

    buffer[count] = (packet->header >> 8);
    count++;
    buffer[count] = (packet->header & 0xFF);
    count++;

    buffer[count] = (packet->timestamp >> 24) & 0xFF;
    count++;
    buffer[count] = (packet->timestamp >> 16) & 0xFF;
    count++;
    buffer[count] = (packet->timestamp >> 8) & 0xFF;
    count++;
    buffer[count] = (packet->timestamp & 0xFF);
    count++;

    buffer[count] = (packet->timestamp_ms >> 8);
    count++;
    buffer[count] = (packet->timestamp_ms & 0xFF);
    count++;

    buffer[count] = packet->type;
    count++;

    buffer[count] = packet->action;
    count++;

    buffer[count] = packet->data_length;
    count++;

    for(int i=0; i < packet->data_length; i++) {
        buffer[count] = packet->data_ptr[i];
        count++;
    }

    buffer[count] = PACKET_TAIL;
    count++;

    return count;
}

void create_packet_comm(packet_comm_t *packet, const packet_type_t type, const packet_action_t action, const uint8_t *data, const uint8_t length) {
    packet->header = PACKET_HEADER;
    packet->type = type;
    packet->action = action;
    packet->data_length = length;
    packet->data_ptr = data;
    packet->tail = PACKET_TAIL;
}

uint8_t make_packet_comm(uint8_t *buffer, const packet_comm_t *packet) {
    uint8_t count = 0;
    uint8_t checksum = 0x00;

    if(packet->data_length > MAX_PAYLOAD_LENGTH) {
        return 0;
    }

    buffer[count] = (packet->header >> 8);
    count++;
    buffer[count] = (packet->header & 0xFF);
    count++;

    buffer[count] = packet->type;
    calculate_checksum(&buffer[count], &checksum);
    count++;

    buffer[count] = packet->action;
    calculate_checksum(&buffer[count], &checksum);
    count++;

    buffer[count] = packet->data_length;
    calculate_checksum(&buffer[count], &checksum);
    count++;

    for(int i=0; i < packet->data_length; i++) {
        buffer[count] = packet->data_ptr[i];
        calculate_checksum(&buffer[count], &checksum);
        count++;
    }

    buffer[count] = checksum;
    count++;

    buffer[count] = PACKET_TAIL;
    count++;

    return count;
}

USER_StatusTypeDef parse_packet_comm(const uint8_t *buffer, const size_t buffer_len, packet_comm_t *out_packet, uint8_t *payload_buf) {
    if (!buffer || !out_packet || !payload_buf) return USER_RET_ERR_NULL_POINTER;

    size_t min_packet_size = 2 + 1 + 1 + 1 + 0 + 1 + 1;
    if (buffer_len < min_packet_size) return USER_RET_ERR_NO_RESOURCE;

    size_t offset = 0;
    uint8_t checksum = 0x00;

    uint16_t header = (buffer[offset] << 8) | buffer[offset+1];
    if (header != PACKET_HEADER) return USER_RET_ERR_INVALID_PARAM;
    out_packet->header = header;
    offset += 2;

    out_packet->type = buffer[offset];
    calculate_checksum(&buffer[offset], &checksum);
    offset++;

    out_packet->action = buffer[offset];
    calculate_checksum(&buffer[offset], &checksum);
    offset++;

    out_packet->data_length = buffer[offset];
    calculate_checksum(&buffer[offset], &checksum);
    offset++;

    if (out_packet->data_length > MAX_PAYLOAD_LENGTH) return USER_RET_ERR_INVALID_PARAM;
    if (buffer_len < offset + out_packet->data_length + 2) return USER_RET_ERR_NO_RESOURCE;

    for(int i=0; i < out_packet->data_length; i++) {
        payload_buf[i] = buffer[offset + i];
        calculate_checksum(&buffer[offset + i], &checksum);
    }
    offset += out_packet->data_length;

    //if (buffer[offset] != checksum) return USER_RET_ERR_INVALID_PARAM;
    offset++;

    if (buffer[offset] != PACKET_TAIL) return USER_RET_ERR_INVALID_PARAM;
    out_packet->tail = buffer[offset];

    return USER_RET_OK;
}
