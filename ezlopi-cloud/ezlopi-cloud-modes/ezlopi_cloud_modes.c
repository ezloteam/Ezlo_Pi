#include <string.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_modes.h"
#include "ezlopi_core_modes_cjson.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_api_methods.h"
#include "ezlopi_core_ezlopi_broadcast.h"

#include "ezlopi_cloud_modes.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_modes_updaters.h"
#include "EZLOPI_USER_CONFIG.h"

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)

void ezlopi_cloud_modes_get(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        ezlopi_core_modes_api_get_modes(cj_result);
    }
}

void ezlopi_cloud_modes_current_get(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    if (cj_result)
    {
        ezlopi_core_modes_api_get_current_mode(cj_result);
    }
}

void ezlopi_cloud_modes_switch(cJSON* cj_request, cJSON* cj_response)
{

    s_house_modes_t* house_mode = NULL;

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_mode_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_modeId_str);
        if (cj_mode_id && cj_mode_id->valuestring)
        {
            uint32_t mode_id = strtoul(cj_mode_id->valuestring, NULL, 16);
            house_mode = ezlopi_core_modes_get_house_mode_by_id(mode_id);
        }
        else
        {
            cJSON* cj_mode_name = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_name_str);
            if (cj_mode_name && cj_mode_name->valuestring)
            {
                house_mode = ezlopi_core_modes_get_house_mode_by_name(cj_mode_name->valuestring);
            }
        }
    }

    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
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

void ezlopi_cloud_modes_cancel_switch(cJSON* cj_request, cJSON* cj_response)
{

    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        s_ezlopi_modes_t* custom_mode = ezlopi_core_modes_get_custom_modes();
        if (custom_mode)
        {
            ezlopi_core_modes_api_cancel_switch();
            CJSON_ASSIGN_ID(cj_result, custom_mode->current_mode_id, ezlopi_modeId_str);
        }
    }
}

void ezlopi_cloud_modes_entry_delay_cancel(cJSON* cj_request, cJSON* cj_response)
{

    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        ezlopi_core_modes_api_cancel_entry_delay();
    }
}

void ezlopi_cloud_modes_entry_delay_skip(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        #warning "Implementation required"
    }
}

void ezlopi_cloud_modes_switch_to_delay_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        double _switch_to_delay = 0;
        CJSON_GET_VALUE_DOUBLE(cj_params, ezlopi_switchTo_str, _switch_to_delay);
        ezlopi_core_modes_set_switch_to_delay((uint32_t)_switch_to_delay);
    }
}

void ezlopi_cloud_modes_alarm_delay_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        double _switch_to_delay = 0;
        CJSON_GET_VALUE_DOUBLE(cj_params, ezlopi_switchTo_str, _switch_to_delay);
        ezlopi_core_modes_set_alarm_delay((uint32_t)_switch_to_delay);
    }
}

void ezlopi_cloud_modes_notifications_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_disarmed_default_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(__func__, cj_response, ezlopi_result_str);
    cJSON* cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        cJSON* cj_modeId = cJSON_GetObjectItem(__func__, cj_params, ezlopi_modeId_str);
        cJSON* cj_disarmedDefault = cJSON_GetObjectItem(__func__, cj_params, ezlopi_disarmedDefault_str);
        if (cj_modeId && cj_disarmedDefault)
        {
            uint8_t modeId = strtoul(cj_modeId->valuestring, NULL, 10);
            bool disarmedDefault = cj_disarmedDefault->type == cJSON_True ? true : false;
            ezlopi_core_modes_set_disarmed_default(modeId, disarmedDefault);
            s_house_modes_t* current_house_mode = ezlopi_core_modes_get_current_house_modes();
            if (current_house_mode && current_house_mode->_id == modeId)
            {
                if (current_house_mode->disarmed_default)
                {
                    ezlopi_core_modes_set_unset_device_armed_status(current_house_mode->cj_disarmed_devices, true);
                }
                else
                {
                    ezlopi_core_modes_set_unset_device_armed_status(current_house_mode->cj_disarmed_devices, false);
                }
            }
        }
    }
}

void ezlopi_cloud_modes_disarmed_devices_add(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__func__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON* cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON* cj_modeId = cJSON_GetObjectItem(__func__, cj_params, ezlopi_modeId_str);
            cJSON* cj_deviceId = cJSON_GetObjectItem(__func__, cj_params, ezlopi_deviceId_str);
            if (cj_modeId && cj_deviceId)
            {
                uint8_t modeId = strtoul(cj_modeId->valuestring, NULL, 10);
                ezlopi_core_modes_add_disarmed_device(modeId, cj_deviceId->valuestring);
            }
        }
    }
}

void ezlopi_cloud_modes_disarmed_devices_remove(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON* cj_params = cJSON_GetObjectItem(__func__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON* cj_modeID = cJSON_GetObjectItem(__func__, cj_params, ezlopi_modeId_str);
            cJSON* cj_deviceId = cJSON_GetObjectItem(__func__, cj_params, ezlopi_deviceId_str);
            if (cj_modeID && cj_deviceId)
            {
                uint8_t modeId = strtoul(cj_modeID->valuestring, NULL, 10);
                ezlopi_core_modes_remove_disarmed_device(modeId, cj_deviceId->valuestring);
            }
        }
    }
}

void ezlopi_cloud_modes_alarms_off_add(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_alarms_off_remove(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_cameras_off_add(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_cameras_off_remove(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_bypass_devices_add(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_bypass_devices_remove(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_protect_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_protect_buttons_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_protect_devices_add(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_protect_devices_remove(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_entry_delay_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

void ezlopi_cloud_modes_entry_delay_reset(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
    }
}

#endif // CONFIG_EZPI_SERV_ENABLE_MODES
