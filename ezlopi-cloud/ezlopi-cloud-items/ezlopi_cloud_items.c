#include "../../build/config/sdkconfig.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// #include "cjext.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_room.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_devices_list.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_methods_str.h"

#include "ezlopi_service_webprov.h"

static cJSON *ezlopi_device_create_item_table_from_prop(l_ezlopi_device_t *device_prop, l_ezlopi_item_t *item_properties)
{
    cJSON *cj_item_properties = cJSON_CreateObject(__FUNCTION__);
    if (cj_item_properties)
    {
        char tmp_string[64];
        snprintf(tmp_string, sizeof(tmp_string), "%08x", item_properties->cloud_properties.item_id);
        cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi__id_str, tmp_string);
        snprintf(tmp_string, sizeof(tmp_string), "%08x", item_properties->cloud_properties.device_id);
        cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi_deviceId_str, tmp_string);
        // cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi_deviceName_str, curr_device->cloud_properties.device_name);
        // cJSON_AddTrueToObject(__FUNCTION__, cj_item_properties, "deviceArmed");
        cJSON_AddBoolToObject(__FUNCTION__, cj_item_properties, ezlopi_hasGetter_str, item_properties->cloud_properties.has_getter);
        cJSON_AddBoolToObject(__FUNCTION__, cj_item_properties, ezlopi_hasSetter_str, item_properties->cloud_properties.has_setter);
        cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi_name_str, item_properties->cloud_properties.item_name);
        cJSON_AddTrueToObject(__FUNCTION__, cj_item_properties, ezlopi_show_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi_valueType_str, item_properties->cloud_properties.value_type);

        if (item_properties->cloud_properties.scale)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi_scale_str, item_properties->cloud_properties.scale);
        }
        item_properties->func(EZLOPI_ACTION_HUB_GET_ITEM, item_properties, cj_item_properties, item_properties->user_arg);
        if (item_properties->cloud_properties.scale)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi_scale_str, item_properties->cloud_properties.scale);
        }
        cJSON_AddStringToObject(__FUNCTION__, cj_item_properties, ezlopi_status_str, ezlopi_idle_str);
    }

    return cj_item_properties;
}

void items_list_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_items_array = cJSON_AddArrayToObject(__FUNCTION__, cj_result, ezlopi_items_str);

        if (cj_items_array)
        {

            cJSON *params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
            if (params != NULL)
            {
                cJSON *device_ids_array = cJSON_GetObjectItem(__FUNCTION__, params, ezlopi_device_ids_str);

                if (device_ids_array != NULL)
                {
                    if (cJSON_IsArray(device_ids_array))
                    {
                        if (cJSON_GetArraySize(device_ids_array) > 0)
                        {
                            cJSON *device_id;
                            cJSON_ArrayForEach(device_id, device_ids_array)
                            {
                                if (cJSON_IsString(device_id))
                                {
                                    char *device_id_str = device_id->valuestring;
                                    uint32_t device_id = strtol(device_id_str, NULL, 16);

                                    l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();
                                    while (curr_device)
                                    {
                                        if (curr_device->cloud_properties.device_id == device_id)
                                        {
                                            l_ezlopi_item_t *curr_item = curr_device->items;
                                            while (curr_item)
                                            {
                                                cJSON *cj_item_properties = ezlopi_device_create_item_table_from_prop(curr_device, curr_item);
                                                if (cj_item_properties)
                                                {
                                                    if (!cJSON_AddItemToArray(cj_items_array, cj_item_properties))
                                                    {
                                                        cJSON_Delete(__FUNCTION__, cj_item_properties);
                                                    }
                                                }

                                                curr_item = curr_item->next;
                                            }
                                        }

                                        curr_device = curr_device->next;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();
                    while (curr_device)
                    {
                        l_ezlopi_item_t *curr_item = curr_device->items;
                        while (curr_item)
                        {
                            cJSON *cj_item_properties = ezlopi_device_create_item_table_from_prop(curr_device, curr_item);
                            if (cj_item_properties)
                            {

                                if (!cJSON_AddItemToArray(cj_items_array, cj_item_properties))
                                {
                                    cJSON_Delete(__FUNCTION__, cj_item_properties);
                                }
                            }

                            curr_item = curr_item->next;
                        }

                        curr_device = curr_device->next;
                    }
                }
            }
        }
    }
}

void items_set_value_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        uint32_t item_id = 0;
        CJSON_GET_ID(item_id, cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi__id_str));
        TRACE_S("item_id: %X", item_id);

        l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();
        uint32_t found_item = 0;
        while (curr_device)
        {
            l_ezlopi_item_t *curr_item = curr_device->items;
            while (curr_item)
            {
                if (item_id == curr_item->cloud_properties.item_id)
                {
                    curr_item->func(EZLOPI_ACTION_SET_VALUE, curr_item, cj_params, curr_item->user_arg);
                    found_item = 1;
                    break;
                }
                curr_item = curr_item->next;
            }
            if (found_item)
            {
                break;
            }
            curr_device = curr_device->next;
        }
    }
}

