#include <string.h>
#include <stdint.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_cloud_settings.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_methods_str.h"

#include "ezlopi_service_webprov.h"

void ezlopi_device_settings_list_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_settings_array = cJSON_AddArrayToObject(__FUNCTION__, cj_result, ezlopi_settings_str);
        if (cj_settings_array)
        {
            l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
            while (curr_device)
            {
                l_ezlopi_device_settings_v3_t *curr_setting = curr_device->settings;
                while (curr_setting)
                {
                    cJSON *cj_properties = cJSON_CreateObject(__FUNCTION__);
                    if (cj_properties)
                    {
                        char tmp_string[64];
                        snprintf(tmp_string, sizeof(tmp_string), "%08x", curr_setting->cloud_properties.setting_id);
                        cJSON_AddStringToObject(__FUNCTION__, cj_properties, ezlopi__id_str, tmp_string);
                        snprintf(tmp_string, sizeof(tmp_string), "%08x", curr_device->cloud_properties.device_id);
                        cJSON_AddStringToObject(__FUNCTION__, cj_properties, ezlopi_deviceId_str, tmp_string);
                        cJSON_AddStringToObject(__FUNCTION__, cj_properties, ezlopi_status_str, ezlopi_synced_str);
                        curr_setting->func(EZLOPI_SETTINGS_ACTION_GET_SETTING, curr_setting, cj_properties, curr_setting->user_arg);
                        if (!cJSON_AddItemToArray(cj_settings_array, cj_properties))
                        {
                            cJSON_Delete(__FUNCTION__, cj_properties);
                        }
                    }
                    curr_setting = curr_setting->next;
                }
                curr_device = curr_device->next;
            }
        }
    }
}

void ezlopi_device_settings_value_set_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        uint32_t setting_id = 0;
        CJSON_GET_ID(setting_id, cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str));

        l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
        uint32_t found_setting = 0;
        while (curr_device)
        {
            l_ezlopi_device_settings_v3_t *curr_setting = curr_device->settings;
            while (curr_setting)
            {
                if (setting_id == curr_setting->cloud_properties.setting_id)
                {
                    curr_setting->func(EZLOPI_SETTINGS_ACTION_SET_SETTING, curr_setting, cj_params, curr_setting->user_arg);
                    found_setting = 1;
                    break;
                }
                curr_setting = curr_setting->next;
            }
            if (found_setting)
            {
                break;
            }
            curr_device = curr_device->next;
        }
    }
}

void ezlopi_device_settings_reset_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        l_ezlopi_device_t *curr_device = ezlopi_device_get_head();

        while (curr_device)
        {
            // check for deviceId first
            if (cJSON_HasObjectItem(__FUNCTION__, cj_params, ezlopi_deviceId_str))
            {
                uint32_t device_id = 0;
                CJSON_GET_ID(device_id, cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_deviceId_str));

                if (device_id == curr_device->cloud_properties.device_id)
                {
                    l_ezlopi_device_settings_v3_t *curr_setting = curr_device->settings;
                    while (curr_setting)
                    {
                        curr_setting->func(EZLOPI_SETTINGS_ACTION_RESET_SETTING, curr_setting, cj_params, curr_setting->user_arg);
                        curr_setting = curr_setting->next;
                    }
                }
                else
                {
                    TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_WARNING, "deviceId: %04X not found!", device_id);
                }
            }
            // checking for settingId
            else if (cJSON_HasObjectItem(__FUNCTION__, cj_params, ezlopi__id_str))
            {
                bool found_id = false;
                uint32_t setting_id = 0;
                CJSON_GET_ID(setting_id, cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str));

                l_ezlopi_device_settings_v3_t *curr_setting = curr_device->settings;
                while (curr_setting)
                {
                    if (setting_id == curr_setting->cloud_properties.setting_id)
                    {
                        found_id = true;
                        curr_setting->func(EZLOPI_SETTINGS_ACTION_RESET_SETTING, curr_setting, cj_params, curr_setting->user_arg);
                    }
                    curr_setting = curr_setting->next;
                }

                if (false == found_id)
                {
                    TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_WARNING, "settingId: %04X not found!", setting_id);
                }
            }

            curr_device = curr_device->next;
        }
    }
}
