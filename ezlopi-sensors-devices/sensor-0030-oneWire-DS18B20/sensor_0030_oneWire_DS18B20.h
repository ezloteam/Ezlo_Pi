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
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

#ifndef SENSOR_0030_ONEWIRE_DS18B20_H
#define SENSOR_0030_ONEWIRE_DS18B20_H

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices.h"
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
    #define DS18B20_TEMPERATURE_LSB 0
    #define DS18B20_TEMPERATURE_HSB 1
    #define DS18B20_TH_REGISTER 2
    #define DS18B20_TL_REGISTER 3
    #define DS18B20_CONFIG_REGISTER 4
    #define DS18B20_CRC_REGISTER 8

    #define DS18B20_TEMPERATURE_9_BIT_RESOLUTION 0x1F
    #define DS18B20_TEMPERATURE_10_BIT_RESOLUTION 0x3F
    #define DS18B20_TEMPERATURE_11_BIT_RESOLUTION 0x5F
    #define DS18B20_TEMPERATURE_12_BIT_RESOLUTION 0x7F

    #define DS18B20_FAMILY_CODE 0x28

    // ROM commands for DS18B20
    #define DS18B20_ROM_COMMAND_SEARCH_ROM 0xF0
    #define DS18B20_ROM_COMMAND_READ_ROM 0x33
    #define DS18B20_ROM_COMMAND_MATCH_ROM 0x55
    #define DS18B20_ROM_COMMAND_SKIP_ROM 0xCC
    #define DS18B20_ROM_COMMAND_ALARM_SEARCH 0xEC

    #define DS18B20_FUNCTION_COMMAND_CONVERT_TEMP 0x44
    #define DS18B20_FUNCTION_COMMAND_WRITE_SCRATCHPAD 0x4E
    #define DS18B20_FUNCTION_COMMAND_READ_SCRATCHPAD 0xBE
    #define DS18B20_FUNCTION_COMMAND_COPY_SCRATCHPAD 0x48
    #define DS18B20_FUNCTION_COMMAND_RECALL_E2 0xB8
    #define DS18B20_FUNCTION_COMMAND_READ_POWER_SUPPLY 0xB4

    #define DS18B20_TH_HIGHER_THRESHOLD 42 // 42 degree celcius
    #define DS18B20_TL_LOWER_THRESHOLD -10 // -10 degree celcius

    #define pgm_read_byte(addr) (*(const unsigned char *)(addr))

    static const uint8_t dscrc2x16_table[] = {
        0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83,
        0xC2, 0x9C, 0x7E, 0x20, 0xA3, 0xFD, 0x1F, 0x41,
        0x00, 0x9D, 0x23, 0xBE, 0x46, 0xDB, 0x65, 0xF8,
        0x8C, 0x11, 0xAF, 0x32, 0xCA, 0x57, 0xE9, 0x74};

    #if 0 // v2.x
    int ds18b20_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_arg);
    #endif

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/
    /**
     * @brief Global function template example
     * Convention : Use capital letter for initial word on extern function
     * maincomponent : Main component as hal, core, service etc.
     * subcomponent : Sub component as i2c from hal, ble from service etc
     * functiontitle : Title of the function
     * eg : EZPI_hal_i2c_init()
     * @param arg
     *
     */
    ezlopi_error_t sensor_0030_oneWire_DS18B20(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg);

#ifdef __cplusplus
}
#endif

#endif // SENSOR_0030_ONEWIRE_DS18B20_H

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
