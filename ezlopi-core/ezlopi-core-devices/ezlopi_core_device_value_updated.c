#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_ezlopi_broadcast.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_settings.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_webprov.h"

/// static declarations
static cJSON* __broadcast_message_items_updated_from_device(l_ezlopi_device_t* device, l_ezlopi_item_t* item);
static cJSON* __broadcast_message_settings_updated_from_devices_v3(l_ezlopi_device_t* device, l_ezlopi_device_settings_v3_t* setting);


/// Global methods
int ezlopi_device_value_updated_from_device_broadcast(l_ezlopi_item_t* item)
{
    int ret = 0;

    if (item)
    {
        l_ezlopi_device_t* curr_device = ezlopi_device_get_head();
        while (curr_device)
        {
            l_ezlopi_item_t* curr_item = curr_device->items;

            while (curr_item)
            {
                if (item == curr_item)
                {
                    cJSON* cj_response = __broadcast_message_items_updated_from_device(curr_device, item);
                    // CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);
                    if (cj_response)
                    {
                        if (!ezlopi_core_ezlopi_broadcast_add_to_queue(cj_response))
                        {
                            cJSON_Delete(cj_response);
                        }
                    }
                    break;
                }

                curr_item = curr_item->next;
            }

            curr_device = curr_device->next;
        }
    }

    return ret;
}

int ezlopi_device_value_updated_from_device_broadcast_by_item_id(uint32_t item_id)
{
    int ret = 0;

    l_ezlopi_device_t* curr_device = ezlopi_device_get_head();
    while (curr_device)
    {
        l_ezlopi_item_t* curr_item = curr_device->items;

        while (curr_item)
        {
            if (item_id == curr_item->cloud_properties.item_id)
            {
                // cJSON* cj_response = NULL;
                cJSON* cj_response = __broadcast_message_items_updated_from_device(curr_device, curr_item);
                CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);

                ret = ezlopi_core_ezlopi_broadcast_add_to_queue(cj_response);

                if (0 == ret)
                {
                    cJSON_Delete(cj_response);
                }

                break;
            }

            curr_item = curr_item->next;
        }

        curr_device = curr_device->next;
    }

    return ret;
}

int ezlopi_core_device_value_updated_settings_broadcast(l_ezlopi_device_settings_v3_t* setting)
{
    int ret = 0;

    if (setting)
    {
        l_ezlopi_device_t* curr_device = ezlopi_device_get_head();
        while (curr_device)
        {
            l_ezlopi_device_settings_v3_t* curr_setting = curr_device->settings;
            while (curr_setting)
            {
                if (setting == curr_setting)
                {
                    cJSON* cj_response = __broadcast_message_settings_updated_from_devices_v3(curr_device, setting);
                    CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);
                    ret = ezlopi_core_ezlopi_broadcast_add_to_queue(cj_response);

                    if (0 == ret)
                    {
                        cJSON_Delete(cj_response);
                    }
                    break;
                }
                curr_setting = curr_setting->next;
            }
            curr_device = curr_device->next;
        }
    }

    return ret;
}

#if 0
int ezlopi_setting_value_updated_from_device_settings_id_v3(uint32_t setting_id)
{
    int ret = 0;

    // if (setting)
    {
        l_ezlopi_device_t* curr_device = ezlopi_device_get_head();
        while (curr_device)
        {
            l_ezlopi_device_settings_v3_t* curr_setting = curr_device->settings;
            while (curr_setting)
            {
                if (setting_id == curr_setting->cloud_properties.setting_id)
                {
                    cJSON* cj_response = __broadcast_message_settings_updated_from_devices_v3(curr_device, curr_setting);
                    CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);
                    ret = ezlopi_core_ezlopi_broadcast_add_to_queue(cj_response);

                    if (0 == ret)
                    {
                        cJSON_Delete(cj_response);
                    }
                    break;
                }
                curr_setting = curr_setting->next;
            }
            curr_device = curr_device->next;
        }
    }

    return ret;
}
#endif

