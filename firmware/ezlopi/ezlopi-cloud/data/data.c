#include <string.h>

#include "cJSON.h"
#include "data.h"
#include "trace.h"
#include "frozen.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_methods_str.h"

static const char *data_list_start = "{\"method\":\"hub.data.list\",\"msg_id\":%d,\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{\"settings\":{";
static const char *data_list_cont = "\"first_start\":{\"value\": 0}";
static const char *data_list_end = "}},\"sender\":%.*s}";

#if 0
char *data_list_v2(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    char *string_response = NULL;
    cJSON *cjson_request = cJSON_ParseWithLength(payload, len);

    if (cjson_request)
    {
        cJSON *id = cJSON_GetObjectItem(cjson_request, ezlopi_id_str);
        cJSON *sender = cJSON_GetObjectItem(cjson_request, ezlopi_sender_str);

        cJSON *cjson_response = cJSON_CreateObject();
        if (cjson_response)
        {
            cJSON_AddStringToObject(cjson_response, ezlopi_key_method_str, method_hub_data_value_list_request);
            cJSON_AddNumberToObject(cjson_response, ezlopi_msg_id_str, msg_count);
            cJSON_AddStringToObject(cjson_response, ezlopi_id_str, id ? (id->valuestring ? id->valuestring : "") : "");
            cJSON_AddStringToObject(cjson_response, ezlopi_sender_str, sender ? (sender->valuestring ? sender->valuestring : "{}") : "{}");
            cJSON_AddNullToObject(cjson_response, "error");

            cJSON *cjson_result = cJSON_CreateObject();
            if (cjson_result)
            {
                cJSON *cjson_devices_array = cJSON_CreateArray();
                if (cjson_devices_array)
                {

                    l_ezlopi_configured_devices_t *registered_devices = ezlopi_devices_list_get_configured_items();
                    int dev_idx = 0;
                    while (NULL != registered_devices)
                    {
                        if (NULL != registered_devices->properties)
                        {
                            cJSON *cjson_properties = cJSON_CreateObject();
                            if (cjson_properties)
                            {
                                char tmp_string[64];
                                snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_devices->properties->ezlopi_cloud.device_id);
                                cJSON_AddStringToObject(cjson_properties, "_id", tmp_string);
                                cJSON_AddStringToObject(cjson_properties, "deviceTypeId", ezlopi_ezlopi_str);
                                cJSON_AddStringToObject(cjson_properties, "parentDeviceId", "");
                                cJSON_AddStringToObject(cjson_properties, "category", registered_devices->properties->ezlopi_cloud.category);
                                cJSON_AddStringToObject(cjson_properties, "subcategory", registered_devices->properties->ezlopi_cloud.subcategory);
                                cJSON_AddBoolToObject(cjson_properties, "batteryPowered", registered_devices->properties->ezlopi_cloud.battery_powered);
                                cJSON_AddStringToObject(cjson_properties, "name", registered_devices->properties->ezlopi_cloud.device_name);
                                cJSON_AddStringToObject(cjson_properties, "type", registered_devices->properties->ezlopi_cloud.device_type);
                                cJSON_AddBoolToObject(cjson_properties, "reachable", registered_devices->properties->ezlopi_cloud.reachable);
                                cJSON_AddBoolToObject(cjson_properties, "persistent", true);
                                cJSON_AddBoolToObject(cjson_properties, "serviceNotification", false);
                                cJSON_AddBoolToObject(cjson_properties, "armed", false);
                                snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_devices->properties->ezlopi_cloud.room_id);
                                cJSON_AddStringToObject(cjson_properties, "roomId", tmp_string);
                                cJSON_AddStringToObject(cjson_properties, "security", "");
                                cJSON_AddBoolToObject(cjson_properties, "ready", true);
                                cJSON_AddStringToObject(cjson_properties, "status", "synced");
                                cJSON_AddStringToObject(cjson_properties, "info", "{}");

                                if (!cJSON_AddItemToArray(cjson_devices_array, cjson_properties))
                                {
                                    cJSON_Delete(cjson_properties);
                                }
                            }
                        }

                        registered_devices = registered_devices->next;
                    }

                    if (!cJSON_AddItemToObjectCS(cjson_result, "devices", cjson_devices_array))
                    {
                        cJSON_Delete(cjson_devices_array);
                    }
                }

                if (!cJSON_AddItemToObjectCS(cjson_response, ezlopi_result, cjson_result))
                {
                    cJSON_Delete(cjson_result);
                }
            }

            string_response = cJSON_Print(cjson_response);
            if (string_response)
            {
                printf("'hub.items.list' response:\r\n%s\r\n", string_response);
                cJSON_Minify(string_response);
            }

            cJSON_Delete(cjson_response);
        }

        cJSON_Delete(cjson_request);
    }

    return string_response;
}
#endif

