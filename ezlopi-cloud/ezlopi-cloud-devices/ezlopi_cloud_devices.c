#include <string.h>

#include "ezlopi_cloud_devices.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_api_methods.h"
#include "ezlopi_core_sntp.h"

#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_methods_str.h"
#include "cjext.h"

void devices_list_v3(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON* cjson_devices_array = cJSON_AddArrayToObject(__FUNCTION__, cjson_result, "devices");
        if (cjson_devices_array)
        {
            l_ezlopi_device_t* curr_device = ezlopi_device_get_head();

            while (NULL != curr_device)
            {
                cJSON* cj_properties = ezlopi_device_create_device_table_from_prop(curr_device);
                if (cj_properties)
                {
                    if (!cJSON_AddItemToArray(cjson_devices_array, cj_properties))
                    {
                        cJSON_Delete(__FUNCTION__, cj_properties);
                    }
                }
                curr_device = curr_device->next;
            }
        }
#if (1 == ENABLE_TRACE)
        else
        {
            TRACE_E("Failed to create devices-array");
        }
#endif
    }

#if (1 == ENABLE_TRACE)
    else
    {
        TRACE_E("Failed to create 'result'");
    }
#endif
}

void device_name_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON* cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
            if (cj_device_id)
            {
                uint32_t device_id = strtoul(cj_device_id->valuestring, NULL, 16);
                if (device_id)
                {
                    ezlopi_device_name_set_by_device_id(device_id, cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_name_str));
                }
            }
        }
    }
}

void device_armed_set(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON* cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
            cJSON* cj_armed_status = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_armed_str);
            if (cj_device_id && cj_armed_status)
            {
                uint32_t device_id = strtoul(cj_device_id->valuestring, NULL, 16);
                bool armed_status = cj_armed_status->type == cJSON_True ? true : false;
                ezlopi_device_set_reset_device_armed_status(device_id, armed_status);
            }
        }
    }
    return;
}

void device_updated(cJSON* cj_request, cJSON* cj_response)
{
    if (cj_request)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, "hub.device.updated");

        cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (cj_params)
            {
                cJSON* cj_device_id = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str);
                if (cj_device_id && cj_device_id->valuestring)
                {
                    uint32_t device_id = strtoul(cj_device_id->valuestring, NULL, 16);
                    l_ezlopi_device_t* device_node = ezlopi_device_get_head();
                    while (device_node)
                    {
                        if (device_id == device_node->cloud_properties.device_id)
                        {
                            char tmp_str[32];
                            snprintf(tmp_str, sizeof(tmp_str), "%08x", device_id);
                            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi__id_str, tmp_str);
                            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_name_str, device_node->cloud_properties.device_name);
                            cJSON_AddTrueToObject(__FUNCTION__, cj_result, ezlopi_syncNotification_str);

                            s_ezlopi_cloud_controller_t* controller_info = ezlopi_device_get_controller_information();

                            if (controller_info)
                            {
                                cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_armed_str, controller_info->armed ? cJSON_True : cJSON_False);
                                cJSON_AddBoolToObject(__FUNCTION__, cj_result, ezlopi_serviceNotification_str, controller_info->service_notification ? cJSON_True : cJSON_False);
                            }

                            uint64_t time = EZPI_CORE_sntp_get_current_time_ms();
                            cJSON_AddNumberToObject(__FUNCTION__, cj_result, "fwTimestampMs", time);

                            cJSON* cj_method = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_method_str);
                            if (cj_method)
                            {
                                cJSON* cj_device_changable = cJSON_AddArrayToObject(__FUNCTION__, cj_result, "__DEVICE_CHANGEABLE_FIELD");
                                if (cj_device_changable)
                                {
                                    cJSON_AddItemToArray(cj_device_changable, cJSON_CreateString(__FUNCTION__, ezlopi_name_str));
                                    cJSON_AddItemToArray(cj_device_changable, cJSON_CreateString(__FUNCTION__, ezlopi_armed_str));
                                }
                            }
                            TRACE_E("%s", cJSON_Print(__FUNCTION__, cj_response));
                            break;
                        }

                        device_node = device_node->next;
                    }
                }
            }
        }
    }
}


void device_group_create(cJSON* cj_request, cJSON* cj_response)
{
    if (cj_request && cj_response)
    {
        cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

        cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
        if (cj_params)
        {
            CJSON_TRACE("device_group params", cj_params);
            ezlopi_device_grp_add_to_head(0, cj_params);
        }
    }
}

////// updater for device-grps
////// for 'hub.device.group.created'


