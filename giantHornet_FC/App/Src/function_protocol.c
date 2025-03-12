#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define START_BYTE 0xAA
#define END_BYTE   0xBB

#define TYPE_GPS_COORDINATES  0x01
#define TYPE_PID_RESULTS      0x02
#define TYPE_IMU_ALT_GPS      0x03
#define TYPE_PID_CALC_DATA    0x04

#define MAX_PAYLOAD_LENGTH 50

#pragma pack(push, 1)
typedef struct {
    uint8_t start_byte;
    uint8_t data_type;
    uint8_t payload_length;
    uint8_t payload[MAX_PAYLOAD_LENGTH];
    uint8_t checksum;
    uint8_t end_byte;
} DronePacket;
#pragma pack(pop)


uint8_t calculate_checksum(uint8_t *data, uint8_t length) {
    uint8_t checksum = 0;
    for (uint8_t i = 0; i < length; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

void create_packet(DronePacket *packet, uint8_t type, uint8_t *data, uint8_t length) {
    packet->start_byte = START_BYTE;
    packet->data_type = type;
    packet->payload_length = length;
    memcpy(packet->payload, data, length);
    packet->checksum = calculate_checksum(data, length);
    packet->end_byte = END_BYTE;
}

void send_gps_coordinates(float x, float y, float z) {
    uint8_t data[12];
    memcpy(&data[0], &x, sizeof(float));
    memcpy(&data[4], &y, sizeof(float));
    memcpy(&data[8], &z, sizeof(float));

    DronePacket packet;
    create_packet(&packet, TYPE_GPS_COORDINATES, data, sizeof(data));

}