void data_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);
    if (cjson_result)
    {
        cJSON *cj_settings = cJSON_AddObjectToObject(cjson_result, "settings");
        if (cj_settings)
        {
            l_ezlopi_configured_devices_t *registered_devices = ezlopi_devices_list_get_configured_items();
            while (NULL != registered_devices)
            {
                if (NULL != registered_devices->properties)
                {
                    cJSON *cj_first_start = cJSON_AddObjectToObject(cj_settings, "first_start");
                    if (cj_first_start)
                    {
                        cJSON_AddNumberToObject(cj_first_start, "value", 0);
                    }
#warning "WARNING: Remove break from here!"
                    break;
                }

                registered_devices = registered_devices->next;
            }
        }
    }
}

#if 0
static cJSON *ezlopi_cloud_data_create_device_list(void)
{
    cJSON *cjson_device_list = cJSON_CreateObject();

    if (cjson_device_list)
    {
        cJSON_AddNumberToObject(cjson_device_list, "ids", 1234);
    }

    return cjson_device_list;
}

static cJSON *ezlopi_cloud_data_create_settings_list(void)
{
    cJSON *cjson_device_list = cJSON_CreateObject();

    if (cjson_device_list)
    {
        cJSON_AddNumberToObject(cjson_device_list, "ids", 1234);
    }

    return cjson_device_list;
}

static cJSON *ezlopi_cloud_data_list_settings(l_ezlopi_configured_devices_t *ezlopi_device)
{
    cJSON *cjson_settings = NULL;
    if (ezlopi_device)
    {
        cjson_settings = cJSON_CreateObject();
        if (cjson_settings)
        {
            char tmp_string[64];
            snprintf(tmp_string, sizeof(tmp_string), "%08x", ezlopi_device->properties->ezlopi_cloud.device_id);
            cJSON_AddStringToObject(cjson_settings, "_id", tmp_string);
            cJSON_AddStringToObject(cjson_settings, "deviceTypeId", ezlopi_ezlopi_str);
            cJSON_AddStringToObject(cjson_settings, "parentDeviceId", "");
            cJSON_AddStringToObject(cjson_settings, "category", ezlopi_device->properties->ezlopi_cloud.category);
            cJSON_AddStringToObject(cjson_settings, "subcategory", ezlopi_device->properties->ezlopi_cloud.subcategory);
            cJSON_AddBoolToObject(cjson_settings, "batteryPowered", ezlopi_device->properties->ezlopi_cloud.battery_powered);
            cJSON_AddStringToObject(cjson_settings, "name", ezlopi_device->properties->ezlopi_cloud.device_name);
            cJSON_AddStringToObject(cjson_settings, "type", ezlopi_device->properties->ezlopi_cloud.device_type);
            cJSON_AddBoolToObject(cjson_settings, "reachable", ezlopi_device->properties->ezlopi_cloud.reachable);
            cJSON_AddBoolToObject(cjson_settings, "persistent", true);
            cJSON_AddBoolToObject(cjson_settings, "serviceNotification", false);
            cJSON_AddBoolToObject(cjson_settings, "armed", false);
            snprintf(tmp_string, sizeof(tmp_string), "%08x", ezlopi_device->properties->ezlopi_cloud.room_id);
            cJSON_AddStringToObject(cjson_settings, "roomId", tmp_string);
            cJSON_AddStringToObject(cjson_settings, "security", "");
            cJSON_AddBoolToObject(cjson_settings, "ready", true);
            cJSON_AddStringToObject(cjson_settings, "status", "synced");
            cJSON_AddStringToObject(cjson_settings, "info", "{}");
        }
    }

    return cjson_settings;
}
#endif