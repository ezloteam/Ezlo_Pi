
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
 * @file    sensor_0068_ENS160_gas_sensor_settings.h
 * @brief   perform some function on sensor_0068_ENS160
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    xx
 */

#ifndef _SENSOR_ENS160_GAS_SENSOR_SETTINGS_H_
#define _SENSOR_ENS160_GAS_SENSOR_SETTINGS_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

#include "cjext.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_core_devices.h"

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

#define SENSOR_ENS160_GAS_SENSOR_SETTING_AMBIENT_TEMPERATURE_LABEL_TEXT "Set ENS160 gas sensor ambient temperature"
#define SENSOR_ENS160_GAS_SENSOR_SETTING_AMBIENT_TEMPERATURE_DESCRIPTION_TEXT "Setter for ENS160 gas sensor ambient temperature value. Sensor can operate on different surroundings depending on the ambeint temperature"
#define SENSOR_ENS160_GAS_SENSOR_SETTING_RELATIVE_HUMIDITY_LABEL_TEXT "Set ENS160 gas sensor relative humidity"
#define SENSOR_ENS160_GAS_SENSOR_SETTING_RELATIVE_HUMIDITY_DESCRIPTION_TEXT "Setter for ENS160 gas sensor relative humidity value. Sensor can operate on different surroundings depending on the relative humidity"

#define SENSOR_ENS160_GAS_SENSOR_SETTING_AMBIENT_TEMPERATURE_LABEL_LANG_TAG "ens160_gas_sensor_ambient_temperature_setting_label"
#define SENSOR_ENS160_GAS_SENSOR_SETTING_AMBIENT_TEMPERATURE_DESCRIPTION_LANG_TAG "ens160_gas_sensor_ambient_temperature_setting_description"
#define SENSOR_ENS160_GAS_SENSOR_SETTING_RELATIVE_HUMIDITY_LABEL_LANG_TAG "ens160_gas_sensor_relative_humidity_setting_label"
#define SENSOR_ENS160_GAS_SENSOR_SETTING_RELATIVE_HUMIDITY_DESCRIPTION_LANG_TAG "ens160_gas_sensor_relative_humidity_setting_description"

    typedef struct s_sensor_ens160_gas_sensor_setting_ambient_temperature
    {
        float ambient_temperature;
        void *user_arg;
    } s_sensor_ens160_gas_sensor_setting_ambient_temperature_t;

    typedef struct s_sensor_ens160_gas_sensor_setting_relative_humidity
    {
        float relative_humidity;
        void *user_arg;
    } s_sensor_ens160_gas_sensor_setting_relative_humidity_t;
    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    /**
     * @brief Function to initialize setting for ens160
     *
     * @param devices
     * @param user_arg
     * @return int
     */
    int ENS160_gas_sensor_settings_init(l_ezlopi_device_t *devices, void *user_arg);
    /**
     * @brief Get the ambient temperature setting object
     *
     * @return float
     */
    float ENS160_get_ambient_temp_setting();
    /**
     * @brief Get the relative humidity setting object
     *
     * @return float
     */
    float ENS160_get_relative_humidity_setting();
    /**
     * @brief Function to check if settings changed
     *
     * @return true
     * @return false
     */
    bool ENS160_has_setting_changed();
    /**
     * @brief Function to change setting to false
     *
     */
    void ENS160_set_setting_changed_to_false()

#ifdef __cplusplus
}
#endif

#endif // _SENSOR_ENS160_GAS_SENSOR_SETTINGS_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/

;
