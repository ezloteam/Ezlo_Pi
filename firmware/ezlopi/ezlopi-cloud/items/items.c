#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "sdkconfig.h"
#include "items.h"
#include "trace.h"
#include "frozen.h"
// #include "web_provisioning.h"

#include "cJSON.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_devices_list.h"
#include "web_provisioning.h"

extern float dht11_service_get_temperature(void);
static void parse_item_id(char *data, uint32_t len, char *item_id);

void items_list(cJSON *cj_request, cJSON *cj_method, cJSON *cj_response)
{
 
        cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, "result");
        if (cjson_result)
        {
            cJSON *cjson_items_array = cJSON_AddArrayToObject(cjson_result, "items");
            if (cjson_items_array)
            {
                l_ezlopi_configured_devices_t *registered_device = ezlopi_devices_list_get_configured_items();
                while (NULL != registered_device)
                {
                    if (NULL != registered_device->properties)
                    {
                        cJSON *cjson_properties = cJSON_CreateObject();
                        if (cjson_properties)
                        {
                            char tmp_string[64];
                            snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_device->properties->ezlopi_cloud.item_id);
                            cJSON_AddStringToObject(cjson_properties, "_id", tmp_string);
                            snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_device->properties->ezlopi_cloud.device_id);
                            cJSON_AddStringToObject(cjson_properties, "deviceId", tmp_string);
                            cJSON_AddStringToObject(cjson_properties, "deviceName", registered_device->properties->ezlopi_cloud.device_name);
                            cJSON_AddTrueToObject(cjson_properties, "deviceArmed");
                            cJSON_AddBoolToObject(cjson_properties, "hasGetter", registered_device->properties->ezlopi_cloud.has_getter);
                            cJSON_AddBoolToObject(cjson_properties, "hasSetter", registered_device->properties->ezlopi_cloud.has_setter);
                            cJSON_AddStringToObject(cjson_properties, "name", registered_device->properties->ezlopi_cloud.item_name);
                            cJSON_AddTrueToObject(cjson_properties, "show");
                            cJSON_AddStringToObject(cjson_properties, "valueType", registered_device->properties->ezlopi_cloud.value_type);

                            registered_device->device->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, registered_device->properties, cjson_properties, registered_device->user_arg);
                            cJSON_AddStringToObject(cjson_properties, "valueFormatted", "false");
                            cJSON_AddStringToObject(cjson_properties, "status", "synced");

                            if (!cJSON_AddItemToArray(cjson_items_array, cjson_properties))
                            {
                                cJSON_Delete(cjson_properties);
                            }
                        }
                    }

                    registered_device = registered_device->next;
                }
            
        }
    }
}

cJSON *items_set_value(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    cJSON *cjson_response = cJSON_CreateObject();
    cJSON *cjson_request = cJSON_ParseWithLength(payload, len);

    if (cjson_request)
    {
        cJSON *cjson_id = cJSON_GetObjectItem(cjson_request, ezlopi_id_str);
        cJSON *cjson_sender = cJSON_GetObjectItem(cjson_request, "sender");
        cJSON *cjson_params = cJSON_GetObjectItem(cjson_request, "params");
        // cJSON *cjson_methods = cJSON_GetObjectItem(cjson_request, "method");

        if (cjson_params)
        {
            char *item_id_str = 0;
            CJSON_GET_VALUE_STRING(cjson_params, ezlopi__id_str, item_id_str);
            int item_id = strtol(item_id_str, NULL, 16);
            TRACE_I("item_id: %d | %X", item_id, item_id);

            l_ezlopi_configured_devices_t *registered_device = ezlopi_devices_list_get_configured_items();
            while (NULL != registered_device)
            {
                if (registered_device->properties)
                {
                    if (item_id == registered_device->properties->ezlopi_cloud.item_id)
                    {
                        registered_device->device->func(EZLOPI_ACTION_SET_VALUE, registered_device->properties, cjson_params, registered_device->user_arg);
                    }
                }

                registered_device = registered_device->next;
            }
        }

        if (cjson_response)
        {
            cJSON_AddStringToObject(cjson_response, "method", method_hub_item_value_set);
            cJSON_AddNumberToObject(cjson_response, "msg_id", msg_count);
            cJSON_AddNullToObject(cjson_response, "error");
            cJSON_AddStringToObject(cjson_response, ezlopi_id_str, cjson_id ? (cjson_id->valuestring ? cjson_id->valuestring : "") : "");
            cJSON_AddStringToObject(cjson_response, ezlopi_sender_str, cjson_sender ? (cjson_sender->valuestring ? cjson_sender->valuestring : "{}") : "{}");
            cJSON_AddObjectToObject(cjson_response, "result");
        }

        cJSON_Delete(cjson_request);
    }

    return cjson_response;
}

