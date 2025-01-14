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
 * @file    ezlopi_hal_adc.h
 * @brief   perform some function on ADC
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    xx
 */

#ifndef _EZLOPI_ADC_H_
#define _EZLOPI_ADC_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "driver/adc.h"
#include "esp_adc_cal.h"

#include "ezlopi_core_errors.h"

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

#if CONFIG_IDF_TARGET_ESP32
    typedef enum e_ezlopi_gpio_channel
    {
        EZLOPI_GPIO_CHANNEL_0 = 36,
        EZLOPI_GPIO_CHANNEL_1 = 37,
        EZLOPI_GPIO_CHANNEL_2 = 38,
        EZLOPI_GPIO_CHANNEL_3 = 39,
        EZLOPI_GPIO_CHANNEL_4 = 32,
        EZLOPI_GPIO_CHANNEL_5 = 33,
        EZLOPI_GPIO_CHANNEL_6 = 34,
        EZLOPI_GPIO_CHANNEL_7 = 35,
        EZLOPI_GPIO_CHANNEL_MAX
    } e_ezlopi_gpio_channel_t;
#elif CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32S2
typedef enum e_ezlopi_gpio_channel
{
    EZLOPI_GPIO_CHANNEL_0 = 1,
    EZLOPI_GPIO_CHANNEL_1 = 2,
    EZLOPI_GPIO_CHANNEL_2 = 3,
    EZLOPI_GPIO_CHANNEL_3 = 4,
    EZLOPI_GPIO_CHANNEL_4 = 5,
    EZLOPI_GPIO_CHANNEL_5 = 6,
    EZLOPI_GPIO_CHANNEL_6 = 7,
    EZLOPI_GPIO_CHANNEL_7 = 8,
    EZLOPI_GPIO_CHANNEL_8 = 9,
    EZLOPI_GPIO_CHANNEL_9 = 10,
    EZLOPI_GPIO_CHANNEL_MAX
} e_ezlopi_gpio_channel_t;
#elif CONFIG_IDF_TARGET_ESP32C3
typedef enum e_ezlopi_gpio_channel
{
    EZLOPI_GPIO_CHANNEL_0 = 0,
    EZLOPI_GPIO_CHANNEL_1 = 1,
    EZLOPI_GPIO_CHANNEL_2 = 2,
    EZLOPI_GPIO_CHANNEL_3 = 3,
    EZLOPI_GPIO_CHANNEL_4 = 4,
    EZLOPI_GPIO_CHANNEL_MAX
} e_ezlopi_gpio_channel_t;
#endif

    typedef struct s_ezlopi_analog_data
    {
        uint32_t value;
        uint32_t voltage;
    } s_ezlopi_analog_data_t;

    typedef struct s_ezlopi_adc
    {
        int gpio_num;
        uint8_t resln_bit;
    } s_ezlopi_adc_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    /**
     * @brief Function to initialize adc pin
     *
     * @param gpio_num Target pin to setup as ADC
     * @param width ADC resolution width
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_hal_adc_init(uint8_t gpio_num, uint8_t width);
    /**
     * @brief Function to get adc data
     *
     * @param gpio_num GPIO-pin number
     * @param ezlopi_analog_data Pointer to structure containing req adc data
     * @return int
     */
    int EZPI_hal_adc_get_adc_data(uint8_t gpio_num, s_ezlopi_analog_data_t *ezlopi_analog_data);
    /**
     * @brief Function to get channel of corresponding PIN
     *
     * @param gpio_num Target GPIO pin
     * @return int
     */
    int EZPI_hal_adc_get_channel_number(uint8_t gpio_num);

#ifdef __cplusplus
}
#endif

#endif //_EZLOPI_ADC_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
