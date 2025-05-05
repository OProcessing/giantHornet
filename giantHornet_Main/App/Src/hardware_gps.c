#include "define.h"
#include "hardware_gps.h"

static UART_HandleTypeDef *gps_huart;

static uint8_t rx_data;
static char rx_buffer[GPS_BUFFER_SIZE];
static int rx_index = 0;

GPS_t GPS;

void GPS_Init(UART_HandleTypeDef *huart) 
{
    gps_huart = huart;
    GPS_Clear();
    HAL_UART_Receive_IT(gps_huart, &rx_data, 1);
}

void GPS_Clear(void) 
{
    memset(&GPS, 0, sizeof(GPS));
}

void GPS_UART_Callback(void) 
{
    if (rx_data != '\n' && rx_index < GPS_BUFFER_SIZE - 1) 
    {
        rx_buffer[rx_index++] = rx_data; 
    } 
    else 
    {
        rx_buffer[rx_index] = '\0'; 
        if (GPS_Validate_Checksum(rx_buffer)) 
        { 
            GPS_ParseNMEA(rx_buffer);         
        }
        rx_index = 0;
    }
    HAL_UART_Receive_IT(gps_huart, &rx_data, 1);
}

int GPS_isValid(void)
{
    if (GPS.status.fix_status == 1 && GPS.status.satellites >= 3) 
    {
        return 0; // 유효한 GPS 데이터
    }
    return -1; // 유효하지 않음
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
