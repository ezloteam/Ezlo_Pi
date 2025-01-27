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
 * @file    ezlopi_service_led_indicator.h
 * @brief   This file contains function definitons for LED indicator for device on, Wifi connected, internet available and connected to cloud
 * @author  
 * @version 1.0
 * @date    April 3, 2024
 */

#ifndef _EZLOPI_SERVICE_LED_INDICATOR_H___
#define _EZLOPI_SERVICE_LED_INDICATOR_H___

#ifdef CONFIG_EZPI_ENABLE_LED_INDICATOR

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

    /**
     * @brief Enum to set the LED indocator
     *
     */
    typedef enum
    {
        PRIORITY_POWER = 0, /**< Priority set at device powered */
        PRIORITY_WIFI,      /**< Priority set at device is connected to WiFi */
        PRIORITY_INTERNET,  /**< Priority set at device is connected to internet */
        PRIORITY_CLOUD,     /**< Priority set at device is connected to cloud */
        PRIORITY_MAX,       /**< Invalid priority state */
    } e_indicator_led_priority_t;

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Function to initialize LED indicator
     *
     * @return int
     * @retval 1 on success or 0 on error
     */
    int EZPI_service_led_indicator_init();
#ifdef __cplusplus
}
#endif

#endif // CONFIG_EZPI_ENABLE_LED_INDICATOR

#endif // __EZLOPI_SERVICE_LED_INDICATOR_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
