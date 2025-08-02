/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    hardware_sd_card.c
  * @brief   SD Card logging functionality
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "hardware_sd_card.h"
#include "fatfs.h"
#include "main.h"
#include "string.h"
#include "stdio.h"
#include "stdlib.h"

/* Private defines -----------------------------------------------------------*/
#define MAX_LOG_LINE_LENGTH 256
#define MAX_FILENAME_LENGTH 32
#define LOG_BUFFER_SIZE 512

/* Private variables ---------------------------------------------------------*/
static FIL logFile;
static char logBuffer[LOG_BUFFER_SIZE];
static uint16_t bufferIndex = 0;
static uint8_t sdInitialized = 0;
static char currentFilename[MAX_FILENAME_LENGTH];
static uint32_t systemStartTime = 0;  // 시스템 시작 시간 저장

// GPS 시간 보정을 위한 변수들
static uint32_t gpsTimestamp = 0;     // GPS에서 받은 UTC timestamp
static uint32_t gpsTimeReceived = 0;  // GPS 시간을 받은 시점의 시스템 시간
static uint8_t gpsTimeValid = 0;      // GPS 시간이 유효한지 여부

/* Private function prototypes -----------------------------------------------*/
static uint8_t SD_WriteBuffer(void);
static uint8_t SD_CreateFilename(char *filename);
static uint8_t SD_WriteCSVLine(const char *line);
static void SD_GetTimestamp(uint32_t *timestamp, uint16_t *ms);
static uint32_t SD_GetCorrectedTimestamp(void);

/* USER CODE BEGIN 0 */

/**
 * @brief Set GPS timestamp for time correction
 * @param gps_time: GPS UTC timestamp (seconds since epoch)
 * @retval None
 */
void SD_SetGPSTime(uint32_t gps_time)
{
    gpsTimestamp = gps_time;
    gpsTimeReceived = HAL_GetTick();
    gpsTimeValid = 1;
}

/**
 * @brief Get corrected timestamp using GPS time
 * @retval Corrected timestamp in seconds
 */
static uint32_t SD_GetCorrectedTimestamp(void)
{
    if (gpsTimeValid) {
        // GPS 시간을 받은 시점부터의 경과 시간을 계산
        uint32_t elapsed_since_gps = (HAL_GetTick() - gpsTimeReceived) / 1000;
        return gpsTimestamp + elapsed_since_gps;
    } else {
        // GPS 시간이 없으면 시스템 시작 시간 기준
        return HAL_GetTick() / 1000;
    }
}

/**
 * @brief Get current timestamp relative to system start or GPS time
 * @param timestamp: Output timestamp in seconds
 * @param ms: Output milliseconds
 */
static void SD_GetTimestamp(uint32_t *timestamp, uint16_t *ms)
{
    *timestamp = SD_GetCorrectedTimestamp();
    *ms = HAL_GetTick() % 1000;
}

/**
 * @brief Initialize SD card and file system
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_Init(void)
{
    // Store system start time
    systemStartTime = HAL_GetTick();
    
    // Mount SD card
    if (f_mount(&SDFatFS, SDPath, 1) != FR_OK) {
        return 0;
    }
    
    // Create filename with current date
    if (!SD_CreateFilename(currentFilename)) {
        return 0;
    }
    
    // Create and open log file
    if (f_open(&logFile, currentFilename, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK) {
        return 0;
    }
    
    // Write CSV header
    const char *header = "timestamp,timestamp_ms,sensor_type,data\n";
    if (f_write(&logFile, header, strlen(header), NULL) != FR_OK) {
        f_close(&logFile);
        return 0;
    }
    
    f_sync(&logFile);
    sdInitialized = 1;
    bufferIndex = 0;
    
    return 1;
}

/**
 * @brief Create filename with current date
 * @param filename: Output filename buffer
 * @retval 1: Success, 0: Failed
 */
static uint8_t SD_CreateFilename(char *filename)
{
    // Get current time (simplified - you might want to use RTC)
    uint32_t tick = HAL_GetTick();
    uint32_t seconds = tick / 1000;
    uint32_t minutes = seconds / 60;
    uint32_t hours = minutes / 60;
    uint32_t days = hours / 24;
    
    // Simple date calculation (approximate)
    uint32_t year = 2024;
    uint32_t month = 1;
    uint32_t day = (days % 365) + 1;

    sprintf(filename, "%04d_%02d_%02d_log.csv", year, month, day);
    return 1;
}

/**
 * @brief Write buffer to SD card
 * @retval 1: Success, 0: Failed
 */
