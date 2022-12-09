#include <string.h>

#include "favorite.h"
#include "trace.h"
#include "frozen.h"

#include "cJSON.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_devices_list.h"

const char *favorite_1_start = "{\"method\":\"hub.favorite.list\",\"msg_id\":%d,\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{\"favorites\":{\"devices\":[";
const char *favorite_1_devs = "{\"_id\":\"%.*s\"}";
const char *favorite_1_end = "]}},\"sender\":%.*s}";

char *favorite_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
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
            cJSON_AddStringToObject(cjson_response, ezlopi_key_method_str, method_hub_favorite_list);
            cJSON_AddNumberToObject(cjson_response, ezlopi_msg_id_str, msg_count);
            cJSON_AddItemReferenceToObject(cjson_response, ezlopi_id_str, id);
            cJSON_AddItemReferenceToObject(cjson_response, ezlopi_sender_str, sender);
            cJSON_AddNullToObject(cjson_response, "error");

            cJSON *cjson_result = cJSON_CreateObject();
            if (cjson_result)
            {
                cJSON *cjson_favorites = cJSON_CreateObject();
                if (cjson_favorites)
                {
                    cJSON *cjson_devices_array = cJSON_CreateArray();
                    if (cjson_devices_array)
                    {
                        l_ezlopi_configured_devices_t *registered_devices = ezlopi_devices_list_get_configured_items();
                        while (NULL != registered_devices)
                        {
                            if (NULL != registered_devices->properties)
                            {
                                cJSON *cjson_room_info = cJSON_CreateObject();
                                if (cjson_room_info)
                                {
                                    // char tmp_string[64];
                                    // snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_devices->properties->ezlopi_cloud.room_id);
                                    // cJSON_AddStringToObject(cjson_room_info, "_id", tmp_string);
                                    cJSON_AddStringToObject(cjson_room_info, "_id", "");
                                    cJSON_AddStringToObject(cjson_room_info, "name", registered_devices->properties->ezlopi_cloud.room_name);

                                    if (!cJSON_AddItemToArray(cjson_devices_array, cjson_room_info))
                                    {
                                        cJSON_Delete(cjson_room_info);
                                    }
                                }
                            }

                            registered_devices = registered_devices->next;
                        }

                        if (!cJSON_AddItemToObjectCS(cjson_favorites, "devices", cjson_devices_array))
                        {
                            cJSON_Delete(cjson_devices_array);
                        }
                    }

                    if (!cJSON_AddItemToObject(cjson_result, "favorites", cjson_favorites))
                    {
                        cJSON_Delete(cjson_favorites);
                    }
                }

                if (!cJSON_AddItemToObject(cjson_response, "result", cjson_result))
                {
                    cJSON_Delete(cjson_result);
                }
            }

            string_response = cJSON_Print(cjson_response);
            if (string_response)
            {
                TRACE_B("'%s' response:\r\n%s", method_hub_favorite_list, string_response);
                cJSON_Minify(string_response);
            }

            cJSON_Delete(cjson_response);
        }

        cJSON_Delete(cjson_request);
    }

    return string_response;
}

#if 0
char *favorite_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    uint32_t buf_len = 2048;
    char *send_buf = (char *)malloc(buf_len);

    if (send_buf)
    {
        memset(send_buf, 0, buf_len);
        s_device_properties_t *devices = devices_common_device_list();

        struct json_token msg_id = JSON_INVALID_TOKEN;
        json_scanf(payload, len, "{id: %T}", &msg_id);

        struct json_token sender = JSON_INVALID_TOKEN;
        int sender_status = json_scanf(payload, len, "{sender: %T}", &sender);

        snprintf(send_buf, buf_len, favorite_1_start, msg_count, msg_id.len, msg_id.ptr);

        if (devices[0].name[0])
        {
            for (int i = 0; i < MAX_DEV; i++)
            {
                int len_b = strlen(send_buf);
                snprintf(&send_buf[len_b], buf_len - len_b, favorite_1_devs,
                         sizeof(devices[i].device_id), devices[i].device_id);
                if (i < MAX_DEV - 1)
                {
                    if (devices[i + 1].name[0])
                    {
                        len_b = strlen(send_buf);
                        send_buf[len_b] = ',';
                        send_buf[len_b + 1] = 0;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        int len_b = strlen(send_buf);
        snprintf(&send_buf[len_b], buf_len - len_b, favorite_1_end, sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");

        TRACE_B(">>>>>>>>>>> WS Tx - '%.*s' [%d]\n\r%s", method->len, method->ptr, strlen(send_buf), send_buf);
    }

    return send_buf;
}
#endif
