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
 * @file    dht11.h
 * @brief   perform some function on dht11
 * @author  xx
 * @version 0.1
 * @date    xx
*/

#ifndef __DHT11_H__
#define __DHT11_H__

/*******************************************************************************
*                          Include Files
*******************************************************************************/
/**
* Include Sequence
* 1. C standard libraries eg. stdint, string etc.
* 2. Third party component eg. cJSON, freeRTOS etc etc
* 3. ESP-IDF specific eg. driver/gpio, esp_log etc.
* 4. EzloPi HAL specific eg ezlopi_hal_i2c
* 5. EzloPi core specific eg. ezlopi_core_ble
* 6. EzloPi cloud specific eg. ezlopi_cloud_items
* 7. EzloPi Service specific eg ezlopi_service_ble
* 8. EzloPi Sensors and Device Specific eg. ezlopi_device_0001_digitalout_generic
*/
#include "header.h"

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
    /*
        DHT11 temperature sensor driver
    */

#define DHT_OK 0
#define DHT_CHECKSUM_ERROR -1
#define DHT_TIMEOUT_ERROR -2

    /*******************************************************************************
    *                          Extern Data Declarations
    *******************************************************************************/

    /*******************************************************************************
    *                          Extern Function Prototypes
    *******************************************************************************/
    /**
     * @brief Function to set DHT11 gpio pin
     *
     * @param gpio
     */
    void setDHT11gpio(int gpio);
    /**
     * @brief Function to read DHT11 data
     *
     * @return int
     */
    int readDHT11(void);
    /**
     * @brief Get the Temperature dht11 object
     *
     * @return float
     */
    float getTemperature_dht11();
    /**
     * @brief Get the Humidity dht11 object
     *
     * @return float
     */
    float getHumidity_dht11();
    /**
     * @brief Function to get current signal level of dht11
     *
     * @param usTimeOut
     * @param state
     * @return int
     */
    int dht11_getSignalLevel(int usTimeOut, bool state);
    // void errorHandler(int response);

#ifdef __cplusplus
}
#endif

#endif //__DHT11_H__

/*******************************************************************************
*                          End of File
*******************************************************************************/