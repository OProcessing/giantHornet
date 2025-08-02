#include "define.h"
#include "hardware_gps.h"
#include "ring_buffer.h"
#include "protocol.h"
#include "function_protocol.h"
#include <time.h>

static UART_HandleTypeDef *gps_huart;

uint8_t gps_rx_byte = 0;
uint8_t gps_rx_buffer[GPS_BUFFER_SIZE];
char gps_sentence_buffer[GPS_BUFFER_SIZE];

// GPS 연결 상태 추적
static uint8_t gps_first_fix_sent = 0;  // 첫 번째 GPS fix 시 시간 패킷 전송 여부

GPS_t GPS;
RingBuffer gps_RingBuffer;

extern void RingBuffer_Init(RingBuffer *rb, uint8_t *buf, uint16_t size);

void GPS_Init(UART_HandleTypeDef *huart) 
{
    gps_huart = huart;
    GPS_data_init();
    RingBuffer_Init(&gps_RingBuffer, (uint8_t *)gps_rx_buffer, GPS_BUFFER_SIZE);
    HAL_UART_Receive_IT(gps_huart, &gps_rx_byte, 1);
}

void GPS_buffer_clear(void) 
{
    memset(gps_sentence_buffer, 0, sizeof(gps_sentence_buffer));
}

void GPS_data_init(void) 
{
    memset(&GPS, 0, sizeof(GPS));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == gps_huart)
    {
        RingBuffer_Put(&gps_RingBuffer, gps_rx_byte); 
        HAL_UART_Receive_IT(gps_huart, &gps_rx_byte, 1);
    }
}

void GPS_UART_Callback(void)
{
    static uint16_t gps_buffer_index = 0;
    uint8_t data;

    while (RingBuffer_Get(&gps_RingBuffer, &data)) 
    {
        if (data == '\r') 
        {
            gps_sentence_buffer[gps_buffer_index] = '\0';
            gps_buffer_index = 0;

            if (GPS_Validate_Checksum(gps_sentence_buffer) == 0) 
            {
                GPS_ParseNMEA(gps_sentence_buffer);
            }
            GPS_buffer_clear();
        } 
        else 
        {
            gps_sentence_buffer[gps_buffer_index++] = data;
            if (gps_buffer_index >= GPS_BUFFER_SIZE) 
            {
                gps_buffer_index = 0;
            }
        }
    }
}

int GPS_isValid(void)
{
    if (GPS.status.fix_status == 1 && GPS.status.satellites >= 3) 
    {
        return 0; // valid
    }
    return -1; // invalid
}

float GPS_ConvertToDecimal(float nmea_coord, char direction)
{
    int degrees = (int)(nmea_coord / 100);
    float minutes = nmea_coord - (degrees * 100);
    float decimal = degrees + (minutes / 60.0f);

    if (direction == 'S' || direction == 'W') {
        decimal = -decimal;
    }
    return decimal;
}

void GPS_DebugPrint(void)
{
    printf("\n[GPS DEBUG INFO]\n");
    printf("Latitude: %.6f %c (%.6f deg)\n", GPS.location.latitude, GPS.location.ns, GPS_ConvertToDecimal(GPS.location.latitude, GPS.location.ns));
    printf("Longitude: %.6f %c (%.6f deg)\n", GPS.location.longitude, GPS.location.ew, GPS_ConvertToDecimal(GPS.location.longitude, GPS.location.ew));
    printf("Altitude: %.2f m\n", GPS.location.altitude);
    printf("UTC Time: %.2f\n", GPS.time.utc_time);
    printf("Date: %06ld\n", GPS.time.date);
    printf("Speed: %.2f km/h\n", GPS.vector.speed);
    printf("Course: %.2f degrees\n", GPS.vector.degree);
    printf("Satellites: %d\n", GPS.status.satellites);
    printf("Fix Status: %d\n", GPS.status.fix_status);
}

static NMEA_Type GPS_GetNMEAType(const char *nmea)
{
    if (strncmp(nmea, "$GPGGA", 6) == 0) return NMEA_GPGGA;
    if (strncmp(nmea, "$GPRMC", 6) == 0) return NMEA_GPRMC;
    if (strncmp(nmea, "$GPGLL", 6) == 0) return NMEA_GPGLL;
    if (strncmp(nmea, "$GPVTG", 6) == 0) return NMEA_GPVTG;
    return NMEA_UNKNOWN;
}

int GPS_Validate_Checksum(char *nmea) 
{
    char *checksum_start = strchr(nmea, '*');
    if (!checksum_start || (checksum_start - nmea) > GPS_BUFFER_SIZE - 3)       return -1;

    uint8_t calculated_checksum = 0;
    for (char *ptr = nmea + 1; ptr < checksum_start; ptr++) 
    {
        calculated_checksum ^= *ptr;
    }

    uint8_t received_checksum = (uint8_t)strtol(checksum_start + 1, NULL, 16);
    return calculated_checksum == received_checksum;
}