int ezlopi_core_device_value_update_wifi_scan_broadcast(cJSON* network_array)
{
    int ret = 0;
    if (network_array)
    {
        cJSON* cj_response = cJSON_CreateObject();
        if (cj_response)
        {
            cJSON_AddStringToObject(cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
            cJSON_AddStringToObject(cj_response, ezlopi_msg_subclass_str, method_hub_network_wifi_scan_progress);
            // cJSON_AddNumberToObject(cj_response, ezlopi_msg_id_str, ezlopi_service_web_provisioning_get_message_count());

            cJSON* result = cJSON_AddObjectToObject(cj_response, "result");
            if (result)
            {
                cJSON_AddStringToObject(result, "interfaceId", "wlan0");
                cJSON_AddStringToObject(result, "status", "process");
                cJSON_AddItemToObject(result, "networks", network_array);
            }
            else
            {
                ret = 1;
            }

            CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);
            ret = ezlopi_core_ezlopi_broadcast_add_to_queue(cj_response);

            if (0 == ret)
            {
                cJSON_Delete(cj_response);
            }
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }
    return ret;
}

/// static methods
static cJSON* __broadcast_message_items_updated_from_device(l_ezlopi_device_t* device, l_ezlopi_item_t* item)
{
    cJSON* cjson_response = cJSON_CreateObject();
    if (cjson_response)
    {
        if (NULL != item)
        {
            cJSON_AddStringToObject(cjson_response, ezlopi_msg_subclass_str, method_hub_item_updated);
            // cJSON_AddNumberToObject(cjson_response, ezlopi_msg_id_str, ezlopi_service_web_provisioning_get_message_count());
            cJSON_AddStringToObject(cjson_response, ezlopi_id_str, ezlopi_ui_broadcast_str);

            cJSON* cj_result = cJSON_AddObjectToObject(cjson_response, ezlopi_result_str);
            if (cj_result)
            {
                char tmp_string[64];
                snprintf(tmp_string, sizeof(tmp_string), "%08x", item->cloud_properties.item_id);
                cJSON_AddStringToObject(cj_result, ezlopi__id_str, tmp_string);
                snprintf(tmp_string, sizeof(tmp_string), "%08x", device->cloud_properties.device_id);
                cJSON_AddStringToObject(cj_result, ezlopi_deviceId_str, tmp_string);
                cJSON_AddStringToObject(cj_result, ezlopi_deviceName_str, device->cloud_properties.device_name);
                cJSON_AddStringToObject(cj_result, ezlopi_deviceCategory_str, device->cloud_properties.category);
                cJSON_AddStringToObject(cj_result, ezlopi_deviceSubcategory_str, device->cloud_properties.subcategory);
                cJSON_AddStringToObject(cj_result, ezlopi_roomName_str, ezlopi__str); // roomName -> logic needs to be understood first
                cJSON_AddFalseToObject(cj_result, ezlopi_serviceNotification_str);
                cJSON_AddFalseToObject(cj_result, ezlopi_userNotification_str);
                cJSON_AddNullToObject(cj_result, ezlopi_notifications_str);
                cJSON_AddFalseToObject(cj_result, ezlopi_syncNotification_str);
                cJSON_AddStringToObject(cj_result, ezlopi_name_str, item->cloud_properties.item_name);

                if (item->cloud_properties.scale)
                {
                    cJSON_AddStringToObject(cj_result, ezlopi_scale_str, item->cloud_properties.scale);
                }

                item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, item, cj_result, item->user_arg);
                // registered_device->device->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, registered_device->properties, cj_result, registered_device->user_arg);
                cJSON_AddStringToObject(cj_result, ezlopi_valueType_str, item->cloud_properties.value_type);
            }
        }
        else
        {
            cJSON_Delete(cjson_response);
            cjson_response = NULL;
        }
    }

    return cjson_response;
}

static cJSON* __broadcast_message_settings_updated_from_devices_v3(l_ezlopi_device_t* device, l_ezlopi_device_settings_v3_t* setting)
{
    cJSON* cjson_response = cJSON_CreateObject();
    if (cjson_response)
    {
        if (NULL != setting)
        {
            cJSON_AddStringToObject(cjson_response, ezlopi_msg_subclass_str, method_hub_device_setting_updated);
            // cJSON_AddNumberToObject(cjson_response, ezlopi_msg_id_str, ezlopi_service_web_provisioning_get_message_count());
            cJSON_AddStringToObject(cjson_response, ezlopi_id_str, ezlopi_ui_broadcast_str);

            cJSON* cj_result = cJSON_AddObjectToObject(cjson_response, ezlopi_result_str);
            if (cj_result)
            {
                char tmp_string[64];
                snprintf(tmp_string, sizeof(tmp_string), "%08x", setting->cloud_properties.setting_id);
                cJSON_AddStringToObject(cj_result, ezlopi__id_str, tmp_string);
                setting->func(EZLOPI_SETTINGS_ACTION_UPDATE_SETTING, setting, cj_result, setting->user_arg);
            }
        }
        else
        {
            cJSON_Delete(cjson_response);
            cjson_response = NULL;
        }
    }

    return cjson_response;
}