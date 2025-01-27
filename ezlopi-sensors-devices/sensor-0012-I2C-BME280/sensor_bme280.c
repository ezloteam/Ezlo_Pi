/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    sensor_bme280.c
 * @brief   perform some function on bme280
 * @author
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include "sensor_bme280.h"
#include <inttypes.h>
#include "esp_err.h"
#include "esp_log.h"
#include "ezlopi_hal_i2c_master.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
#define I2C_FREQ_HZ 1000000 // Max 1MHz for esp-idf
/**
 * BMP280 registers
 */
#define BMP280_REG_TEMP_XLSB 0xFC /* bits: 7-4 */
#define BMP280_REG_TEMP_LSB 0xFB
#define BMP280_REG_TEMP_MSB 0xFA
#define BMP280_REG_TEMP (BMP280_REG_TEMP_MSB)
#define BMP280_REG_PRESS_XLSB 0xF9 /* bits: 7-4 */
#define BMP280_REG_PRESS_LSB 0xF8
#define BMP280_REG_PRESS_MSB 0xF7
#define BMP280_REG_PRESSURE (BMP280_REG_PRESS_MSB)
#define BMP280_REG_CONFIG 0xF5   /* bits: 7-5 t_sb; 4-2 filter; 0 spi3w_en */
#define BMP280_REG_CTRL 0xF4     /* bits: 7-5 osrs_t; 4-2 osrs_p; 1-0 mode */
#define BMP280_REG_STATUS 0xF3   /* bits: 3 measuring; 0 im_update */
#define BMP280_REG_CTRL_HUM 0xF2 /* bits: 2-0 osrs_h; */
#define BMP280_REG_RESET 0xE0
#define BMP280_REG_ID 0xD0
#define BMP280_REG_CALIB 0x88
#define BMP280_REG_HUM_CALIB 0x88

#define BMP280_RESET_VALUE 0xB6

#define CHECK(x)                \
    do                          \
    {                           \
        esp_err_t __;           \
        if ((__ = x) != ESP_OK) \
            return __;          \
    } while (0)
#define CHECK_ARG(VAL)                  \
    do                                  \
    {                                   \
        if (!(VAL))                     \
            return ESP_ERR_INVALID_ARG; \
    } while (0)
