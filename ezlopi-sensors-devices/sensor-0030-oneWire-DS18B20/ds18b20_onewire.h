
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
 * @file    main.h
 * @brief   perform some function on data
 * @author  
 * @version 0.1
 * @date    xx
 */

#ifndef _DS18B20_ONEWIRE_H_
#define _DS18B20_ONEWIRE_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <stdbool.h>
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

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

    // Reference for onewire: https://www.analog.com/en/technical-articles/1wire-communication-through-software.html

#define ONEWIRE_TAG __FILE__
#define ONEWIRE_GET_LINE __LINE__

#define onewireENTER_CRITICAL_REGION()               \
    portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED; \
    taskENTER_CRITICAL(&mux)
#define onewireEXIT_CRITICAL_REGION() taskEXIT_CRITICAL(&mux)

    // One wire timing delays for different data values
#define ONE_WIRE_WRITE_1_LINE_PULL_DOWN_HOLD_US 6 // 6us
#define ONE_WIRE_WRITE_1_LINE_RELEASE_HOLD_US 64  // 64us

#define ONE_WIRE_WRITE_0_LINE_PULL_DOWN_HOLD_US 60 // 60us
#define ONE_WIRE_WRITE_0_LINE_RELEASE_HOLD_US 10   // 10us

#define ONE_WIRE_READ_LINE_PULL_DOWN_HOLD_US 6 // 6us
#define ONE_WIRE_READ_LINE_RELEASE_HOLD_US 9   // 9us
#define ONE_WIRE_READ_LINE_SAMPLING_US 55      // 55us

#define ONE_WIRE_HOLD_BEFORE_RESET_US 0           // 0us
#define ONE_WIRE_RESET_LINE_PULL_DOWN_HOLD_US 480 // 470us
#define ONE_WIRE_RESET_LINE_RELEASE_HOLD_US 70    // 70us
#define ONE_WIRE_RESET_LINE_SAMPLING_US 410       // 410us

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Function to write byte to line
     *
     * @param data Pointer to write-data
     * @param gpio_pin Target OneWire-pin
     * @return esp_err_t
     */
    esp_err_t DS18B20_write_byte_to_line(uint8_t *data, uint32_t gpio_pin);
    /**
     * @brief Function to read byte from line
     *
     * @param data Pointer to write-data
     * @param gpio_pin Target OneWire-pin
     * @return esp_err_t
     */
    esp_err_t DS18B20_read_byte_from_line(uint8_t *data, uint32_t gpio_pin);
    /**
     * @brief Function to reset one-wire line
     *
     * @param gpio_pin Target OneWire-pin
     * @return true
     * @return false
     */
    bool DS18B20_reset_line(uint32_t gpio_pin);

#ifdef __cplusplus
}
#endif

#endif // _DS18B20_ONEWIRE_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/