static uint8_t SD_WriteBuffer(void)
{
    if (bufferIndex > 0) {
        UINT bytesWritten;
        if (f_write(&logFile, logBuffer, bufferIndex, &bytesWritten) != FR_OK) {
            return 0;
        }
        bufferIndex = 0;
        f_sync(&logFile);
    }
    return 1;
}

/**
 * @brief Write CSV line to buffer
 * @param line: CSV line to write
 * @retval 1: Success, 0: Failed
 */
static uint8_t SD_WriteCSVLine(const char *line)
{
    uint16_t lineLen = strlen(line);
    
    // Check if buffer has enough space
    if (bufferIndex + lineLen >= LOG_BUFFER_SIZE) {
        if (!SD_WriteBuffer()) {
            return 0;
        }
    }
    
    // Add line to buffer
    memcpy(&logBuffer[bufferIndex], line, lineLen);
    bufferIndex += lineLen;
    
    return 1;
}

/**
 * @brief Log internal IMU data
 * @param attitude: Pointer to attitude data
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogInternalIMU(Filtered_t *attitude)
{
    if (!sdInitialized) return 0;
    
    uint32_t timestamp;
    uint16_t ms;
    SD_GetTimestamp(&timestamp, &ms);
    
    char line[MAX_LOG_LINE_LENGTH];
    sprintf(line, "%lu,%u,IMU,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\n",
            timestamp, ms,
            attitude->attitude.roll, attitude->attitude.pitch, attitude->attitude.yaw,
            attitude->accel.roll, attitude->accel.pitch, attitude->accel.yaw,
            attitude->gyro.roll, attitude->gyro.pitch, attitude->gyro.yaw);
    
    return SD_WriteCSVLine(line);
}

/**
 * @brief Log internal pressure sensor data
 * @param pressure: Pressure value
 * @param temperature: Temperature value
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogInternalPressure(float pressure, float temperature)
{
    if (!sdInitialized) return 0;
    
    uint32_t timestamp;
    uint16_t ms;
    SD_GetTimestamp(&timestamp, &ms);
    
    char line[MAX_LOG_LINE_LENGTH];
    sprintf(line, "%lu,%u,PRESSURE,%.2f,%.2f\n",
            timestamp, ms, pressure, temperature);
    
    return SD_WriteCSVLine(line);
}

/**
 * @brief Log external packet data
 * @param packet: Pointer to packet data
 * @param packet_length: Length of packet
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogExternalPacket(uint8_t *packet, uint16_t packet_length)
{
    if (!sdInitialized || packet_length < 13) return 0;
    
    // Parse packet header
    uint32_t timestamp = (packet[2] << 24) | (packet[3] << 16) | (packet[4] << 8) | packet[5];
    uint16_t timestamp_ms = (packet[6] << 8) | packet[7];
    uint8_t sensor_type = packet[8];
    uint8_t action = packet[9];
    uint8_t data_length = packet[10];
    
    // Convert data to hex string
    char hex_data[101] = "0x"; // 50 bytes * 2 + "0x" = 102 chars
    for (int i = 0; i < data_length && i < 50; i++) {
        sprintf(&hex_data[2 + i*2], "%02X", packet[11 + i]);
    }
    
    char line[MAX_LOG_LINE_LENGTH];
    sprintf(line, "%lu,%u,EXT_%d,%d,%s\n",
            timestamp, timestamp_ms, sensor_type, data_length, hex_data);
    
    return SD_WriteCSVLine(line);
}

/**
 * @brief Close log file and flush buffer
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_CloseLogFile(void)
{
    if (!sdInitialized) return 0;
    
    SD_WriteBuffer();
    f_close(&logFile);
    sdInitialized = 0;
    
    return 1;
}

/**
 * @brief Check SD card status
 * @retval 1: OK, 0: Error
 */
uint8_t SD_CheckStatus(void)
{
    if (!sdInitialized) return 0;
    
    DSTATUS status = disk_status(0);
    return (status == 0) ? 1 : 0;
}

