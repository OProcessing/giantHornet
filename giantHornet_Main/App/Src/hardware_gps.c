#include "define.h"
#include "hardware_gps.h"
#include "ring_buffer.h"

static UART_HandleTypeDef *gps_huart;

uint8_t gps_rx_byte = 0;
uint8_t gps_rx_buffer[GPS_BUFFER_SIZE];
char gps_sentence_buffer[GPS_BUFFER_SIZE];


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
    if (nmea == NULL || nmea[0] != '$') return NMEA_UNKNOWN;
    nmea += 3;
    if (strncmp(nmea, "GGA", 3) == 0) return NMEA_GGA;
    if (strncmp(nmea, "RMC", 3) == 0) return NMEA_RMC;
    if (strncmp(nmea, "GLL", 3) == 0) return NMEA_GLL;
    if (strncmp(nmea, "VTG", 3) == 0) return NMEA_VTG;
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
        case NMEA_GGA: ret = GPS_GGA_Parse(nmea); break;
        case NMEA_GSV: ret = GPS_GSV_Parse(nmea); break;
        case NMEA_RMC: ret = GPS_RMC_Parse(nmea); break;
        case NMEA_GLL: ret = GPS_GLL_Parse(nmea); break;
        case NMEA_VTG: ret = GPS_VTG_Parse(nmea); break;
        default: break;

    }
    GPS_DebugPrint();
    return ret;
}


int GPS_GGA_Parse(char *nmea) 
{
    int ret = sscanf(nmea, "GGA,%f,%f,%c,%f,%c,%d,%d,%*f,%f",
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

int GPS_GSV_Parse(char *nmea)
{
    int total_msgs, msg_num, sats_in_view;
    int ret = sscanf(nmea, "GSV,%d,%d,%d", &total_msgs, &msg_num, &sats_in_view);

    if (ret >= 3)
    {
        GPS.status.satellites = sats_in_view;
        return 0;
    }
    return -1;
}

int GPS_RMC_Parse(char *nmea)
{
    float speed_knots, course;
    char status;

    int ret = sscanf(nmea, "RMC,%f,%c,%f,%c,%f,%c,%f,%f,%ld",
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

int GPS_GLL_Parse(char *nmea)
{
    int ret = sscanf(nmea, "GLL,%f,%c,%f,%c,%f",
        &GPS.location.latitude, &GPS.location.ns,
        &GPS.location.longitude, &GPS.location.ew,
        &GPS.time.utc_time);

    if (ret >= 5)
    {
        return 0;
    }
    return -1;
}

int GPS_VTG_Parse(char *nmea)
{
    float true_track, magnetic_track, speed_knots, speed_kph;

    int ret = sscanf(nmea, "VTG,%f,T,%f,M,%f,N,%f,K",
        &true_track, &magnetic_track, &speed_knots, &speed_kph);

    if (ret >= 4)
    {
        GPS.vector.degree = true_track;
        GPS.vector.speed = speed_kph;
        return 0;
    }
    return -1;
}
