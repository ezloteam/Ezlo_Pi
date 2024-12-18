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
 * @file    ezlopi_core_setting_commands.h
 * @brief   perform some function on setting cmds
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
*/

#ifndef __EZLOPI_CORE_SETTING_COMMANDS_H__
#define __EZLOPI_CORE_SETTING_COMMANDS_H__
/*******************************************************************************
*                          Include Files
*******************************************************************************/

#include "cjext.h"

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
    typedef enum
    {
        SETTING_COMMAND_NAME_SCALE_TEMPERATURE = 0,
        SETTING_COMMAND_NAME_DATE_FORMAT,
        SETTING_COMMAND_NAME_TIME_FORMAT,
        SETTING_COMMAND_NAME_NETWORK_PING_TIMEOUT,
        SETTING_COMMAND_NAME_LOG_LEVEL,
        SETTING_COMMAND_NAME_MAX,
    } e_ezlopi_core_setting_command_names_t;

    typedef enum
    {
        TEMPERATURE_SCALE_FAHRENHEIT = 0,
        TEMPERATURE_SCALE_CELSIUS,
        TEMPERATURE_SCALE_MAX,
    } e_enum_temperature_scale_t;

    typedef enum
    {
        DATE_FORMAT_MMDDYY = 0,
        DATE_FORMAT_DDMMYY,
        DATE_FORMAT_MAX,
    } e_enum_date_format_t;

    typedef enum
    {
        TIME_FORMAT_12 = 0,
        TIME_FORMAT_24,
        TIME_FORMAT_MAX,
    } e_enum_time_format_t;

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

    int EZPI_core_setting_commands_process(cJSON *cj_params);
    e_ezlopi_core_setting_command_names_t ezlopi_core_setting_command_get_command_enum_from_str(const char *name);
    int EZPI_core_setting_commands_populate_settings(cJSON *cj_result);
    int EZPI_core_setting_commands_read_settings();
    e_enum_temperature_scale_t EZPI_core_setting_get_temperature_scale();
    e_enum_date_format_t EZPI_core_setting_get_date_format();
    e_enum_time_format_t EZPI_core_setting_get_time_format();
    int EZPI_core_setting_get_network_ping_timeout();
    int EZPI_core_setting_updated_broadcast(cJSON *cj_params, cJSON *cj_result);

    const char *EZPI_core_setting_get_temperature_scale_str();
    const char *EZPI_core_setting_get_date_format_str();
    const char *EZPI_core_setting_get_time_format_str();


#ifdef __cplusplus
}
#endif

#endif /// __EZLOPI_CORE_SETTING_COMMANDS_H__

/*******************************************************************************
*                          End of File
*******************************************************************************/