char *items_update(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    char *string_response = NULL;
    cJSON *cjson_request = cJSON_ParseWithLength(payload, len);
    if (cjson_request)
    {
        // cJSON *cjson_id = cJSON_GetObjectItem(cjson_request, ezlopi_id_str);
        // cJSON *cjson_sender = cJSON_GetObjectItem(cjson_request, "sender");
        cJSON *cjson_params = cJSON_GetObjectItem(cjson_request, "params");
        // cJSON *cjson_method = cJSON_GetObjectItem(cjson_request, "method");
        char *item_id_str = 0;

        if (cjson_params)
        {
            CJSON_GET_VALUE_STRING(cjson_params, ezlopi__id_str, item_id_str);
            int item_id = strtol(item_id_str, NULL, 16);
            TRACE_I("item_id: %d | %X", item_id, item_id);
            l_ezlopi_configured_devices_t *registered_device = ezlopi_devices_list_get_configured_items();

            while (NULL != registered_device)
            {
                if (NULL != registered_device->properties)
                {
                    if (item_id == registered_device->properties->ezlopi_cloud.item_id)
                    {
                        cJSON *cjson_response = cJSON_CreateObject();
                        if (cjson_response)
                        {
                            cJSON_AddStringToObject(cjson_response, "msg_subclass", method_hub_item_updated);
                            cJSON_AddNumberToObject(cjson_response, "msg_id", msg_count);
                            cJSON_AddStringToObject(cjson_response, "id", "ui_broadcast");
                            cJSON *cjson_result = cJSON_AddObjectToObject(cjson_response, "result");
                            if (cjson_result)
                            {
                                cJSON_AddStringToObject(cjson_result, "_id", item_id_str);
                                char tmp_string[64];
                                snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_device->properties->ezlopi_cloud.device_id);
                                cJSON_AddStringToObject(cjson_result, "deviceId", tmp_string);
                                cJSON_AddStringToObject(cjson_result, "deviceName", registered_device->properties->ezlopi_cloud.device_name);
                                cJSON_AddStringToObject(cjson_result, "deviceCategory", registered_device->properties->ezlopi_cloud.category);
                                cJSON_AddStringToObject(cjson_result, "deviceSubcategory", registered_device->properties->ezlopi_cloud.subcategory);
                                cJSON_AddStringToObject(cjson_result, "roomName", registered_device->properties->ezlopi_cloud.room_name);
                                cJSON_AddFalseToObject(cjson_result, "serviceNotification");
                                cJSON_AddFalseToObject(cjson_result, "userNotification");
                                cJSON_AddNullToObject(cjson_result, "notifications");
                                cJSON_AddStringToObject(cjson_result, "name", registered_device->properties->ezlopi_cloud.item_name);
                                registered_device->device->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, registered_device->properties, cjson_result, registered_device->user_arg);
                                cJSON_AddStringToObject(cjson_result, "valueType", registered_device->properties->ezlopi_cloud.value_type);
                            }

                            string_response = cJSON_Print(cjson_response);
                            if (string_response)
                            {
                                TRACE_B("'hub.items.update' response: %s", string_response);
                                cJSON_Minify(string_response);
                            }

                            cJSON_Delete(cjson_response);
                        }

                        break;
                    }
                }

                registered_device = registered_device->next;
            }
        }

        cJSON_Delete(cjson_request);
    }

    return string_response;
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
                cJSON_AddStringToObject(cjson_response, "msg_subclass", method_hub_item_updated);
                cJSON_AddNumberToObject(cjson_response, "msg_id", web_provisioning_get_message_count());
                cJSON_AddStringToObject(cjson_response, "id", "ui_broadcast");
                cJSON *cjson_result = cJSON_AddObjectToObject(cjson_response, "result");
                if (cjson_result)
                {
                    char tmp_string[64];
                    snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_device->properties->ezlopi_cloud.item_id);
                    cJSON_AddStringToObject(cjson_result, "_id", tmp_string);
                    snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_device->properties->ezlopi_cloud.device_id);
                    cJSON_AddStringToObject(cjson_result, "deviceId", tmp_string);
                    cJSON_AddStringToObject(cjson_result, "deviceName", registered_device->properties->ezlopi_cloud.device_name);
                    cJSON_AddStringToObject(cjson_result, "deviceCategory", registered_device->properties->ezlopi_cloud.category);
                    cJSON_AddStringToObject(cjson_result, "deviceSubcategory", registered_device->properties->ezlopi_cloud.subcategory);
                    cJSON_AddStringToObject(cjson_result, "roomName", registered_device->properties->ezlopi_cloud.room_name);
                    cJSON_AddFalseToObject(cjson_result, "serviceNotification");
                    cJSON_AddFalseToObject(cjson_result, "userNotification");
                    cJSON_AddNullToObject(cjson_result, "notifications");
                    cJSON_AddStringToObject(cjson_result, "name", registered_device->properties->ezlopi_cloud.item_name);
                    registered_device->device->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, registered_device->properties, cjson_result, registered_device->user_arg);
                    cJSON_AddStringToObject(cjson_result, "valueType", registered_device->properties->ezlopi_cloud.value_type);
                }
            }
        }
    }

    return cjson_response;
}
