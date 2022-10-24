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
            cJSON_AddItemReferenceToObject(cjson_response, ezlopi_id_str, id);
            cJSON_AddItemReferenceToObject(cjson_response, ezlopi_sender_str, sender);
            cJSON_AddNullToObject(cjson_response, "error");

            cJSON *cjson_result = cJSON_CreateObject();
            if (cjson_result)
            {
                cJSON *cjson_devices_array = cJSON_CreateArray();
                if (cjson_devices_array)
                {
                    s_ezlopi_device_t *ezlopi_device_list = ezlopi_devices_list_get_list();
                    if (ezlopi_device_list)
                    {
                        int dev_idx = 0;
                        while (EZLOPI_SENSOR_NONE != ezlopi_device_list[dev_idx].id)
                        {
                            if (NULL != ezlopi_device_list[dev_idx].properties)
                            {
                                cJSON *cjson_properties = cJSON_CreateObject();
                                if (cjson_properties)
                                {
                                    char tmp_string[64];
                                    snprintf(tmp_string, sizeof(tmp_string), "%08x", ezlopi_device_list[dev_idx].properties->ezlopi_cloud.device_id);
                                    cJSON_AddStringToObject(cjson_properties, "_id", tmp_string);
                                    cJSON_AddStringToObject(cjson_properties, "deviceTypeId", ezlopi_ezlopi_str);
                                    cJSON_AddStringToObject(cjson_properties, "parentDeviceId", "");
                                    cJSON_AddStringToObject(cjson_properties, "category", ezlopi_device_list[dev_idx].properties->ezlopi_cloud.category);
                                    cJSON_AddStringToObject(cjson_properties, "subcategory", ezlopi_device_list[dev_idx].properties->ezlopi_cloud.subcategory);
                                    cJSON_AddBoolToObject(cjson_properties, "batteryPowered", ezlopi_device_list[dev_idx].properties->ezlopi_cloud.battery_powered);
                                    cJSON_AddStringToObject(cjson_properties, "name", ezlopi_device_list[dev_idx].properties->ezlopi_cloud.device_name);
                                    cJSON_AddStringToObject(cjson_properties, "type", ezlopi_device_list[dev_idx].properties->ezlopi_cloud.device_type);
                                    cJSON_AddBoolToObject(cjson_properties, "reachable", ezlopi_device_list[dev_idx].properties->ezlopi_cloud.reachable);
                                    cJSON_AddBoolToObject(cjson_properties, "persistent", true);
                                    cJSON_AddBoolToObject(cjson_properties, "serviceNotification", false);
                                    cJSON_AddBoolToObject(cjson_properties, "armed", false);
                                    snprintf(tmp_string, sizeof(tmp_string), "%08x", ezlopi_device_list[dev_idx].properties->ezlopi_cloud.room_id);
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

                            dev_idx++;
                        }
                    }

                    if (!cJSON_AddItemToObjectCS(cjson_result, "devices", cjson_devices_array))
                    {
                        cJSON_Delete(cjson_devices_array);
                    }
                }

                if (!cJSON_AddItemToObjectCS(cjson_response, "result", cjson_result))
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

static cJSON *ezlopi_cloud_data_list_settings(s_ezlopi_device_t *ezlopi_device)
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

char *data_list(const char *data, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    uint32_t buf_len = 4096;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {
        int sender_status = 0;
        struct json_token msg_id = JSON_INVALID_TOKEN;
        struct json_token sender = JSON_INVALID_TOKEN;
        s_ezlopi_device_t *devices_list = ezlopi_devices_list_get_list();

        if (devices_list)
        {
            json_scanf(data, len, "{id: %T}", &msg_id);
            sender_status = json_scanf(data, len, "{sender: %T}", &sender);

            snprintf(send_buf, buf_len, data_list_start, msg_count, msg_id.len, msg_id.ptr);

            int device_idx = 0;
            while (NULL != devices_list[device_idx].func)
            {
                if (NULL != devices_list[device_idx].properties)
                {
                    int len_b = strlen(send_buf);
                    snprintf(&send_buf[len_b], buf_len - len_b, "%s", data_list_cont); //, devices[i].device_id, devices[i].name);
#warning "WARNING: Remove break from here!"
                    break;
                }

                device_idx++;
            }
        }

        snprintf(&send_buf[strlen(send_buf)], buf_len - strlen(send_buf), data_list_end, sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");
        TRACE_B(">> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, strlen(send_buf), send_buf);
    }

    return send_buf;
}

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
