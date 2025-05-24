#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "protocol.h"

uint8_t* data_ptr;

uint8_t calculate_checksum(uint8_t *data, uint8_t *checksum) {
    *checksum ^= *data;
    return *checksum;
}

void create_packet_log(packet_log_t *packet, uint32_t timestamp, uint16_t timestamp_ms, uint8_t type, uint8_t action, uint8_t *data, uint8_t length) {
    packet->header = PACKET_HEADER;
    packet->timestamp = timestamp;
    packet->timestamp_ms = timestamp_ms;
    packet->type = type;
    packet->action = action;
    packet->data_length = length;
    packet->data_ptr = data;
    packet->tail = PACKET_TAIL;
}

uint8_t make_packet_log(uint8_t *data, packet_log_t *packet) {
    uint8_t count = 0;

    if(packet->data_length > MAX_PAYLOAD_LENGTH) {
        return 0;
    }

    data[count] = (packet->header >> 8);
    count++;
    data[count] = (packet->header & 0xFF);
    count++;

    data[count] = (packet->timestamp >> 24) & 0xFF;
    count++;
    data[count] = (packet->timestamp >> 16) & 0xFF;
    count++;
    data[count] = (packet->timestamp >> 8) & 0xFF;
    count++;
    data[count] = (packet->timestamp & 0xFF);
    count++;

    data[count] = (packet->timestamp_ms >> 8);
    count++;
    data[count] = (packet->timestamp_ms & 0xFF);
    count++;

    data[count] = packet->type;
    count++;

    data[count] = packet->action;
    count++;

    data[count] = packet->data_length;
    count++;

    for(int i=0; i<packet->data_length; i++) {
        data[count] = packet->data_ptr[i];
        count++;
    }

    data[count] = PACKET_TAIL;
    count++;

    return count;
}

void create_packet_comm(packet_comm_t *packet, uint8_t type, uint8_t action, uint8_t *data, uint8_t length) {
    packet->header = PACKET_HEADER;
    packet->type = type;
    packet->action = action;
    packet->data_length = length;
    packet->data_ptr = data;
    packet->tail = PACKET_TAIL;
}

uint8_t make_packet_comm(uint8_t *data, packet_comm_t *packet) {
    uint8_t count = 0;
    uint8_t checksum = 0x00;

    if(packet->data_length > MAX_PAYLOAD_LENGTH) {
        return 0;
    }

    data[count] = (packet->header >> 8);
    count++;
    data[count] = (packet->header & 0xFF);
    count++;

    data[count] = packet->type;
    calculate_checksum(&data[count], &checksum);
    count++;

    data[count] = packet->action;
    calculate_checksum(&data[count], &checksum);
    count++;

    data[count] = packet->data_length;
    calculate_checksum(&data[count], &checksum);
    count++;

    for(int i=0; i<packet->data_length; i++) {
        data[count] = packet->data_ptr[i];
        calculate_checksum(&data[count], &checksum);
        count++;
    }

    data[count] = checksum;
    count++;

    data[count] = PACKET_TAIL;
    count++;

    return count;
}

int parse_packet(const uint8_t *buffer, size_t buffer_len, packet_log_t *out_packet, uint8_t *payload_buf) {
    if (!buffer || !out_packet || !payload_buf) return -1;

    size_t min_packet_size = 2 + 4 + 2 + 1 + 1 + 1 + 1;
    if (buffer_len < min_packet_size) return -2;

    size_t offset = 0;

    uint16_t header = (buffer[offset+1] << 8) | buffer[offset];
    if (header != PACKET_HEADER) return -3;
    out_packet->header = header;
    offset += 2;

    out_packet->timestamp = (buffer[offset+3] << 24) |
                            (buffer[offset+2] << 16) |
                            (buffer[offset+1] << 8)  |
                            (buffer[offset]);
    offset += 4;

    out_packet->timestamp_ms = (buffer[offset+1] << 8) | buffer[offset];
    offset += 2;

    out_packet->type = buffer[offset++];
    out_packet->action = buffer[offset++];
    out_packet->data_length = buffer[offset++];

    if (out_packet->data_length > MAX_PAYLOAD_LENGTH) return -4;
    if (buffer_len < offset + out_packet->data_length + 1) return -5;

    memcpy(payload_buf, &buffer[offset], out_packet->data_length);
    out_packet->data_ptr = payload_buf;
    offset += out_packet->data_length;

    if (buffer[offset] != PACKET_TAIL) return -6;
    out_packet->tail = buffer[offset];

    return 0;
}

int parse_packet(const uint8_t *buffer, size_t buffer_len, packet_log_t *out_packet, uint8_t *payload_buf) {
    if (!buffer || !out_packet || !payload_buf) return -1;

    size_t min_packet_size = 2 + 4 + 2 + 1 + 1 + 1 + 1;
    if (buffer_len < min_packet_size) return -2;

    size_t offset = 0;

    uint16_t header = (buffer[offset+1] << 8) | buffer[offset];
    if (header != PACKET_HEADER) return -3;
    out_packet->header = header;
    offset += 2;

    out_packet->timestamp = (buffer[offset+3] << 24) |
                            (buffer[offset+2] << 16) |
                            (buffer[offset+1] << 8)  |
                            (buffer[offset]);
    offset += 4;

    out_packet->timestamp_ms = (buffer[offset+1] << 8) | buffer[offset];
    offset += 2;

    out_packet->type = buffer[offset++];
    out_packet->action = buffer[offset++];
    out_packet->data_length = buffer[offset++];

    if (out_packet->data_length > MAX_PAYLOAD_LENGTH) return -4;
    if (buffer_len < offset + out_packet->data_length + 1) return -5;

    memcpy(payload_buf, &buffer[offset], out_packet->data_length);
    out_packet->data_ptr = payload_buf;
    offset += out_packet->data_length;

    if (buffer[offset] != PACKET_TAIL) return -6;
    out_packet->tail = buffer[offset];

    return 0;
}
