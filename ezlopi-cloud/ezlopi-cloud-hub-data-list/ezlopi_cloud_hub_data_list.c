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

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "cjext.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_setting_commands.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_hub_data_list.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static void ezlopi_core_hub_data_list_populate_settings_json(cJSON *cj_result_name, const char* field_str, const char *name_str);
static void ezlopi_core_hub_data_list_process_settings_data_list(cJSON *cj_names, cJSON *cj_include, cJSON *cj_settings);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/**
 * @brief Global/extern function template example
 * Convention : Use capital letter for initial word on extern function
 * @param arg
 */
void hub_data_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_setting_data_list = cJSON_GetObjectItem(__FUNCTION__, cj_params, "settings");
            if (cj_setting_data_list)
            {
                cJSON *cj_settings = cJSON_AddObjectToObject(__FUNCTION__, cj_result, "settings");
                cJSON *cj_names_array = cJSON_GetObjectItem(__FUNCTION__, cj_setting_data_list, "names");
                if (cj_names_array && cJSON_IsArray(cj_names_array) && cj_settings)
                {
                    cJSON *cj_fileds = cJSON_GetObjectItem(__FUNCTION__, cj_setting_data_list, "fields");
                    if (cj_fileds)
                    {
                        cJSON *cj_include = cJSON_GetObjectItem(__FUNCTION__, cj_fileds, "include");
                        if (cj_include && cJSON_IsArray(cj_include))
                        {
                            ezlopi_core_hub_data_list_process_settings_data_list(cj_names_array, cj_include, cj_settings);
                        }
                    }
                }
            }
        }
    }
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/
static void ezlopi_core_hub_data_list_populate_settings_json(cJSON *cj_result_name, const char* field_str, const char *name_str)
{
    e_ezlopi_core_setting_command_names_t settings_name_enum = ezlopi_core_setting_command_get_command_enum_from_str(name_str);
    switch (settings_name_enum)
    {
    case SETTING_COMMAND_NAME_SCALE_TEMPERATURE:
    {
        char *scale_str = (ezlopi_core_setting_get_temperature_scale() == TEMPERATURE_SCALE_FAHRENHEIT) ? "fahrenheit" : "celsius";
        cJSON_AddStringToObject(__FUNCTION__, cj_result_name, field_str, scale_str);
        break;
    }
    case SETTING_COMMAND_NAME_DATE_FORMAT:
    {
        char *date_format = (ezlopi_core_setting_get_date_format() == DATE_FORMAT_MMDDYY) ? "mmddyy" : "ddmmyy";
        cJSON_AddStringToObject(__FUNCTION__, cj_result_name, field_str, date_format);
        break;
    }
    case SETTING_COMMAND_NAME_TIME_FORMAT:
    {
        char *time_format = (ezlopi_core_setting_get_time_format() == TIME_FORMAT_12) ? "12" : "24";
        cJSON_AddStringToObject(__FUNCTION__, cj_result_name, field_str, time_format);
        break;
    }
    default:
    {
        break;
    }
    }
}

static void ezlopi_core_hub_data_list_process_settings_data_list(cJSON *cj_names, cJSON *cj_include, cJSON *cj_settings)
{
    cJSON *cj_include_element = NULL;
    cJSON *cj_names_element = NULL;
    cJSON_ArrayForEach(cj_names_element, cj_names)
    {
        cJSON *cj_result_names = cJSON_AddObjectToObject(__FUNCTION__, cj_settings, cj_names_element->valuestring);
        if (cj_result_names)
        {
            cJSON_ArrayForEach(cj_include_element, cj_include)
            {
                if (0 == strncmp(cj_include_element->valuestring, "value", 6))
                {
                    ezlopi_core_hub_data_list_populate_settings_json(cj_result_names, cj_include_element->valuestring, cj_names_element->valuestring);
                }
            }
        }
    }
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
