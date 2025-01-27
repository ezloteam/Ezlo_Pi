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
 * @file    dht11.c
 * @brief   perform some function on dht11
 * @author
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

/*
 *  Note:
 *  A suitable pull-up resistor should be connected to the selected GPIO line
 *
 *  __           ______          _______                              ___________________________
 *    \    A    /      \   C    /       \   DHT duration_data_low    /                           \
 *     \_______/   B    \______/    D    \__________________________/   DHT duration_data_high    \__
 *
 *
 *  Initializing communications with the DHT requires four 'phases' as follows:
 *
 *  Phase A - MCU pulls signal low for at least 18000 us
 *  Phase B - MCU allows signal to float back up and waits 20-40us for DHT to pull it low
 *  Phase C - DHT pulls signal low for ~80us
 *  Phase D - DHT lets signal float back up for ~80us
 *
 *  After this, the DHT transmits its first bit by holding the signal low for 50us
 *  and then letting it float back high for a period of time that depends on the data bit.
 *  duration_data_high is shorter than 50us for a logic '0' and longer than 50us for logic '1'.
 *
 *  There are a total of 40 data bits transmitted sequentially. These bits are read into a byte array
 *  of length 5.  The first and third bytes are humidity_dht11 (%) and temperature_dht11 (C), respectively.  Bytes 2 and 4
 *  are zero-filled and the fifth is a checksum such that:
 *
 *  byte_5 == (byte_1 + byte_2 + byte_3 + byte_4) & 0xFF
 *
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"
#if (CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)

// #include "esp_log.h"
#include "driver/gpio.h"

#if CONFIG_IDF_TARGET_ESP32S3
#include "esp32s3/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32
#include "esp32/rom/ets_sys.h"
#elif CONFIG_IDF_TARGET_ESP32S2
#include "esp32s2/rom/ets_sys.h"
#endif

#include "dht11.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
// == Read DHT ===============================================
#define MAXdht11Data 5 // to complete 40 = 5*8 Bits

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/
int dht11_getSignalLevel(int DHT11gpio, int usTimeOut, bool state)
{
    int uSec = 0;
    while (gpio_get_level(DHT11gpio) == state)
    {

        if (uSec > usTimeOut)
            return -1;

        ++uSec;
        ets_delay_us(1); // uSec delay
    }

    return uSec;
}

int readDHT11(float *temperature_dht11, float *humidity_dht11, int DHT11gpio)
{
    if ((NULL == temperature_dht11) || (NULL == humidity_dht11))
    {
        return DHT11_INVALID_REQ;
    }

    int uSec = 0;

    uint8_t dhtData[MAXdht11Data];
    uint8_t byteInx = 0;
    uint8_t bitInx = 7;

    for (int k = 0; k < MAXdht11Data; k++)
        dhtData[k] = 0;

    // == Send start signal to DHT sensor ===========

    gpio_set_direction(DHT11gpio, GPIO_MODE_OUTPUT);

    // pull down for 20 ms for a smooth and nice wake up
    gpio_set_level(DHT11gpio, 0);
    ets_delay_us(20000);

    // pull up for 25 us for a gentile asking for data
    gpio_set_level(DHT11gpio, 1);
    ets_delay_us(25);

    gpio_set_direction(DHT11gpio, GPIO_MODE_INPUT); // change to input mode

    // == DHT will keep the line low for 80 us and then high for 80us ====

    uSec = dht11_getSignalLevel(DHT11gpio, 85, 0);
    // ESP_LOGI(TAG, "Response = %d", uSec);
    if (uSec < 0)
        return DHT11_TIMEOUT_ERROR;

    // -- 80us up ------------------------

    uSec = dht11_getSignalLevel(DHT11gpio, 85, 1);
    // ESP_LOGI(TAG, "Response = %d", uSec);
    if (uSec < 0)
        return DHT11_TIMEOUT_ERROR;

    // == No errors, read the 40 data bits ================

    for (int k = 0; k < 40; k++)
    {

        // -- starts new data transmission with >50us low signal

        uSec = dht11_getSignalLevel(DHT11gpio, 56, 0);
        if (uSec < 0)
            return DHT11_TIMEOUT_ERROR;

        // -- check to see if after >70us rx data is a 0 or a 1

        uSec = dht11_getSignalLevel(DHT11gpio, 75, 1);
        if (uSec < 0)
            return DHT11_TIMEOUT_ERROR;

        // add the current read to the output data
        // since all dhtData array where set to 0 at the start,
        // only look for "1" (>28us us)

        if (uSec > 40)
        {
            dhtData[byteInx] |= (1 << bitInx);
        }

        // index to next byte

        if (bitInx == 0)
        {
            bitInx = 7;
            ++byteInx;
        }
        else
            bitInx--;
    }

    // == get humidity_dht11 from Data[0] and Data[1] ==========================

    *humidity_dht11 = dhtData[0] + (dhtData[1] * 0.1);

    // == get temp from Data[2] and Data[3]

    *temperature_dht11 = dhtData[2];
    if (dhtData[3] & 0x80) // negative temp, brrr it's freezing
    {
        *temperature_dht11 *= -1;
    }
    *temperature_dht11 += ((dhtData[3] & 0x0F) * 0.1);

    // == verify if checksum is ok ===========================================
    // Checksum is the sum of Data 8 bits masked out 0xFF

    if (dhtData[4] == ((dhtData[0] + dhtData[1] + dhtData[2] + dhtData[3]) & 0xFF))
        return DHT11_OK;

    else
        return DHT11_CHECKSUM_ERROR;
}

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
// == set the DHT used pin=========================================
// void setDHT11gpio(int gpio)
// {
//     DHT11gpio = gpio;
// }
// == get temp & hum =============================================
// float getHumidity_dht11() { return humidity_dht11; }
// float getTemperature_dht11() { return temperature_dht11; }

// == error handler ===============================================
// void errorHandler(int response)
// {
//     switch (response)
//     {
//     case DHT11_TIMEOUT_ERROR:
//         ESP_LOGE(TAG, "Sensor Timeout\n");
//         break;
//     case DHT11_CHECKSUM_ERROR:
//         ESP_LOGE(TAG, "CheckSum error\n");
//         break;
//     case DHT11_OK:
//         break;
//     default:
//         ESP_LOGE(TAG, "Unknown error\n");
//     }
// }
/*-------------------------------------------------------------------------------
;
;	get next state
;
;	I don't like this logic. It needs some interrupt blocking / priority
;	to ensure it runs in realtime.
;
;--------------------------------------------------------------------------------*/

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

#endif // CONFIG_IDF_TARGET_ESP32C3
       /*******************************************************************************
        *                          End of File
        *******************************************************************************/