#include <string.h>

#include "ezlopi_cloud_devices.h"
#include "trace.h"

#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_methods_str.h"
#include "cJSON.h"

void devices_list_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON *cjson_devices_array = cJSON_AddArrayToObject(cjson_result, "devices");
        if (cjson_devices_array)
        {
            l_ezlopi_device_t *curr_device = ezlopi_device_get_head();

            while (NULL != curr_device)
            {
                cJSON *cj_properties = ezlopi_device_create_device_table_from_prop(curr_device);
                if (cj_properties)
                {
                    if (!cJSON_AddItemToArray(cjson_devices_array, cj_properties))
                    {
                        cJSON_Delete(cj_properties);
                    }
                }

                curr_device = curr_device->next;
            }
        }
        else
        {
            TRACE_E("Failed to create devices-array");
        }
    }
    else
    {
        TRACE_E("Failed to create 'result'");
    }
}

void device_name_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
        if (cj_params)
        {
            cJSON *cj_device_id = cJSON_GetObjectItem(cj_params, ezlopi__id_str);
            if (cj_device_id)
            {
                uint32_t device_id = strtoul(cj_device_id->valuestring, NULL, 16);
                if (device_id)
                {
                    ezlopi_device_name_set_by_device_id(device_id, cJSON_GetObjectItem(cj_params, ezlopi_name_str));
                }
            }
        }
    }
}

void device_updated(cJSON *cj_request, cJSON *cj_response)
{
    if (cj_request)
    {
        cJSON_DeleteItemFromObject(cj_response, ezlopi_sender_str);
        cJSON_DeleteItemFromObject(cj_response, ezlopi_error_str);

        cJSON_AddStringToObject(cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
        cJSON_AddStringToObject(cj_response, ezlopi_msg_subclass_str, "hub.device.updated");

        cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
        if (cj_result)
        {
            cJSON *cj_params = cJSON_GetObjectItem(cj_request, ezlopi_params_str);
            if (cj_params)
            {
                cJSON *cj_device_id = cJSON_GetObjectItem(cj_params, ezlopi__id_str);
                if (cj_device_id && cj_device_id->valuestring)
                {
                    uint32_t device_id = strtoul(cj_device_id->valuestring, NULL, 16);
                    l_ezlopi_device_t *device_node = ezlopi_device_get_head();

                    while (device_node)
                    {
                        if (device_id == device_node->cloud_properties.device_id)
                        {
                            char tmp_str[32];
                            snprintf(tmp_str, sizeof(tmp_str), "%08x", device_id);
                            cJSON_AddStringToObject(cj_result, ezlopi__id_str, tmp_str);
                            cJSON_AddStringToObject(cj_result, ezlopi_name_str, device_node->cloud_properties.device_name);
                            cJSON_AddTrueToObject(cj_result, ezlopi_syncNotification_str);

                            s_ezlopi_cloud_controller_t *controller_info = ezlopi_device_get_controller_information();

                            if (controller_info)
                            {
                                cJSON_AddBoolToObject(cj_result, ezlopi_armed_str, controller_info->armed ? ezlopi_true_str : ezlopi_false_str);
                                cJSON_AddBoolToObject(cj_result, ezlopi_serviceNotification_str, controller_info->service_notification ? ezlopi_true_str : ezlopi_false_str);
                            }

                            break;
                        }

                        device_node = device_node->next;
                    }
                }
            }
        }
    }
}
