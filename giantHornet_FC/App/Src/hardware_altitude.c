/**
 * Ciastkolog.pl (https://github.com/ciastkolog)
 * 
*/
/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 sheinz (https://github.com/sheinz)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "hardware_altitude.h"

/**
 * BMP280 registers
 */
#define BMP280_REG_TEMP_XLSB   0xFC /* bits: 7-4 */
#define BMP280_REG_TEMP_LSB    0xFB
#define BMP280_REG_TEMP_MSB    0xFA
#define BMP280_REG_TEMP        (BMP280_REG_TEMP_MSB)
#define BMP280_REG_PRESS_XLSB  0xF9 /* bits: 7-4 */
#define BMP280_REG_PRESS_LSB   0xF8
#define BMP280_REG_PRESS_MSB   0xF7
#define BMP280_REG_PRESSURE    (BMP280_REG_PRESS_MSB)
#define BMP280_REG_CONFIG      0xF5 /* bits: 7-5 t_sb; 4-2 filter; 0 spi3w_en */
#define BMP280_REG_CTRL        0xF4 /* bits: 7-5 osrs_t; 4-2 osrs_p; 1-0 mode */
#define BMP280_REG_STATUS      0xF3 /* bits: 3 measuring; 0 im_update */
#define BMP280_REG_CTRL_HUM    0xF2 /* bits: 2-0 osrs_h; */
#define BMP280_REG_RESET       0xE0
#define BMP280_REG_ID          0xD0
#define BMP280_REG_CALIB       0x88
#define BMP280_REG_HUM_CALIB   0x88

#define BMP280_RESET_VALUE     0xB6



void bmp280_init_default_params(bmp280_params_t *params) {
    params->mode = BMP280_MODE_NORMAL;
    params->filter = BMP280_FILTER_OFF;
    params->oversampling_pressure = BMP280_STANDARD;
    params->oversampling_temperature = BMP280_STANDARD;
    params->oversampling_humidity = BMP280_STANDARD;
    params->standby = BMP280_STANDBY_250;
}

static bool read_register16(BMP280_HandleTypedef *dev, uint8_t addr, uint16_t *value) {
    uint16_t tx_buff;
    uint8_t rx_buff[2];
    tx_buff = (dev->addr << 1);

    if (HAL_I2C_Mem_Read(dev->i2c, tx_buff, addr, 1, rx_buff, 2, 5000)
            == HAL_OK) {
        *value = (uint16_t) ((rx_buff[1] << 8) | rx_buff[0]);
        return true;
    } else
        return false;

}

static inline int read_data(BMP280_HandleTypedef *dev, uint8_t addr, uint8_t *value,
        uint8_t len) {
    uint16_t tx_buff;
    tx_buff = (dev->addr << 1);
    if (HAL_I2C_Mem_Read(dev->i2c, tx_buff, addr, 1, value, len, 5000) == HAL_OK)

        return 0;
    else
        return 1;

}

static bool read_calibration_data(BMP280_HandleTypedef *dev) {

    if (read_register16(dev, 0x88, &dev->dig_T1)
            && read_register16(dev, 0x8a, (uint16_t *) &dev->dig_T2)
            && read_register16(dev, 0x8c, (uint16_t *) &dev->dig_T3)
            && read_register16(dev, 0x8e, &dev->dig_P1)
            && read_register16(dev, 0x90, (uint16_t *) &dev->dig_P2)
            && read_register16(dev, 0x92, (uint16_t *) &dev->dig_P3)
            && read_register16(dev, 0x94, (uint16_t *) &dev->dig_P4)
            && read_register16(dev, 0x96, (uint16_t *) &dev->dig_P5)
            && read_register16(dev, 0x98, (uint16_t *) &dev->dig_P6)
            && read_register16(dev, 0x9a, (uint16_t *) &dev->dig_P7)
            && read_register16(dev, 0x9c, (uint16_t *) &dev->dig_P8)
            && read_register16(dev, 0x9e,
                    (uint16_t *) &dev->dig_P9)) {

        return true;
    }

    return false;
}

static bool read_hum_calibration_data(BMP280_HandleTypedef *dev) {
    uint16_t h4, h5;

    if (!read_data(dev, 0xa1, &dev->dig_H1, 1)
            && read_register16(dev, 0xe1, (uint16_t *) &dev->dig_H2)
            && !read_data(dev, 0xe3, &dev->dig_H3, 1)
            && read_register16(dev, 0xe4, &h4)
            && read_register16(dev, 0xe5, &h5)
            && !read_data(dev, 0xe7, (uint8_t *) &dev->dig_H6, 1)) {
        dev->dig_H4 = (h4 & 0x00ff) << 4 | (h4 & 0x0f00) >> 8;
        dev->dig_H5 = h5 >> 4;

        return true;
    }

    return false;
}

static int write_register8(BMP280_HandleTypedef *dev, uint8_t addr, uint8_t value) {
    uint16_t tx_buff;

    tx_buff = (dev->addr << 1);

    if (HAL_I2C_Mem_Write(dev->i2c, tx_buff, addr, 1, &value, 1, 10000) == HAL_OK)
        return false;
    else
        return true;
}

bool bmp280_init(BMP280_HandleTypedef *dev, bmp280_params_t *params) {

    if (dev->addr != BMP280_I2C_ADDRESS_0
            && dev->addr != BMP280_I2C_ADDRESS_1) {

        return false;
    }

    if (read_data(dev, BMP280_REG_ID, &dev->id, 1)) {
        return false;
    }

    if (dev->id != BMP280_CHIP_ID && dev->id != BME280_CHIP_ID) {

        return false;
    }

    // Soft reset.
    if (write_register8(dev, BMP280_REG_RESET, BMP280_RESET_VALUE)) {
        return false;
    }

    // Wait until finished copying over the NVP data.
    while (1) {
        uint8_t status;
        if (!read_data(dev, BMP280_REG_STATUS, &status, 1)
                && (status & 1) == 0)
            break;
    }

    if (!read_calibration_data(dev)) {
        return false;
    }

    if (dev->id == BME280_CHIP_ID && !read_hum_calibration_data(dev)) {
        return false;
    }

    uint8_t config = (params->standby << 5) | (params->filter << 2);
    if (write_register8(dev, BMP280_REG_CONFIG, config)) {
        return false;
    }

    if (params->mode == BMP280_MODE_FORCED) {
        params->mode = BMP280_MODE_SLEEP;  // initial mode for forced is sleep
    }

    uint8_t ctrl = (params->oversampling_temperature << 5)
            | (params->oversampling_pressure << 2) | (params->mode);

    if (dev->id == BME280_CHIP_ID) {
        // Write crtl hum reg first, only active after write to BMP280_REG_CTRL.
        uint8_t ctrl_hum = params->oversampling_humidity;
        if (write_register8(dev, BMP280_REG_CTRL_HUM, ctrl_hum)) {
            return false;
        }
    }

    if (write_register8(dev, BMP280_REG_CTRL, ctrl)) {
        return false;
    }

    return true;
}

bool bmp280_force_measurement(BMP280_HandleTypedef *dev) {
    uint8_t ctrl;
    if (read_data(dev, BMP280_REG_CTRL, &ctrl, 1))
        return false;
    ctrl &= ~0b11;  // clear two lower bits
    ctrl |= BMP280_MODE_FORCED;
    if (write_register8(dev, BMP280_REG_CTRL, ctrl)) {
        return false;
    }
    return true;
}

bool bmp280_is_measuring(BMP280_HandleTypedef *dev) {
    uint8_t status;
    if (read_data(dev, BMP280_REG_STATUS, &status, 1))
        return false;
    if (status & (1 << 3)) {
        return true;
    }
    return false;
}

/**
 * Compensation algorithm is taken from BMP280 datasheet.
 *
 * Return value is in degrees Celsius.
 */
static inline int32_t compensate_temperature(BMP280_HandleTypedef *dev, int32_t adc_temp,
        int32_t *fine_temp) {
    int32_t var1, var2;

    var1 = ((((adc_temp >> 3) - ((int32_t) dev->dig_T1 << 1)))
            * (int32_t) dev->dig_T2) >> 11;
    var2 = (((((adc_temp >> 4) - (int32_t) dev->dig_T1)
            * ((adc_temp >> 4) - (int32_t) dev->dig_T1)) >> 12)
            * (int32_t) dev->dig_T3) >> 14;

    *fine_temp = var1 + var2;
    return (*fine_temp * 5 + 128) >> 8;
}

/**
 * Compensation algorithm is taken from BMP280 datasheet.
 *
 * Return value is in Pa, 24 integer bits and 8 fractional bits.
 */
static inline uint32_t compensate_pressure(BMP280_HandleTypedef *dev, int32_t adc_press,
        int32_t fine_temp) {
    int64_t var1, var2, p;

    var1 = (int64_t) fine_temp - 128000;
    var2 = var1 * var1 * (int64_t) dev->dig_P6;
    var2 = var2 + ((var1 * (int64_t) dev->dig_P5) << 17);
    var2 = var2 + (((int64_t) dev->dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t) dev->dig_P3) >> 8)
            + ((var1 * (int64_t) dev->dig_P2) << 12);
    var1 = (((int64_t) 1 << 47) + var1) * ((int64_t) dev->dig_P1) >> 33;

    if (var1 == 0) {
        return 0;  // avoid exception caused by division by zero
    }

    p = 1048576 - adc_press;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = ((int64_t) dev->dig_P9 * (p >> 13) * (p >> 13)) >> 25;
    var2 = ((int64_t) dev->dig_P8 * p) >> 19;

    p = ((p + var1 + var2) >> 8) + ((int64_t) dev->dig_P7 << 4);
    return p;
}

