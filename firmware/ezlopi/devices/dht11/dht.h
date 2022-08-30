/*
 * Copyright (c) 2016 Jonathan Hartsuiker <https://github.com/jsuiker>
 * Copyright (c) 2018 Ruslan V. Uss <unclerus@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of itscontributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file dht.h
 * @defgroup dht dht
 * @{
 *
 * ESP-IDF driver for DHT11, AM2301 (DHT21, DHT22, AM2302, AM2321), Itead Si7021
 *
 * Ported from esp-open-rtos
 *
 * Copyright (c) 2016 Jonathan Hartsuiker <https://github.com/jsuiker>\n
 * Copyright (c) 2018 Ruslan V. Uss <unclerus@gmail.com>\n
 *
 * BSD Licensed as described in the file LICENSE
 *
 * @note A suitable pull-up resistor should be connected to the selected GPIO line
 *
 */
#ifndef __DHT_H__
#define __DHT_H__
#include <driver/gpio.h>
#include <esp_err.h>
#include <esp_idf_version.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if !defined(ESP_IDF_VERSION) || !defined(ESP_IDF_VERSION_VAL)
#error Unknown ESP-IDF/ESP8266 RTOS SDK version
#endif

/* Minimal supported version for ESP32, ESP32S2 */
#define HELPER_ESP32_MIN_VER ESP_IDF_VERSION_VAL(3, 3, 5)
/* Minimal supported version for ESP8266 */
#define HELPER_ESP8266_MIN_VER ESP_IDF_VERSION_VAL(3, 3, 0)

/* HELPER_TARGET_IS_ESP32
 * 1 when the target is esp32
 */
#if defined(CONFIG_IDF_TARGET_ESP32) || defined(CONFIG_IDF_TARGET_ESP32S2) || defined(CONFIG_IDF_TARGET_ESP32C3) || defined(CONFIG_IDF_TARGET_ESP32S3)
#define HELPER_TARGET_IS_ESP32 (1)
#define HELPER_TARGET_IS_ESP8266 (0)

/* HELPER_TARGET_IS_ESP8266
 * 1 when the target is esp8266
 */
#elif defined(CONFIG_IDF_TARGET_ESP8266)
#define HELPER_TARGET_IS_ESP32 (0)
#define HELPER_TARGET_IS_ESP8266 (1)
#else
#error BUG: cannot determine the target
#endif

        /**
         * Sensor type
         */
        typedef enum
        {
                DHT_TYPE_DHT11 = 0, //!< DHT11
                DHT_TYPE_AM2301,    //!< AM2301 (DHT21, DHT22, AM2302, AM2321)
                DHT_TYPE_SI7021     //!< Itead Si7021
        } dht_sensor_type_t;

        /**
         * @brief Read integer data from sensor on specified pin
         *
         * Humidity and temperature are returned as integers.
         * For example: humidity=625 is 62.5 %, temperature=244 is 24.4 degrees Celsius
         *
         * @param sensor_type DHT11 or DHT22
         * @param pin GPIO pin connected to sensor OUT
         * @param[out] humidity Humidity, percents * 10, nullable
         * @param[out] temperature Temperature, degrees Celsius * 10, nullable
         * @return `ESP_OK` on success
         */
        esp_err_t dht_read_data(dht_sensor_type_t sensor_type, gpio_num_t pin,
                                int16_t *humidity, int16_t *temperature);

        /**
         * @brief Read float data from sensor on specified pin
         *
         * Humidity and temperature are returned as floats.
         *
         * @param sensor_type DHT11 or DHT22
         * @param pin GPIO pin connected to sensor OUT
         * @param[out] humidity Humidity, percents, nullable
         * @param[out] temperature Temperature, degrees Celsius, nullable
         * @return `ESP_OK` on success
         */
        esp_err_t dht_read_float_data(dht_sensor_type_t sensor_type, gpio_num_t pin,
                                      float *humidity, float *temperature);

#ifdef __cplusplus
}
#endif

/**@}*/

#endif // __DHT_H__