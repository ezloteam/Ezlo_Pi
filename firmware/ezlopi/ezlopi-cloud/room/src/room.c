#include <string.h>

#include "room.h"
#include "trace.h"
#include "frozen.h"
#include "cJSON.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_devices_list.h"

char *room_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
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
            cJSON_AddStringToObject(cjson_response, ezlopi_key_method_str, method_hub_room_list);
            cJSON_AddNumberToObject(cjson_response, ezlopi_msg_id_str, msg_count);
            cJSON_AddItemReferenceToObject(cjson_response, ezlopi_id_str, id);
            cJSON_AddItemReferenceToObject(cjson_response, ezlopi_sender_str, sender);
            cJSON_AddNullToObject(cjson_response, "error");

            cJSON *cjson_result_array = cJSON_CreateArray();
            if (cjson_result_array)
            {
                l_ezlopi_configured_devices_t *registered_device = ezlopi_devices_list_get_configured_items();
                while (NULL != registered_device)
                {
                    if (NULL != registered_device->properties)
                    {
                        cJSON *cjson_room_info = cJSON_CreateObject();
                        if (cjson_room_info)
                        {
                            char tmp_string[64];
                            snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_device->properties->ezlopi_cloud.room_id);
                            cJSON_AddStringToObject(cjson_room_info, "_id", tmp_string);
                            cJSON_AddStringToObject(cjson_room_info, "name", registered_device->properties->ezlopi_cloud.room_name);

                            if (!cJSON_AddItemToArray(cjson_result_array, cjson_room_info))
                            {
                                cJSON_Delete(cjson_room_info);
                            }
                        }
                    }

                    registered_device = registered_device->next;
                }

                if (!cJSON_AddItemToObjectCS(cjson_response, "result", cjson_result_array))
                {
                    cJSON_Delete(cjson_result_array);
                }
            }

            string_response = cJSON_Print(cjson_response);
            if (string_response)
            {
                TRACE_B("'%s' response:\r\n%s\r\n", method_hub_room_list, string_response);
                cJSON_Minify(string_response);
            }

            cJSON_Delete(cjson_response);
        }

        cJSON_Delete(cjson_request);
    }

    return string_response;
}

#if 0
char *room_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    uint32_t buf_len = 2048;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {
        int len_s = 0;
        int remove_comma = 0;
        memset(send_buf, 0, buf_len);

        s_device_properties_t *devices_list = devices_common_device_list();

        struct json_token msg_id = JSON_INVALID_TOKEN;
        struct json_token sender = JSON_INVALID_TOKEN;

        json_scanf(payload, len, "{id:%T}", &msg_id);
        int found_sender = json_scanf(payload, len, "{sender:%T}", &sender);
        snprintf(send_buf, buf_len, room_list_start, msg_count, msg_id.len, msg_id.ptr);

        for (int idx = 0; idx < MAX_DEV; idx++)
        {
            if (devices_list[idx].name[0])
            {
                remove_comma = 1;
                len_s = strlen(send_buf);
                snprintf(&send_buf[len_s], buf_len - len_s, room_list_room,
                         sizeof(devices_list[idx].roomId), devices_list[idx].roomId,
                         sizeof(devices_list[idx].roomName), devices_list[idx].roomName);
                len_s = strlen(send_buf);
                send_buf[len_s] = ',';
            }
        }

        if (remove_comma)
        {
            len_s = strlen(send_buf);
            send_buf[len_s - 1] = '\0';
        }

        len_s = strlen(send_buf);
        snprintf(&send_buf[len_s], buf_len - len_s, room_list_end, found_sender ? sender.len : 2, found_sender ? sender.ptr : "{}");

        TRACE_B(">> WS Tx - '%.*s' [%d]\r\n%s", method->len, method->ptr, strlen(send_buf), send_buf);
    }

    return send_buf;
}
#endif
