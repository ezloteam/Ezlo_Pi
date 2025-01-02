/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
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
 * @file    sensor_bme280.h
 * @brief   perform some function on bme280
 * @author  xx
 * @version 0.1
 * @date    xx
*/

#ifndef __BMP280_H__
#define __BMP280_H__

/*******************************************************************************
*                          Include Files
*******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <esp_err.h>
#include "ezlopi_hal_i2c_master.h"

/*******************************************************************************
*                          C++ Declaration Wrapper
*******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
    *                          Type & Macro Declarations
    *******************************************************************************/

#define BMP280_I2C_ADDRESS_0  0x76 //!< I2C address when SDO pin is low
#define BMP280_I2C_ADDRESS_1  0x77 //!< I2C address when SDO pin is high

#define BMP280_CHIP_ID  0x58 //!< BMP280 has chip-id 0x58
#define BME280_CHIP_ID  0x60 //!< BME280 has chip-id 0x60

    /**
     * Mode of BMP280 module operation.
     */
    typedef enum {
        BMP280_MODE_SLEEP = 0,  //!< Sleep mode
        BMP280_MODE_FORCED = 1, //!< Measurement is initiated by user
        BMP280_MODE_NORMAL = 3  //!< Continues measurement
    } BMP280_Mode;

    typedef enum {
        BMP280_FILTER_OFF = 0,
        BMP280_FILTER_2 = 1,
        BMP280_FILTER_4 = 2,
        BMP280_FILTER_8 = 3,
        BMP280_FILTER_16 = 4
    } BMP280_Filter;

    /**
     * Pressure oversampling settings
     */
    typedef enum {
        BMP280_SKIPPED = 0,          //!< no measurement
        BMP280_ULTRA_LOW_POWER = 1,  //!< oversampling x1
        BMP280_LOW_POWER = 2,        //!< oversampling x2
        BMP280_STANDARD = 3,         //!< oversampling x4
        BMP280_HIGH_RES = 4,         //!< oversampling x8
        BMP280_ULTRA_HIGH_RES = 5    //!< oversampling x16
    } BMP280_Oversampling;

    /**
     * Stand by time between measurements in normal mode
     */
    typedef enum {
        BMP280_STANDBY_05 = 0,      //!< stand by time 0.5ms
        BMP280_STANDBY_62 = 1,      //!< stand by time 62.5ms
        BMP280_STANDBY_125 = 2,     //!< stand by time 125ms
        BMP280_STANDBY_250 = 3,     //!< stand by time 250ms
        BMP280_STANDBY_500 = 4,     //!< stand by time 500ms
        BMP280_STANDBY_1000 = 5,    //!< stand by time 1s
        BMP280_STANDBY_2000 = 6,    //!< stand by time 2s BMP280, 10ms BME280
        BMP280_STANDBY_4000 = 7,    //!< stand by time 4s BMP280, 20ms BME280
    } BMP280_StandbyTime;

    /**
     * Configuration parameters for BMP280 module.
     * Use function ::bmp280_init_default_params() to use default configuration.
     */
    typedef struct {
        BMP280_Mode mode;
        BMP280_Filter filter;
        BMP280_Oversampling oversampling_pressure;
        BMP280_Oversampling oversampling_temperature;
        BMP280_Oversampling oversampling_humidity;
        BMP280_StandbyTime standby;
    } bmp280_params_t;

    /**
     * Device descriptor
     */
    typedef struct {
        uint16_t dig_T1;
        int16_t  dig_T2;
        int16_t  dig_T3;
        uint16_t dig_P1;
        int16_t  dig_P2;
        int16_t  dig_P3;
        int16_t  dig_P4;
        int16_t  dig_P5;
        int16_t  dig_P6;
        int16_t  dig_P7;
        int16_t  dig_P8;
        int16_t  dig_P9;

        /* Humidity compensation for BME280 */
        uint8_t  dig_H1;
        int16_t  dig_H2;
        uint8_t  dig_H3;
        int16_t  dig_H4;
        int16_t  dig_H5;
        int8_t   dig_H6;

        uint8_t   id;       //!< Chip ID
    } bmp280_t;



    /*******************************************************************************
    *                          Extern Data Declarations
    *******************************************************************************/

    /*******************************************************************************
    *                          Extern Function Prototypes
    *******************************************************************************/

    /**
     * @brief Initialize default parameters
     *
     * Default configuration:
     *
     *  - mode: NORMAL
     *  - filter: OFF
     *  - oversampling: x4
     *  - standby time: 250ms
     *
     * @param[out] params Default parameters
     * @return `ESP_OK` on success
     */
    esp_err_t bmp280_init_default_params(bmp280_params_t *params);

    /**
     * @brief Initialize BMP280 module
     *
     * Probes for the device, soft resets the device, reads the calibration
     * constants, and configures the device using the supplied parameters.
     *
     * This may be called again to soft reset the device and initialize it again.
     *
     * @param dev Device descriptor
     * @param params Parameters
     * @return `ESP_OK` on success
     */
    esp_err_t bmp280_init(bmp280_t *dev, bmp280_params_t *params, s_ezlopi_i2c_master_t *i2c_master_conf);


    /**
     * @brief Read raw compensated temperature and pressure data
     *
     * Temperature in degrees Celsius times 100.
     *
     * Pressure in Pascals in fixed point 24 bit integer 8 bit fraction format.
     *
     * Humidity is optional and only read for the BME280, in percent relative
     * humidity as a fixed point 22 bit integer and 10 bit fraction format.
     *
     * @param dev Device descriptor
     * @param[out] temperature Temperature, deg.C * 100
     * @param[out] pressure Pressure
     * @param[out] humidity Humidity, optional
     * @return `ESP_OK` on success
     */
    esp_err_t bmp280_read_fixed(s_ezlopi_i2c_master_t *i2c_master_conf, bmp280_t *dev, int32_t *temperature,
        uint32_t *pressure, uint32_t *humidity);

    /**
     * @brief Read compensated temperature and pressure data
     *
     * Humidity is optional and only read for the BME280.
     *
     * @param dev Device descriptor
     * @param[out] temperature Temperature, deg.C
     * @param[out] pressure Pressure, Pascal
     * @param[out] humidity Relative humidity, percents (optional)
     * @return `ESP_OK` on success
     */
    esp_err_t bmp280_read_float(s_ezlopi_i2c_master_t *i2c_master_conf, bmp280_t *dev, float *temperature,
        float *pressure, float *humidity);


#ifdef __cplusplus
}
#endif

#endif  // __BMP280_H__

/*******************************************************************************
*                          End of File
*******************************************************************************/

