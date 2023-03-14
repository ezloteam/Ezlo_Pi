#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sdkconfig.h"
#include "items.h"
#include "trace.h"
#include "frozen.h"
// #include "web_provisioning.h"

#include "cJSON.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_devices_list.h"
#include "web_provisioning.h"

void items_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);
    if (cj_result)
    {
        cJSON *cj_items_array = cJSON_AddArrayToObject(cj_result, "items");
        if (cj_items_array)
        {
            l_ezlopi_configured_devices_t *registered_device = ezlopi_devices_list_get_configured_items();
            while (NULL != registered_device)
            {
                if (NULL != registered_device->properties)
                {
                    cJSON *cj_properties = cJSON_CreateObject();
                    if (cj_properties)
                    {
                        char tmp_string[64];
                        snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_device->properties->ezlopi_cloud.item_id);
                        cJSON_AddStringToObject(cj_properties, "_id", tmp_string);
                        snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_device->properties->ezlopi_cloud.device_id);
                        cJSON_AddStringToObject(cj_properties, "deviceId", tmp_string);
                        cJSON_AddStringToObject(cj_properties, "deviceName", registered_device->properties->ezlopi_cloud.device_name);
                        cJSON_AddTrueToObject(cj_properties, "deviceArmed");
                        cJSON_AddBoolToObject(cj_properties, "hasGetter", registered_device->properties->ezlopi_cloud.has_getter);
                        cJSON_AddBoolToObject(cj_properties, "hasSetter", registered_device->properties->ezlopi_cloud.has_setter);
                        cJSON_AddStringToObject(cj_properties, "name", registered_device->properties->ezlopi_cloud.item_name);
                        cJSON_AddTrueToObject(cj_properties, "show");
                        cJSON_AddStringToObject(cj_properties, "valueType", registered_device->properties->ezlopi_cloud.value_type);

                        registered_device->device->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, registered_device->properties, cj_properties, registered_device->user_arg);
                        cJSON_AddStringToObject(cj_properties, "valueFormatted", "false");
                        cJSON_AddStringToObject(cj_properties, "status", "synced");

                        if (!cJSON_AddItemToArray(cj_items_array, cj_properties))
                        {
                            cJSON_Delete(cj_properties);
                        }
                    }
                }

                registered_device = registered_device->next;
            }
        }
    }
}

void items_set_value(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddObjectToObject(cj_response, ezlopi_result);

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        char *item_id_str = 0;
        CJSON_GET_VALUE_STRING(cj_params, ezlopi__id_str, item_id_str);
        int item_id = strtol(item_id_str, NULL, 16);
        TRACE_I("item_id: %X", item_id);

        l_ezlopi_configured_devices_t *registered_device = ezlopi_devices_list_get_configured_items();
        while (NULL != registered_device)
        {
            if (registered_device->properties)
            {
                if (item_id == registered_device->properties->ezlopi_cloud.item_id)
                {
                    registered_device->device->func(EZLOPI_ACTION_SET_VALUE, registered_device->properties, cj_params, registered_device->user_arg);
                }
            }

            registered_device = registered_device->next;
        }
    }
}

void items_update(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cjson_params = cJSON_GetObjectItem(cj_request, "params");
    if (cjson_params)
    {
        char *item_id_str = NULL;
        CJSON_GET_VALUE_STRING(cjson_params, ezlopi__id_str, item_id_str);
        int item_id = strtol(item_id_str, NULL, 16);

        l_ezlopi_configured_devices_t *registered_device = ezlopi_devices_list_get_configured_items();
        while (NULL != registered_device)
        {
            if (NULL != registered_device->properties)
            {
                if (item_id == registered_device->properties->ezlopi_cloud.item_id)
                {
                    cJSON_AddStringToObject(cj_response, ezlopi_id_str, ezlopi_ui_broadcast);
                    cJSON_AddStringToObject(cj_response, ezlopi_msg_subclass, method_hub_item_updated);
                    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);
                    if (cj_result)
                    {
                        char tmp_string[64];
                        cJSON_AddStringToObject(cj_result, "_id", item_id_str);
                        snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_device->properties->ezlopi_cloud.device_id);
                        cJSON_AddStringToObject(cj_result, "deviceId", tmp_string);
                        cJSON_AddStringToObject(cj_result, "deviceName", registered_device->properties->ezlopi_cloud.device_name);
                        cJSON_AddStringToObject(cj_result, "deviceCategory", registered_device->properties->ezlopi_cloud.category);
                        cJSON_AddStringToObject(cj_result, "deviceSubcategory", registered_device->properties->ezlopi_cloud.subcategory);
                        cJSON_AddStringToObject(cj_result, "roomName", registered_device->properties->ezlopi_cloud.room_name);
                        cJSON_AddFalseToObject(cj_result, "serviceNotification");
                        cJSON_AddFalseToObject(cj_result, "userNotification");
                        cJSON_AddNullToObject(cj_result, "notifications");
                        cJSON_AddStringToObject(cj_result, "name", registered_device->properties->ezlopi_cloud.item_name);
                        registered_device->device->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, registered_device->properties, cj_result, registered_device->user_arg);
                        cJSON_AddStringToObject(cj_result, "valueType", registered_device->properties->ezlopi_cloud.value_type);
                    }

                    break;
                }
            }

            registered_device = registered_device->next;
        }
    }
}

cJSON *ezlopi_cloud_items_updated_from_devices(l_ezlopi_configured_devices_t *registered_device)
{
    cJSON *cjson_response = cJSON_CreateObject();

    if (NULL != registered_device)
    {
        if (NULL != registered_device->properties)
        {
            if (cjson_response)
            {
                cJSON_AddStringToObject(cjson_response, ezlopi_msg_subclass, method_hub_item_updated);
                cJSON_AddNumberToObject(cjson_response, ezlopi_msg_id_str, web_provisioning_get_message_count());
                cJSON_AddStringToObject(cjson_response, ezlopi_id_str, ezlopi_ui_broadcast);
                cJSON *cj_result = cJSON_AddObjectToObject(cjson_response, ezlopi_result);
                if (cj_result)
                {
                    char tmp_string[64];
                    snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_device->properties->ezlopi_cloud.item_id);
                    cJSON_AddStringToObject(cj_result, "_id", tmp_string);
                    snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_device->properties->ezlopi_cloud.device_id);
                    cJSON_AddStringToObject(cj_result, "deviceId", tmp_string);
                    cJSON_AddStringToObject(cj_result, "deviceName", registered_device->properties->ezlopi_cloud.device_name);
                    cJSON_AddStringToObject(cj_result, "deviceCategory", registered_device->properties->ezlopi_cloud.category);
                    cJSON_AddStringToObject(cj_result, "deviceSubcategory", registered_device->properties->ezlopi_cloud.subcategory);
                    cJSON_AddStringToObject(cj_result, "roomName", registered_device->properties->ezlopi_cloud.room_name);
                    cJSON_AddFalseToObject(cj_result, "serviceNotification");
                    cJSON_AddFalseToObject(cj_result, "userNotification");
                    cJSON_AddNullToObject(cj_result, "notifications");
                    cJSON_AddStringToObject(cj_result, "name", registered_device->properties->ezlopi_cloud.item_name);
                    registered_device->device->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, registered_device->properties, cj_result, registered_device->user_arg);
                    cJSON_AddStringToObject(cj_result, "valueType", registered_device->properties->ezlopi_cloud.value_type);
                }
            }
        }
    }

    return cjson_response;
}