#define CHECK_LOGE(dev, x, msg, ...)                \
    do                                              \
    {                                               \
        esp_err_t __;                               \
        if ((__ = x) != ESP_OK)                     \
        {                                           \
            ESP_LOGE("bmp280", msg, ##__VA_ARGS__); \
            return __;                              \
        }                                           \
    } while (0)

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
inline static esp_err_t read_register_8(s_ezlopi_i2c_master_t *i2c_master_conf, uint8_t reg, uint8_t *r);
static esp_err_t read_register16(s_ezlopi_i2c_master_t *i2c_master_conf, uint8_t reg, uint16_t *r);
static esp_err_t read_calibration_data(s_ezlopi_i2c_master_t *i2c_master_conf, bmp280_t *dev);
static esp_err_t read_hum_calibration_data(s_ezlopi_i2c_master_t *i2c_master_conf, bmp280_t *dev);
static inline int32_t compensate_temperature(bmp280_t *dev, int32_t adc_temp, int32_t *fine_temp);
static inline uint32_t compensate_pressure(bmp280_t *dev, int32_t adc_press, int32_t fine_temp);
static inline uint32_t compensate_humidity(bmp280_t *dev, int32_t adc_hum, int32_t fine_temp);
/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
// static const char *TAG = "bmp280";

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
esp_err_t bmp280_init_default_params(bmp280_params_t *params)
{
    CHECK_ARG(params);

    params->mode = BMP280_MODE_NORMAL;
    params->filter = BMP280_FILTER_OFF;
    params->oversampling_pressure = BMP280_STANDARD;
    params->oversampling_temperature = BMP280_STANDARD;
    params->oversampling_humidity = BMP280_STANDARD;
    params->standby = BMP280_STANDBY_250;

    return ESP_OK;
}

esp_err_t bmp280_init(bmp280_t *dev, bmp280_params_t *params, s_ezlopi_i2c_master_t *i2c_master_conf)
{
    CHECK_ARG(dev && params && i2c_master_conf);

    uint8_t write_buffer = BMP280_REG_ID;

    ESP_ERROR_CHECK(EZPI_hal_i2c_master_write_to_device(i2c_master_conf, &write_buffer, 1));
    ESP_ERROR_CHECK(EZPI_hal_i2c_master_read_from_device(i2c_master_conf, &dev->id, 1));

    if (dev->id != BMP280_CHIP_ID && dev->id != BME280_CHIP_ID)
    {
        CHECK_LOGE(dev, ESP_ERR_INVALID_VERSION, "Invalid chip ID: expected: 0x%x (BME280) or 0x%x (BMP280) got: 0x%x",
                   BME280_CHIP_ID, BMP280_CHIP_ID, dev->id);
    }

    // Soft reset.
    uint8_t reset_data[2] = {BMP280_REG_RESET, BMP280_RESET_VALUE};
    ESP_ERROR_CHECK(EZPI_hal_i2c_master_write_to_device(i2c_master_conf, reset_data, 2));

    // Wait until finished copying over the NVP data.
    uint8_t status;
    write_buffer = BMP280_REG_STATUS;
    while (1)
    {
        ESP_ERROR_CHECK(EZPI_hal_i2c_master_write_to_device(i2c_master_conf, &write_buffer, 1));
        ESP_ERROR_CHECK(EZPI_hal_i2c_master_read_from_device(i2c_master_conf, &status, 1));
        if ((status & 1) == 0)
        {
            break;
        }
    }

    CHECK_LOGE(dev, read_calibration_data(i2c_master_conf, dev), "Failed to read calibration data");

    if (dev->id == BME280_CHIP_ID)
    {
        CHECK_LOGE(dev, read_hum_calibration_data(i2c_master_conf, dev), "Failed to read humidity calibration data");
    }

    uint8_t config = (params->standby << 5) | (params->filter << 2);
    ESP_LOGD("bmp280", "Writing config reg=%x", config);

    uint8_t config_data[2] = {BMP280_REG_CONFIG, config};
    ESP_ERROR_CHECK(EZPI_hal_i2c_master_write_to_device(i2c_master_conf, config_data, 2));

    if (params->mode == BMP280_MODE_FORCED)
    {
        params->mode = BMP280_MODE_SLEEP; // initial mode for forced is sleep
    }

    if (dev->id == BME280_CHIP_ID)
    {
        // Write crtl hum reg first, only active after write to BMP280_REG_CTRL.
        uint8_t ctrl_hum = params->oversampling_humidity;
        ESP_LOGD("bmp280", "Writing ctrl hum reg=%x", ctrl_hum);
        uint8_t humid_crtl_data[2] = {BMP280_REG_CTRL_HUM, ctrl_hum};
        ESP_ERROR_CHECK(EZPI_hal_i2c_master_write_to_device(i2c_master_conf, humid_crtl_data, 2));
    }

    uint8_t ctrl = (params->oversampling_temperature << 5) | (params->oversampling_pressure << 2) | (params->mode);
    ESP_LOGD("bmp280", "Writing ctrl reg=%x", ctrl);
    uint8_t crtl_data[2] = {BMP280_REG_CTRL, ctrl};
    ESP_ERROR_CHECK(EZPI_hal_i2c_master_write_to_device(i2c_master_conf, crtl_data, 2));

    return ESP_OK;
}

esp_err_t bmp280_read_fixed(s_ezlopi_i2c_master_t *i2c_master_conf, bmp280_t *dev, int32_t *temperature, uint32_t *pressure, uint32_t *humidity)
{
    CHECK_ARG(i2c_master_conf && dev && temperature && pressure && humidity);

    int32_t adc_pressure;
    int32_t adc_temp;
    uint8_t data[8];

    // Only the BME280 supports reading the humidity.
    if (dev->id != BME280_CHIP_ID)
    {
        if (humidity)
            *humidity = 0;
        humidity = NULL;
    }

    // Need to read in one sequence to ensure they match.
    size_t size = humidity ? 8 : 6;
    uint8_t register_addr = 0xf7;
    ESP_ERROR_CHECK(EZPI_hal_i2c_master_write_to_device(i2c_master_conf, &register_addr, 1));
    ESP_ERROR_CHECK(EZPI_hal_i2c_master_read_from_device(i2c_master_conf, data, size));

    adc_pressure = data[0] << 12 | data[1] << 4 | data[2] >> 4;
    adc_temp = data[3] << 12 | data[4] << 4 | data[5] >> 4;
    ESP_LOGD("bmp280", "ADC temperature: %" PRIi32, adc_temp);
    ESP_LOGD("bmp280", "ADC pressure: %" PRIi32, adc_pressure);

    int32_t fine_temp;
    *temperature = compensate_temperature(dev, adc_temp, &fine_temp);
    *pressure = compensate_pressure(dev, adc_pressure, fine_temp);

    if (humidity)
    {
        int32_t adc_humidity = data[6] << 8 | data[7];
        ESP_LOGD("bmp280", "ADC humidity: %" PRIi32, adc_humidity);
        *humidity = compensate_humidity(dev, adc_humidity, fine_temp);
    }

    return ESP_OK;
}

esp_err_t bmp280_read_float(s_ezlopi_i2c_master_t *i2c_master_conf, bmp280_t *dev, float *temperature, float *pressure, float *humidity)
{
    int32_t fixed_temperature;
    uint32_t fixed_pressure;
    uint32_t fixed_humidity;
    CHECK(bmp280_read_fixed(i2c_master_conf, dev, &fixed_temperature, &fixed_pressure, humidity ? &fixed_humidity : NULL));
    *temperature = (float)fixed_temperature / 100;
    *pressure = (float)fixed_pressure / 256;
    if (humidity)
        *humidity = (float)fixed_humidity / 1024;

    return ESP_OK;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/
inline static esp_err_t read_register_8(s_ezlopi_i2c_master_t *i2c_master_conf, uint8_t reg, uint8_t *r)
{
    ESP_ERROR_CHECK(EZPI_hal_i2c_master_write_to_device(i2c_master_conf, &reg, 1));
    ESP_ERROR_CHECK(EZPI_hal_i2c_master_read_from_device(i2c_master_conf, r, 1));
    return 0;
}

static esp_err_t read_register16(s_ezlopi_i2c_master_t *i2c_master_conf, uint8_t reg, uint16_t *r)
{
    uint8_t d[] = {0, 0};

    ESP_ERROR_CHECK(EZPI_hal_i2c_master_write_to_device(i2c_master_conf, &reg, 1));
    ESP_ERROR_CHECK(EZPI_hal_i2c_master_read_from_device(i2c_master_conf, d, 2));
    *r = d[0] | (d[1] << 8);

    return ESP_OK;
}

static esp_err_t read_calibration_data(s_ezlopi_i2c_master_t *i2c_master_conf, bmp280_t *dev)
{
    CHECK(read_register16(i2c_master_conf, 0x88, &dev->dig_T1));
    CHECK(read_register16(i2c_master_conf, 0x8a, (uint16_t *)&dev->dig_T2));
    CHECK(read_register16(i2c_master_conf, 0x8c, (uint16_t *)&dev->dig_T3));
    CHECK(read_register16(i2c_master_conf, 0x8e, &dev->dig_P1));
    CHECK(read_register16(i2c_master_conf, 0x90, (uint16_t *)&dev->dig_P2));
    CHECK(read_register16(i2c_master_conf, 0x92, (uint16_t *)&dev->dig_P3));
    CHECK(read_register16(i2c_master_conf, 0x94, (uint16_t *)&dev->dig_P4));
    CHECK(read_register16(i2c_master_conf, 0x96, (uint16_t *)&dev->dig_P5));
    CHECK(read_register16(i2c_master_conf, 0x98, (uint16_t *)&dev->dig_P6));
    CHECK(read_register16(i2c_master_conf, 0x9a, (uint16_t *)&dev->dig_P7));
    CHECK(read_register16(i2c_master_conf, 0x9c, (uint16_t *)&dev->dig_P8));
    CHECK(read_register16(i2c_master_conf, 0x9e, (uint16_t *)&dev->dig_P9));

    ESP_LOGD("bmp280", "Calibration data received:");
    ESP_LOGD("bmp280", "dig_T1=%d", dev->dig_T1);
    ESP_LOGD("bmp280", "dig_T2=%d", dev->dig_T2);
    ESP_LOGD("bmp280", "dig_T3=%d", dev->dig_T3);
    ESP_LOGD("bmp280", "dig_P1=%d", dev->dig_P1);
    ESP_LOGD("bmp280", "dig_P2=%d", dev->dig_P2);
    ESP_LOGD("bmp280", "dig_P3=%d", dev->dig_P3);
    ESP_LOGD("bmp280", "dig_P4=%d", dev->dig_P4);
    ESP_LOGD("bmp280", "dig_P5=%d", dev->dig_P5);
    ESP_LOGD("bmp280", "dig_P6=%d", dev->dig_P6);
    ESP_LOGD("bmp280", "dig_P7=%d", dev->dig_P7);
    ESP_LOGD("bmp280", "dig_P8=%d", dev->dig_P8);
    ESP_LOGD("bmp280", "dig_P9=%d", dev->dig_P9);

    return ESP_OK;
}

static esp_err_t read_hum_calibration_data(s_ezlopi_i2c_master_t *i2c_master_conf, bmp280_t *dev)
{
    uint16_t h4, h5;

    uint8_t register_addr = 0xa1;
    ESP_ERROR_CHECK(read_register_8(i2c_master_conf, register_addr, &dev->dig_H1));
    CHECK(read_register16(i2c_master_conf, 0xe1, (uint16_t *)&dev->dig_H2));

    register_addr = 0xe3;
    ESP_ERROR_CHECK(read_register_8(i2c_master_conf, register_addr, &dev->dig_H3));
    CHECK(read_register16(i2c_master_conf, 0xe4, &h4));
    CHECK(read_register16(i2c_master_conf, 0xe5, &h5));

    register_addr = 0xe7;
    ESP_ERROR_CHECK(read_register_8(i2c_master_conf, register_addr, (uint8_t *)&dev->dig_H6));

    dev->dig_H4 = (h4 & 0x00ff) << 4 | (h4 & 0x0f00) >> 8;
    dev->dig_H5 = h5 >> 4;
    ESP_LOGD("bmp280", "Calibration data received:");
    ESP_LOGD("bmp280", "dig_H1=%d", dev->dig_H1);
    ESP_LOGD("bmp280", "dig_H2=%d", dev->dig_H2);
    ESP_LOGD("bmp280", "dig_H3=%d", dev->dig_H3);
    ESP_LOGD("bmp280", "dig_H4=%d", dev->dig_H4);
    ESP_LOGD("bmp280", "dig_H5=%d", dev->dig_H5);
    ESP_LOGD("bmp280", "dig_H6=%d", dev->dig_H6);

    return ESP_OK;
}

/**
 * Compensation algorithm is taken from BMP280 datasheet.
 *
 * Return value is in degrees Celsius.
 */
static inline int32_t compensate_temperature(bmp280_t *dev, int32_t adc_temp, int32_t *fine_temp)
{
    int32_t var1, var2;

    var1 = ((((adc_temp >> 3) - ((int32_t)dev->dig_T1 << 1))) * (int32_t)dev->dig_T2) >> 11;
    var2 = (((((adc_temp >> 4) - (int32_t)dev->dig_T1) * ((adc_temp >> 4) - (int32_t)dev->dig_T1)) >> 12) * (int32_t)dev->dig_T3) >> 14;

    *fine_temp = var1 + var2;
    return (*fine_temp * 5 + 128) >> 8;
}

/**
 * Compensation algorithm is taken from BMP280 datasheet.
 *
 * Return value is in Pa, 24 integer bits and 8 fractional bits.
 */
static inline uint32_t compensate_pressure(bmp280_t *dev, int32_t adc_press, int32_t fine_temp)
{
    int64_t var1, var2, p;

    var1 = (int64_t)fine_temp - 128000;
    var2 = var1 * var1 * (int64_t)dev->dig_P6;
    var2 = var2 + ((var1 * (int64_t)dev->dig_P5) << 17);
    var2 = var2 + (((int64_t)dev->dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dev->dig_P3) >> 8) + ((var1 * (int64_t)dev->dig_P2) << 12);
    var1 = (((int64_t)1 << 47) + var1) * ((int64_t)dev->dig_P1) >> 33;

    if (var1 == 0)
    {
        return 0; // avoid exception caused by division by zero
    }

    p = 1048576 - adc_press;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = ((int64_t)dev->dig_P9 * (p >> 13) * (p >> 13)) >> 25;
    var2 = ((int64_t)dev->dig_P8 * p) >> 19;

    p = ((p + var1 + var2) >> 8) + ((int64_t)dev->dig_P7 << 4);
    return p;
}

/**
 * Compensation algorithm is taken from BME280 datasheet.
 *
 * Return value is in Pa, 24 integer bits and 8 fractional bits.
 */
static inline uint32_t compensate_humidity(bmp280_t *dev, int32_t adc_hum, int32_t fine_temp)
{
    int32_t v_x1_u32r;

    v_x1_u32r = fine_temp - (int32_t)76800;
    v_x1_u32r = ((((adc_hum << 14) - ((int32_t)dev->dig_H4 << 20) - ((int32_t)dev->dig_H5 * v_x1_u32r)) + (int32_t)16384) >> 15) * (((((((v_x1_u32r * (int32_t)dev->dig_H6) >> 10) * (((v_x1_u32r * (int32_t)dev->dig_H3) >> 11) + (int32_t)32768)) >> 10) + (int32_t)2097152) * (int32_t)dev->dig_H2 + 8192) >> 14);
    v_x1_u32r = v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * (int32_t)dev->dig_H1) >> 4);
    v_x1_u32r = v_x1_u32r < 0 ? 0 : v_x1_u32r;
    v_x1_u32r = v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r;
    return v_x1_u32r >> 12;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
