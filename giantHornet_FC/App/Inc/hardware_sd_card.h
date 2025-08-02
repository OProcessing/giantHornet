#ifndef INC_HARDWARE_SD_CARD_H_
#define INC_HARDWARE_SD_CARD_H_

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "hardware_imu.h"

/* Exported defines ----------------------------------------------------------*/
#define SD_LOG_SUCCESS 1
#define SD_LOG_FAILED  0

/* Exported function prototypes -----------------------------------------------*/

/**
 * @brief Initialize SD card and file system
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_Init(void);

/**
 * @brief Set GPS timestamp for time correction
 * @param gps_time: GPS UTC timestamp (seconds since epoch)
 * @retval None
 */
void SD_SetGPSTime(uint32_t gps_time);

/**
 * @brief Log internal IMU data
 * @param attitude: Pointer to attitude data
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogInternalIMU(Filtered_t *attitude);

/**
 * @brief Log internal pressure sensor data
 * @param pressure: Pressure value
 * @param temperature: Temperature value
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogInternalPressure(float pressure, float temperature);

/**
 * @brief Log external packet data
 * @param packet: Pointer to packet data
 * @param packet_length: Length of packet
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogExternalPacket(uint8_t *packet, uint16_t packet_length);

/**
 * @brief Close log file and flush buffer
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_CloseLogFile(void);

/**
 * @brief Check SD card status
 * @retval 1: OK, 0: Error
 */
uint8_t SD_CheckStatus(void);

/**
 * @brief Force flush buffer to SD card
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_FlushBuffer(void);

// 시간 정보 제공 함수들
/**
 * @brief Get current GPS corrected timestamp (seconds)
 * @retval Current timestamp in seconds
 */
uint32_t SD_GetCurrentTimestamp(void);

/**
 * @brief Get current GPS corrected timestamp (milliseconds)
 * @retval Current timestamp milliseconds
 */
uint16_t SD_GetCurrentTimestampMS(void);

// 포맷된 시간 문자열은 로그 분석 시에만 필요하므로 제거
// 실제 센서 로깅에서는 timestamp와 ms만 사용

// 추가 센서 로깅 함수들
/**
 * @brief Log motor control data
 * @param throttle: Throttle value (0-100)
 * @param rpm: Motor RPM
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogMotorControl(uint16_t throttle, uint16_t rpm);

/**
 * @brief Log distance sensor data
 * @param distance: Distance in meters
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogDistanceSensor(float distance);

/**
 * @brief Log temperature sensor data
 * @param temperature: Temperature in Celsius
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogTemperatureSensor(float temperature);

/**
 * @brief Log battery voltage data
 * @param voltage: Battery voltage in volts
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogBatteryVoltage(float voltage);

/**
 * @brief Log system status
 * @param status_code: Status code
 * @param message: Status message
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogSystemStatus(uint8_t status_code, const char* message);

// FC 내부 센서 로깅 함수들
/**
 * @brief Log raw IMU data (before filtering)
 * @param raw_imu: Pointer to raw IMU data
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogRawIMU(MPU9250_t *raw_imu);

/**
 * @brief Log Kalman filtered attitude data
 * @param filtered: Pointer to filtered attitude data
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogKalmanFiltered(Filtered_t *filtered);

/**
 * @brief Log motor input values
 * @param motor_inputs: Array of motor input values (4 motors)
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogMotorInputs(uint16_t motor_inputs[4]);

/**
 * @brief Log PID controller outputs
 * @param pid_outputs: PID controller outputs (roll, pitch, yaw, throttle)
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogPIDOutputs(float pid_outputs[4]);

/**
 * @brief Log flight controller state
 * @param flight_mode: Current flight mode
 * @param armed: Armed status
 * @param altitude: Current altitude
 * @param vertical_speed: Vertical speed
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogFlightControllerState(uint8_t flight_mode, uint8_t armed, float altitude, float vertical_speed);

// 빈도 제어 로깅 함수들
/**
 * @brief Log raw IMU data with frequency control
 * @param raw_imu: Pointer to raw IMU data
 * @param frequency_hz: Logging frequency in Hz (1-1000)
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogRawIMU_Freq(MPU9250_t *raw_imu, uint16_t frequency_hz);

/**
 * @brief Log Kalman filtered data with frequency control
 * @param filtered: Pointer to filtered attitude data
 * @param frequency_hz: Logging frequency in Hz (1-1000)
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogKalmanFiltered_Freq(Filtered_t *filtered, uint16_t frequency_hz);

/**
 * @brief Log motor inputs with frequency control
 * @param motor_inputs: Array of motor input values (4 motors)
 * @param frequency_hz: Logging frequency in Hz (1-500)
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogMotorInputs_Freq(uint16_t motor_inputs[4], uint16_t frequency_hz);

/**
 * @brief Log PID outputs with frequency control
 * @param pid_outputs: PID controller outputs (roll, pitch, yaw, throttle)
 * @param frequency_hz: Logging frequency in Hz (1-500)
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogPIDOutputs_Freq(float pid_outputs[4], uint16_t frequency_hz);

/**
 * @brief Log flight controller state with frequency control
 * @param flight_mode: Current flight mode
 * @param armed: Armed status
 * @param altitude: Current altitude
 * @param vertical_speed: Vertical speed
 * @param frequency_hz: Logging frequency in Hz (1-100)
 * @retval 1: Success, 0: Failed
 */
uint8_t SD_LogFlightControllerState_Freq(uint8_t flight_mode, uint8_t armed, float altitude, float vertical_speed, uint8_t frequency_hz);

#endif /* INC_HARDWARE_SD_CARD_H_ */