/**
 * @brief Force flush buffer to SD card
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_FlushBuffer(void)
{
    return SD_WriteBuffer();
}

// 시간 정보 제공 함수들
uint32_t SD_GetCurrentTimestamp(void)
{
    return SD_GetCorrectedTimestamp();
}

uint16_t SD_GetCurrentTimestampMS(void)
{
    return HAL_GetTick() % 1000;
}

// SD_GetFormattedTime 함수 제거 - 로그 분석 시에만 필요
// 실제 센서 로깅에서는 timestamp와 ms만 사용하여 오버헤드 최소화

// 추가 센서 로깅 함수들
uint8_t SD_LogMotorControl(uint16_t throttle, uint16_t rpm)
{
    if (!sdInitialized) return 0;
    
    uint32_t timestamp;
    uint16_t ms;
    SD_GetTimestamp(&timestamp, &ms);
    
    char line[MAX_LOG_LINE_LENGTH];
    sprintf(line, "%lu,%u,MOTOR,%u,%u\n",
            timestamp, ms, throttle, rpm);
    
    return SD_WriteCSVLine(line);
}

uint8_t SD_LogDistanceSensor(float distance)
{
    if (!sdInitialized) return 0;
    
    uint32_t timestamp;
    uint16_t ms;
    SD_GetTimestamp(&timestamp, &ms);
    
    char line[MAX_LOG_LINE_LENGTH];
    sprintf(line, "%lu,%u,DISTANCE,%.2f\n",
            timestamp, ms, distance);
    
    return SD_WriteCSVLine(line);
}

uint8_t SD_LogTemperatureSensor(float temperature)
{
    if (!sdInitialized) return 0;
    
    uint32_t timestamp;
    uint16_t ms;
    SD_GetTimestamp(&timestamp, &ms);
    
    char line[MAX_LOG_LINE_LENGTH];
    sprintf(line, "%lu,%u,TEMPERATURE,%.2f\n",
            timestamp, ms, temperature);
    
    return SD_WriteCSVLine(line);
}

uint8_t SD_LogBatteryVoltage(float voltage)
{
    if (!sdInitialized) return 0;
    
    uint32_t timestamp;
    uint16_t ms;
    SD_GetTimestamp(&timestamp, &ms);
    
    char line[MAX_LOG_LINE_LENGTH];
    sprintf(line, "%lu,%u,BATTERY,%.2f\n",
            timestamp, ms, voltage);
    
    return SD_WriteCSVLine(line);
}

uint8_t SD_LogSystemStatus(uint8_t status_code, const char* message)
{
    if (!sdInitialized) return 0;
    
    uint32_t timestamp;
    uint16_t ms;
    SD_GetTimestamp(&timestamp, &ms);
    
    char line[MAX_LOG_LINE_LENGTH];
    sprintf(line, "%lu,%u,SYSTEM,%u,%s\n",
            timestamp, ms, status_code, message);
    
    return SD_WriteCSVLine(line);
}

// FC 내부 센서 로깅 함수들
uint8_t SD_LogRawIMU(MPU9250_t *raw_imu)
{
    if (!sdInitialized) return 0;
    
    uint32_t timestamp;
    uint16_t ms;
    SD_GetTimestamp(&timestamp, &ms);
    
    char line[MAX_LOG_LINE_LENGTH];
    sprintf(line, "%lu,%u,RAW_IMU,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\n",
            timestamp, ms,
            raw_imu->sensorData.ax, raw_imu->sensorData.ay, raw_imu->sensorData.az,
            raw_imu->sensorData.gx, raw_imu->sensorData.gy, raw_imu->sensorData.gz,
            raw_imu->attitude.roll, raw_imu->attitude.pitch, raw_imu->attitude.yaw);
    
    return SD_WriteCSVLine(line);
}

uint8_t SD_LogKalmanFiltered(Filtered_t *filtered)
{
    if (!sdInitialized) return 0;
    
    uint32_t timestamp;
    uint16_t ms;
    SD_GetTimestamp(&timestamp, &ms);
    
    char line[MAX_LOG_LINE_LENGTH];
    sprintf(line, "%lu,%u,KALMAN,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\n",
            timestamp, ms,
            filtered->attitude.roll, filtered->attitude.pitch, filtered->attitude.yaw,
            filtered->accel.roll, filtered->accel.pitch, filtered->accel.yaw,
            filtered->gyro.roll, filtered->gyro.pitch, filtered->gyro.yaw);
    
    return SD_WriteCSVLine(line);
}

uint8_t SD_LogMotorInputs(uint16_t motor_inputs[4])
{
    if (!sdInitialized) return 0;
    
    uint32_t timestamp;
    uint16_t ms;
    SD_GetTimestamp(&timestamp, &ms);
    
    char line[MAX_LOG_LINE_LENGTH];
    sprintf(line, "%lu,%u,MOTOR_INPUT,%u,%u,%u,%u\n",
            timestamp, ms,
            motor_inputs[0], motor_inputs[1], motor_inputs[2], motor_inputs[3]);
    
    return SD_WriteCSVLine(line);
}

uint8_t SD_LogPIDOutputs(float pid_outputs[4])
{
    if (!sdInitialized) return 0;
    
    uint32_t timestamp;
    uint16_t ms;
    SD_GetTimestamp(&timestamp, &ms);
    
    char line[MAX_LOG_LINE_LENGTH];
    sprintf(line, "%lu,%u,PID_OUTPUT,%.3f,%.3f,%.3f,%.3f\n",
            timestamp, ms,
            pid_outputs[0], pid_outputs[1], pid_outputs[2], pid_outputs[3]);
    
    return SD_WriteCSVLine(line);
}

uint8_t SD_LogFlightControllerState(uint8_t flight_mode, uint8_t armed, float altitude, float vertical_speed)
{
    if (!sdInitialized) return 0;
    
    uint32_t timestamp;
    uint16_t ms;
    SD_GetTimestamp(&timestamp, &ms);
    
    char line[MAX_LOG_LINE_LENGTH];
    sprintf(line, "%lu,%u,FC_STATE,%u,%u,%.2f,%.2f\n",
            timestamp, ms, flight_mode, armed, altitude, vertical_speed);
    
    return SD_WriteCSVLine(line);
}

// 빈도 제어 로깅 함수들
uint8_t SD_LogRawIMU_Freq(MPU9250_t *raw_imu, uint16_t frequency_hz)
{
    static uint32_t last_log_time = 0;
    static uint32_t call_count = 0;
    
    call_count++;
    uint32_t current_time = HAL_GetTick();
    
    // 빈도 계산: 1000ms / frequency_hz
    uint32_t interval_ms = 1000 / frequency_hz;
    
    if (current_time - last_log_time >= interval_ms) {
        last_log_time = current_time;
        return SD_LogRawIMU(raw_imu);
    }
    
    return 0; // 로깅하지 않음
}

uint8_t SD_LogKalmanFiltered_Freq(Filtered_t *filtered, uint16_t frequency_hz)
{
    static uint32_t last_log_time = 0;
    static uint32_t call_count = 0;
    
    call_count++;
    uint32_t current_time = HAL_GetTick();
    
    // 빈도 계산: 1000ms / frequency_hz
    uint32_t interval_ms = 1000 / frequency_hz;
    
    if (current_time - last_log_time >= interval_ms) {
        last_log_time = current_time;
        return SD_LogKalmanFiltered(filtered);
    }
    
    return 0; // 로깅하지 않음
}

uint8_t SD_LogMotorInputs_Freq(uint16_t motor_inputs[4], uint16_t frequency_hz)
{
    static uint32_t last_log_time = 0;
    static uint32_t call_count = 0;
    
    call_count++;
    uint32_t current_time = HAL_GetTick();
    
    // 빈도 계산: 1000ms / frequency_hz
    uint32_t interval_ms = 1000 / frequency_hz;
    
    if (current_time - last_log_time >= interval_ms) {
        last_log_time = current_time;
        return SD_LogMotorInputs(motor_inputs);
    }
    
    return 0; // 로깅하지 않음
}

uint8_t SD_LogPIDOutputs_Freq(float pid_outputs[4], uint16_t frequency_hz)
{
    static uint32_t last_log_time = 0;
    static uint32_t call_count = 0;
    
    call_count++;
    uint32_t current_time = HAL_GetTick();
    
    // 빈도 계산: 1000ms / frequency_hz
    uint32_t interval_ms = 1000 / frequency_hz;
    
    if (current_time - last_log_time >= interval_ms) {
        last_log_time = current_time;
        return SD_LogPIDOutputs(pid_outputs);
    }
    
    return 0; // 로깅하지 않음
}

uint8_t SD_LogFlightControllerState_Freq(uint8_t flight_mode, uint8_t armed, float altitude, float vertical_speed, uint8_t frequency_hz)
{
    static uint32_t last_log_time = 0;
    static uint32_t call_count = 0;
    
    call_count++;
    uint32_t current_time = HAL_GetTick();
    
    // 빈도 계산: 1000ms / frequency_hz
    uint32_t interval_ms = 1000 / frequency_hz;
    
    if (current_time - last_log_time >= interval_ms) {
        last_log_time = current_time;
        return SD_LogFlightControllerState(flight_mode, armed, altitude, vertical_speed);
    }
    
    return 0; // 로깅하지 않음
}

/* USER CODE END 0 */