int GPS_ParseNMEA(char *nmea) 
{
    int ret = -1;
    NMEA_Type type = GPS_GetNMEAType(nmea);

    switch (type)
    {
        case NMEA_UNKNOWN: break;
        case NMEA_GPGGA: ret = GPS_GPGGA_Parse(nmea); break;
        case NMEA_GPGSV: ret = GPS_GPGSV_Parse(nmea); break;
        case NMEA_GPRMC: ret = GPS_GPRMC_Parse(nmea); break;
        case NMEA_GPGLL: ret = GPS_GPGLL_Parse(nmea); break;
        case NMEA_GPVTG: ret = GPS_GPVTG_Parse(nmea); break;
        default: break;

    }
    
    // 첫 번째 GPS fix 시 시간 패킷 전송
    if (GPS_IsFirstFix()) {
        GPS_SendTimePacket();
        gps_first_fix_sent = 1;  // 플래그 설정
    }
    
    GPS_DebugPrint();
    return ret;
}


int GPS_GPGGA_Parse(char *nmea) 
{
    int ret = sscanf(nmea, "$GPGGA,%f,%f,%c,%f,%c,%d,%d,%*f,%f",
        &GPS.time.utc_time,
        &GPS.location.latitude, &GPS.location.ns,
        &GPS.location.longitude, &GPS.location.ew,
        &GPS.status.fix_status, &GPS.status.satellites,
        &GPS.location.altitude);

    if (ret >= 8) 
    {
        return 0;
    }
    return -1;
}

int GPS_GPGSV_Parse(char *nmea)
{
    int total_msgs, msg_num, sats_in_view;
    int ret = sscanf(nmea, "$GPGSV,%d,%d,%d", &total_msgs, &msg_num, &sats_in_view);

    if (ret >= 3)
    {
        GPS.status.satellites = sats_in_view;
        return 0;
    }
    return -1;
}

int GPS_GPRMC_Parse(char *nmea)
{
    float speed_knots, course;
    char status;

    int ret = sscanf(nmea, "$GPRMC,%f,%c,%f,%c,%f,%c,%f,%f,%ld",
        &GPS.time.utc_time,
        &status,
        &GPS.location.latitude, &GPS.location.ns,
        &GPS.location.longitude, &GPS.location.ew,
        &speed_knots, &course, &GPS.time.date);

    if (ret >= 9)
    {
        GPS.vector.speed = speed_knots * 1.852f;
        GPS.vector.degree = course;
        GPS.status.fix_status = (status == 'A') ? 1 : 0;
        return 0;
    }
    return -1;
}

int GPS_GPGLL_Parse(char *nmea)
{
    int ret = sscanf(nmea, "$GPGLL,%f,%c,%f,%c,%f",
        &GPS.location.latitude, &GPS.location.ns,
        &GPS.location.longitude, &GPS.location.ew,
        &GPS.time.utc_time);

    if (ret >= 5)
    {
        return 0;
    }
    return -1;
}

int GPS_GPVTG_Parse(char *nmea)
{
    float true_track, magnetic_track, speed_knots, speed_kph;

    int ret = sscanf(nmea, "$GPVTG,%f,T,%f,M,%f,N,%f,K",
        &true_track, &magnetic_track, &speed_knots, &speed_kph);

    if (ret >= 4)
    {
        GPS.vector.degree = true_track;
        GPS.vector.speed = speed_kph;
        return 0;
    }
    return -1;
}

/**
 * @brief GPS 시간 패킷 전송
 */
void GPS_SendTimePacket(void)
{
    // 간단한 시간 데이터 (예시: 2024년 기준)
    unsigned long unix_time = 1704067200;  // 2024-01-01 00:00:00 UTC
    
    // 시간 데이터를 바이트 배열로 변환
    uint8_t time_data[4];
    time_data[0] = (unix_time >> 24) & 0xFF;
    time_data[1] = (unix_time >> 16) & 0xFF;
    time_data[2] = (unix_time >> 8) & 0xFF;
    time_data[3] = unix_time & 0xFF;
    
    // 패킷 생성 및 전송
    packet_comm_t time_packet;
    create_packet_comm(&time_packet, TYPE_GPS_INIT_TIME, ACTION_PACKET, time_data, 4);
    
    uint8_t packet_buffer[64];
    uint8_t packet_length = make_packet_comm(packet_buffer, &time_packet);
    
    protocol_tx(packet_buffer, packet_length);
    
    printf("GPS Time packet sent: %lu\n", unix_time);
}

/**
 * @brief 첫 번째 GPS fix인지 확인
 * @retval 1: 첫 번째 fix, 0: 이미 fix됨
 */
int GPS_IsFirstFix(void)
{
    return (GPS_isValid() == 0 && gps_first_fix_sent == 0) ? 1 : 0;
}
