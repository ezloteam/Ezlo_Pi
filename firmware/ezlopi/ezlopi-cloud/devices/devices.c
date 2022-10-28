#include <string.h>

#include "devices.h"
#include "trace.h"
#include "frozen.h"
#include "devices_common.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_devices.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_methods_str.h"
#include "cJSON.h"

char *devices_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
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
            cJSON_AddStringToObject(cjson_response, ezlopi_key_method_str, method_hub_devices_list);
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
                                    // cJSON_AddStringToObject(cjson_properties, "info", "{}");
                                    cJSON_AddObjectToObject(cjson_properties, "info");

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
                printf("'%s' response:\r\n%s\r\n", method_hub_devices_list, string_response);
                cJSON_Minify(string_response);
            }

            cJSON_Delete(cjson_response);
        }

        cJSON_Delete(cjson_request);
    }

    return string_response;
}

#if 0
char *devices_settings_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    uint32_t buf_len = 256;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {
        memset(send_buf, 0, buf_len);
        struct json_token msg_id = JSON_INVALID_TOKEN;
        struct json_token sender = JSON_INVALID_TOKEN;

        json_scanf(payload, len, "{id:%T}", &msg_id);
        json_scanf(payload, len, "{sender:%T}", &sender);

        static const char *settings_list_frmt = "{\"id\":\"%.*s\",\"msg_id\":%u,\"method\":\"%.*s\",\"result\":{\"settings\":[]},\"sender\":%.*s,\"error\":null}";

        snprintf(send_buf, buf_len, settings_list_frmt,
                 msg_id.len, msg_id.ptr,
                 msg_count,
                 method->len, method->ptr,
                 sender.len ? sender.len : 2, sender.len ? sender.ptr : "{}");

        TRACE_B(">> WS Tx - '%.*s' [%d]\r\n%s", method->len, method->ptr, strlen(send_buf), send_buf);
    }

    return send_buf;
}

char *devices_name_set(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    uint32_t buf_len = 300;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {
        struct json_token msg_id = JSON_INVALID_TOKEN;
        struct json_token sender = JSON_INVALID_TOKEN;
        memset(send_buf, 0, buf_len);

        json_scanf(payload, len, "{id: %T}", &msg_id);
        json_scanf(payload, len, "{sender: %T}", &sender);

        snprintf(send_buf, buf_len, "{\"method\": \"%.*s\",\"msg_id\":%d,\"error\": null,\"id\": \"%.*s\",\"result\": {},\"sender\":%.*s}",
                 method->len, method->ptr,
                 msg_count,
                 msg_id.len, msg_id.ptr,
                 sender.len ? sender.len : 2, sender.len ? sender.ptr : "{}");

        TRACE_B(">> WS Tx - '%.*s' [%d]\r\n%s", method->len, method->ptr, strlen(send_buf), send_buf);
    }

    return send_buf;
}
#endif