/**
 * Compensation algorithm is taken from BME280 datasheet.
 *
 * Return value is in Pa, 24 integer bits and 8 fractional bits.
 */
static inline uint32_t compensate_humidity(BMP280_HandleTypedef *dev, int32_t adc_hum,
        int32_t fine_temp) {
    int32_t v_x1_u32r;

    v_x1_u32r = fine_temp - (int32_t) 76800;
    v_x1_u32r = ((((adc_hum << 14) - ((int32_t) dev->dig_H4 << 20)
            - ((int32_t) dev->dig_H5 * v_x1_u32r)) + (int32_t) 16384) >> 15)
            * (((((((v_x1_u32r * (int32_t) dev->dig_H6) >> 10)
                    * (((v_x1_u32r * (int32_t) dev->dig_H3) >> 11)
                            + (int32_t) 32768)) >> 10) + (int32_t) 2097152)
                    * (int32_t) dev->dig_H2 + 8192) >> 14);
    v_x1_u32r = v_x1_u32r
            - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7)
                    * (int32_t) dev->dig_H1) >> 4);
    v_x1_u32r = v_x1_u32r < 0 ? 0 : v_x1_u32r;
    v_x1_u32r = v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r;
    return v_x1_u32r >> 12;
}

bool bmp280_read_fixed(BMP280_HandleTypedef *dev, int32_t *temperature, uint32_t *pressure,
        uint32_t *humidity) {
    int32_t adc_pressure;
    int32_t adc_temp;
    uint8_t data[8];

    // Only the BME280 supports reading the humidity.
    if (dev->id != BME280_CHIP_ID) {
        if (humidity)
            *humidity = 0;
        humidity = NULL;
    }

    // Need to read in one sequence to ensure they match.
    size_t size = humidity ? 8 : 6;
    if (read_data(dev, 0xf7, data, size)) {
        return false;
    }

    adc_pressure = data[0] << 12 | data[1] << 4 | data[2] >> 4;
    adc_temp = data[3] << 12 | data[4] << 4 | data[5] >> 4;

    int32_t fine_temp;
    *temperature = compensate_temperature(dev, adc_temp, &fine_temp);
    *pressure = compensate_pressure(dev, adc_pressure, fine_temp);

    if (humidity) {
        int32_t adc_humidity = data[6] << 8 | data[7];
        *humidity = compensate_humidity(dev, adc_humidity, fine_temp);
    }

    return true;
}

bool bmp280_read_float(BMP280_HandleTypedef *dev, float *temperature, float *pressure,
        float *humidity) {
    int32_t fixed_temperature;
    uint32_t fixed_pressure;
    uint32_t fixed_humidity;
    if (bmp280_read_fixed(dev, &fixed_temperature, &fixed_pressure,
            humidity ? &fixed_humidity : NULL)) {
        *temperature = (float) fixed_temperature / 100;
        *pressure = (float) fixed_pressure / 256;
        if (humidity)
            *humidity = (float) fixed_humidity / 1024;
        return true;
    }

    return false;
}