#if 0
void items_update_v3(cJSON *cj_request, cJSON *cj_response)
{
    bool _break_loop = false;

    cJSON *cjson_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cjson_params)
    {
        uint32_t item_id = 0;
        CJSON_GET_ID(item_id, cJSON_GetObjectItem(__FUNCTION__, cjson_params, ezlopi__id_str));

        l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();
        while (curr_device)
        {
            l_ezlopi_item_t *curr_item = curr_device->items;
            while (curr_item)
            {
                if (item_id == curr_item->cloud_properties.item_id)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
                    cJSON_AddStringToObject(__FUNCTION__, cj_response, ezlopi_msg_subclass_str, method_hub_item_updated);
                    cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
                    if (cj_result)
                    {
                        char tmp_string[64];

                        snprintf(tmp_string, sizeof(tmp_string), "%08x", item_id);
                        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi__id_str, tmp_string);

                        snprintf(tmp_string, sizeof(tmp_string), "%08x", curr_device->cloud_properties.device_id);
                        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_deviceId_str, tmp_string);

                        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_deviceName_str, curr_device->cloud_properties.device_name);
                        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_deviceCategory_str, curr_device->cloud_properties.category);
                        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_deviceSubcategory_str, curr_device->cloud_properties.subcategory);

                        char *room_name = ezlopi_core_room_get_name_by_id(curr_device->cloud_properties.room_id);
                        if (room_name)
                        {
                            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_roomName_str, room_name);
                        }
                        else
                        {
                            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_roomName_str, ezlopi__str);
                        }

                        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_roomName_str, ezlopi__str);
                        cJSON_AddFalseToObject(__FUNCTION__, cj_result, ezlopi_serviceNotification_str);
                        cJSON_AddTrueToObject(__FUNCTION__, cj_result, ezlopi_userNotification_str);
                        cJSON_AddNullToObject(__FUNCTION__, cj_result, ezlopi_notifications_str);
                        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_name_str, curr_item->cloud_properties.item_name);
                        if (curr_item->cloud_properties.scale)
                        {
                            cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_scale_str, curr_item->cloud_properties.scale);
                        }
                        curr_item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, curr_item, cj_result, curr_item->user_arg);
                        cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueType_str, curr_item->cloud_properties.value_type);
                    }

                    _break_loop = true;
                    break;
                }

                curr_item = curr_item->next;
            }

            if (_break_loop == true)
            {
                break;
            }

            curr_device = curr_device->next;
        }
    }
}

cJSON *ezlopi_cloud_items_updated_from_devices_v3(l_ezlopi_device_t *device, l_ezlopi_item_t *item)
{
    cJSON *cjson_response = cJSON_CreateObject(__FUNCTION__);
    if (cjson_response)
    {
        if (NULL != item)
        {
            cJSON_AddStringToObject(__FUNCTION__, cjson_response, ezlopi_msg_subclass_str, method_hub_item_updated);
            cJSON_AddNumberToObject(__FUNCTION__, cjson_response, ezlopi_msg_id_str, EZPI_service_web_provisioning_get_message_count());
            cJSON_AddStringToObject(__FUNCTION__, cjson_response, ezlopi_id_str, ezlopi_ui_broadcast_str);

            cJSON *cj_result = cJSON_AddObjectToObject(__FUNCTION__, cjson_response, ezlopi_result_str);
            if (cj_result)
            {
                char tmp_string[64];
                snprintf(tmp_string, sizeof(tmp_string), "%08x", item->cloud_properties.item_id);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi__id_str, tmp_string);
                snprintf(tmp_string, sizeof(tmp_string), "%08x", device->cloud_properties.device_id);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_deviceId_str, tmp_string);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_deviceName_str, device->cloud_properties.device_name);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_deviceCategory_str, device->cloud_properties.category);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_deviceSubcategory_str, device->cloud_properties.subcategory);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_roomName_str, ezlopi__str); // roomName -> logic needs to be understood first
                cJSON_AddFalseToObject(__FUNCTION__, cj_result, ezlopi_serviceNotification_str);
                cJSON_AddFalseToObject(__FUNCTION__, cj_result, ezlopi_userNotification_str);
                cJSON_AddNullToObject(__FUNCTION__, cj_result, ezlopi_notifications_str);
                cJSON_AddFalseToObject(__FUNCTION__, cj_result, ezlopi_syncNotification_str);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_name_str, item->cloud_properties.item_name);
                if (item->cloud_properties.scale)
                {
                    cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_scale_str, item->cloud_properties.scale);
                }
                item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, cj_result, item->user_arg);
                // registered_device->device->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, registered_device->properties, cj_result, registered_device->user_arg);
                cJSON_AddStringToObject(__FUNCTION__, cj_result, ezlopi_valueType_str, item->cloud_properties.value_type);
            }
        }
        else
        {
            cJSON_Delete(__FUNCTION__, cjson_response);
            cjson_response = NULL;
        }
    }

    return cjson_response;
}
#endif
