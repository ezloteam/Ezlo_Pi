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
 * @file    dht22.h
 * @brief   perform some function on dht22
 * @author  xx
 * @version 0.1
 * @date    xx
 */

#ifndef __DHT22_H__
#define __DHT22_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

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

#if 0
#include "stdint.h"

#define DHT22_TAG __FILE__

#define DHT22_STANDARD_DELAY_TIME 3000 // 2sec

#define DHT22_START_SIGNAL_LINE_PULLDOWN_HOLD_US 20000 // 18ms
#define DHT22_START_SIGHAL_LINE_HOLD_RELEASE_US 40     // 40us

#define DHT22_RECEIVE_BIT_LOW_TIME_US 54 // 50us
#define DHT22_RECEIVE_BIT_1_HIGH_TIME_US 26
#define DHT22_RECEIVE_BIT_0_HIGH_TIME_US 26

#define DHT22_RECEIVE_RESPONSE_HIGH_TIME_US 80 // 80us
#define DHT22_RECEIVE_RESPONSE_LOW_TIME_US 80  // 80us

#define onewireENTER_CRITICAL_REGION()               \
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED; \
    taskENTER_CRITICAL(&mux)
#define onewireEXIT_CRITICAL_REGION() taskEXIT_CRITICAL(&mux)

    typedef struct dht22_sensor {
        double temperature;
        double humidity;
        uint32_t gpio_pin;
    }dht22_sensor_t;

    void dht22_sensor_init(uint32_t gpio_pin);
    int dht22_sensor_read_data(uint32_t gpio_pin, dht22_sensor_t *data);
#endif

#if (CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)

#define DHT22_OK 0
#define DHT22_CHECKSUM_ERROR -1
#define DHT22_TIMEOUT_ERROR -2
#define DHT22_INVALID_REQ -3

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Function to read dht22 data
     * 
     * @param temperature_dht22 
     * @param humidity_dht22 
     * @param DHT22gpio 
     * @return int 
     */
    int readDHT22(float *temperature_dht22, float *humidity_dht22, int DHT22gpio);
    /**
     * @brief Get current dht22 signal level
     * 
     * @param DHT22gpio 
     * @param usTimeOut 
     * @param state 
     * @return int 
     */
    int dht22_getSignalLevel(int DHT22gpio, int usTimeOut, bool state);
    // void errorHandler(int response);

#endif // (CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)

#ifdef __cplusplus
}
#endif

#endif //__DHT22_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/