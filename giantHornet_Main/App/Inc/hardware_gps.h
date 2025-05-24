#ifndef INC_HARDWARE_GPS_H_
#define INC_HARDWARE_GPS_H_

#include "stm32f4xx_hal.h"

#define GPS_BUFFER_SIZE 1024

typedef struct {
    struct {
        float latitude;      // 위도
        float longitude;     // 경도
        char ns;             // 북/남위
        char ew;             // 동/서경
        float altitude;      // 고도
    } location;

    struct {
        float utc_time;     // UTC
        int32_t date;       // 날짜
    } time;

    struct {
        float speed;        // 속도
        float degree;       // 방향
    } vector;

    struct {
        int satellites;     // 위성 수
        int fix_status;     // GPS fix
    } status;
} GPS_t;

typedef enum {
    NMEA_UNKNOWN = 0,
    NMEA_GGA,
    NMEA_GSV,
    NMEA_RMC,
    NMEA_GLL,
    NMEA_VTG,
} NMEA_Type;

void GPS_Init(UART_HandleTypeDef *huart);
void GPS_buffer_clear(void);
void GPS_data_init(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void GPS_UART_Callback(void);
int GPS_isValid(void);
float GPS_ConvertToDecimalDegrees(float degrees, char direction);
void GPS_DebugPrint(void);
int GPS_Validate_Checksum(char *nmea);
int GPS_ParseNMEA(char *nmea);
int GPS_GGA_Parse(char *nmea);
int GPS_GSV_Parse(char *nmea);
int GPS_RMC_Parse(char *nmea);
int GPS_GLL_Parse(char *nmea);
int GPS_VTG_Parse(char *nmea);

#endif /* INC_HARDWARE_GPS_H_ */
