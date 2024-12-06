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
#include <string.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_modes.h"
#include "ezlopi_core_modes_cjson.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_cloud_modes.h"
#include "ezlopi_cloud_constants.h"
#include "EZLOPI_USER_CONFIG.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)

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

void ezlopi_cloud_modes_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        ezlopi_core_modes_api_get_modes(cj_result);
    }
}

void ezlopi_cloud_modes_current_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    if (cj_result)
    {
        ezlopi_core_modes_api_get_current_mode(cj_result);
    }
}

void ezlopi_cloud_modes_switch(cJSON *cj_request, cJSON *cj_response)
{

    s_house_modes_t *house_mode = NULL;

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON *cj_mode_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_modeId_str);
        if (cj_mode_id && cj_mode_id->valuestring)
        {
            uint32_t mode_id = strtoul(cj_mode_id->valuestring, NULL, 16);
            house_mode = ezlopi_core_modes_get_house_mode_by_id(mode_id);
        }
        else
        {
            cJSON *cj_mode_name = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_name_str);
            if (cj_mode_name && cj_mode_name->valuestring)
            {
                house_mode = ezlopi_core_modes_get_house_mode_by_name(cj_mode_name->valuestring);
            }
        }
    }

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        if (house_mode)
        {
            ezlopi_core_modes_api_switch_mode(house_mode);
            cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_switchToDelay_str, house_mode->switch_to_delay_sec);
            cJSON_AddNumberToObject(__FUNCTION__, cj_result, ezlopi_alarmDelay_str, house_mode->alarm_delay_sec);
        }
    }
}

void ezlopi_cloud_modes_cancel_switch(cJSON *cj_request, cJSON *cj_response)
{

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        s_ezlopi_modes_t *custom_mode = ezlopi_core_modes_get_custom_modes();
        if (custom_mode)
        {
            ezlopi_core_modes_api_cancel_switch();
            CJSON_ASSIGN_ID(cj_result, custom_mode->current_mode_id, ezlopi_modeId_str);
        }
    }
}

void ezlopi_cloud_modes_entry_delay_cancel(cJSON *cj_request, cJSON *cj_response)
{

    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        ezlopi_core_modes_api_cancel_entry_delay();
    }
}

void ezlopi_cloud_modes_entry_delay_skip(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        #warning "Implementation required";
    }
}

void ezlopi_cloud_modes_switch_to_delay_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        double _switch_to_delay = 0;
        CJSON_GET_VALUE_DOUBLE(cj_params, ezlopi_switchTo_str, _switch_to_delay);
        ezlopi_core_modes_set_switch_to_delay((uint32_t)_switch_to_delay);
    }
}

void ezlopi_cloud_modes_alarm_delay_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        double _switch_to_delay = 0;
        CJSON_GET_VALUE_DOUBLE(cj_params, ezlopi_switchTo_str, _switch_to_delay);
        ezlopi_core_modes_set_alarm_delay((uint32_t)_switch_to_delay);
    }
}

void ezlopi_cloud_modes_notifications_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_result && cj_params)
    {
        bool all = false;
        CJSON_GET_VALUE_BOOL(cj_params, "all", all);
        cJSON *cj_modeId = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_modeId_str);
        cJSON *cj_userIds = cJSON_GetObjectItem(__FUNCTION__, cj_params, "userIds");
        if (cj_userIds && cj_modeId)
        {
            uint8_t modeId = strtoul(cj_modeId->valuestring, NULL, 10);
            ezlopi_core_modes_set_notifications(modeId, all, cj_userIds);
        }
    }
}

void ezlopi_cloud_modes_disarmed_default_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_modeId = cJSON_GetObjectItem(__func__, cj_params, ezlopi_modeId_str);
            cJSON *cj_disarmedDefault = cJSON_GetObjectItem(__func__, cj_params, ezlopi_disarmedDefault_str);
            if (cj_modeId && cj_disarmedDefault)
            {
                uint8_t modeId = strtoul(cj_modeId->valuestring, NULL, 10);
                bool disarmedDefault = cj_disarmedDefault->type == cJSON_True ? true : false;

                // toggle the flag in - inactive and active mode
                ezlopi_core_modes_set_disarmed_default(modeId, disarmedDefault);

                // Trigger broadcast if the devices in 'modeId' is 'disarmed'.
                s_house_modes_t *target_house_mode = ezlopi_core_modes_get_house_mode_by_id(modeId);
                if (target_house_mode)
                { // To disarm devices [given in the list -> 'default-disarmed'].
                    ezlopi_core_modes_set_unset_device_armed_status(target_house_mode->cj_disarmed_devices, !disarmedDefault);
                }
            }
        }
    }
}

void ezlopi_cloud_modes_disarmed_devices_add(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_modeId = cJSON_GetObjectItem(__func__, cj_params, ezlopi_modeId_str);
            cJSON *cj_deviceId = cJSON_GetObjectItem(__func__, cj_params, ezlopi_deviceId_str);
            if (cj_modeId && cj_deviceId)
            {
                uint8_t modeId = strtoul(cj_modeId->valuestring, NULL, 10);
                ezlopi_core_modes_add_disarmed_device(modeId, cj_deviceId->valuestring);
            }
        }
    }
}

void ezlopi_cloud_modes_disarmed_devices_remove(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_modeID = cJSON_GetObjectItem(__func__, cj_params, ezlopi_modeId_str);
            cJSON *cj_deviceId = cJSON_GetObjectItem(__func__, cj_params, ezlopi_deviceId_str);
            if (cj_modeID && cj_deviceId)
            {
                uint8_t modeId = strtoul(cj_modeID->valuestring, NULL, 10);
                ezlopi_core_modes_remove_disarmed_device(modeId, cj_deviceId->valuestring);
            }
        }
    }
}

void ezlopi_cloud_modes_alarms_off_add(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_mode_id = cJSON_GetObjectItem(__func__, cj_params, ezlopi_modeId_str);
            cJSON *cj_device_id = cJSON_GetObjectItem(__func__, cj_params, ezlopi_deviceId_str);
            if (cj_mode_id && cj_device_id && cJSON_IsString(cj_mode_id) && cJSON_IsString(cj_device_id))
            {
                uint8_t mode_id = strtoul(cj_mode_id->valuestring, NULL, 10);
                ezlopi_core_modes_add_alarm_off(mode_id, cj_device_id);
            }
        }
    }
}

void ezlopi_cloud_modes_alarms_off_remove(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_mode_id = cJSON_GetObjectItem(__func__, cj_params, ezlopi_modeId_str);
            cJSON *cj_device_id = cJSON_GetObjectItem(__func__, cj_params, ezlopi_deviceId_str);
            if (cj_mode_id && cj_device_id && cJSON_IsString(cj_mode_id) && cJSON_IsString(cj_device_id))
            {
                uint32_t mode_id = strtoul(cj_mode_id->valuestring, NULL, 16);
                ezlopi_core_modes_remove_alarm_off(mode_id, cj_device_id);
            }
        }
    }
}

void ezlopi_cloud_modes_cameras_off_add(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_cameras_off_remove(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_bypass_devices_add(cJSON *cj_request, cJSON *cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON* cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON* cj_modelID = cJSON_GetObjectItem(__func__, cj_params, ezlopi_modeId_str);
            cJSON* cj_deviceIds = cJSON_GetObjectItem(__func__, cj_params, ezlopi_device_ids_str);
            if (cj_modelID && cj_deviceIds && (cj_deviceIds->type == cJSON_Array))
            {
                uint8_t modeId = strtoul(cj_modelID->valuestring, NULL, 10);
                ezlopi_core_modes_bypass_device_add(modeId, cj_deviceIds);
            }
        }
    }
}

void ezlopi_cloud_modes_bypass_devices_remove(cJSON *cj_request, cJSON *cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON* cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON* cj_modelID = cJSON_GetObjectItem(__func__, cj_params, ezlopi_modeId_str);
            cJSON* cj_deviceIds = cJSON_GetObjectItem(__func__, cj_params, ezlopi_device_ids_str);
            if (cj_modelID && cj_deviceIds && (cj_deviceIds->type == cJSON_Array))
            {
                uint8_t modeId = strtoul(cj_modelID->valuestring, NULL, 10);
                ezlopi_core_modes_bypass_device_remove(modeId, cj_deviceIds);
            }
        }
    }
}

void ezlopi_cloud_modes_protect_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        bool protect_state = false;
        double timestamp = 0;

        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            CJSON_GET_VALUE_BOOL(cj_params, ezlopi_protect_str, protect_state);
            CJSON_GET_VALUE_DOUBLE(cj_params, ezlopi_timestamp_str, timestamp); // timestamp not needed rightnow

            cJSON *cj_mode_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_modeId_str);
            if (cj_mode_id && cj_mode_id->valuestring)
            {
                uint32_t mode_id = strtoul(cj_mode_id->valuestring, NULL, 16);
                ezlopi_core_modes_set_protect(mode_id, protect_state);
            }
        }
    }
}

void ezlopi_cloud_modes_protect_buttons_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_protect_devices_add(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_protect_devices_remove(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_entry_delay_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            double normal_sec = 30;
            // double short_sec = 30;
            double extended_sec = 60;
            double long_extended_sec = 120;
            double instant_sec = 0;

            CJSON_GET_VALUE_DOUBLE(cj_params, ezlopi_normal_str, normal_sec);
            normal_sec = (normal_sec > 240) ? 240 : normal_sec;

            // CJSON_GET_VALUE_DOUBLE(cj_params, ezlopi_short_str, short_sec);
            // short_sec = (short_sec > 240) ? 240 : short_sec;

            CJSON_GET_VALUE_DOUBLE(cj_params, ezlopi_long_extended_str, long_extended_sec);
            long_extended_sec = (long_extended_sec > 240) ? 240 : long_extended_sec;

            CJSON_GET_VALUE_DOUBLE(cj_params, ezlopi_extended_str, extended_sec);
            extended_sec = (extended_sec > 240) ? 240 : extended_sec;

            CJSON_GET_VALUE_DOUBLE(cj_params, ezlopi_instant_str, instant_sec);
            instant_sec = (instant_sec > 240) ? 240 : instant_sec;

            ezlopi_core_modes_set_entry_delay((uint32_t)normal_sec, (uint32_t)extended_sec, (uint32_t)long_extended_sec, (uint32_t)instant_sec);
        }
    }
}

void ezlopi_cloud_modes_entry_delay_reset(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        ezlopi_core_modes_reset_entry_delay();
    }
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

#endif // CONFIG_EZPI_SERV_ENABLE_MODES

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
