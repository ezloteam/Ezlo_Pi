#include <string.h>
#include "trace.h"
#include "room.h"
#include "frozen.h"
#include "cJSON.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_devices_list.h"

cJSON *room_list(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    cJSON *cjson_response = cJSON_CreateObject();
    cJSON *cjson_request = cJSON_ParseWithLength(payload, len);

    if (cjson_request)
    {
        cJSON *id = cJSON_GetObjectItem(cjson_request, ezlopi_id_str);
        cJSON *sender = cJSON_GetObjectItem(cjson_request, ezlopi_sender_str);

        if (cjson_response)
        {
            cJSON_AddStringToObject(cjson_response, ezlopi_key_method_str, method_hub_room_list);
            cJSON_AddNumberToObject(cjson_response, ezlopi_msg_id_str, msg_count);
            cJSON_AddStringToObject(cjson_response, ezlopi_id_str, id ? (id->valuestring ? id->valuestring : "") : "");
            cJSON_AddStringToObject(cjson_response, ezlopi_sender_str, sender ? (sender->valuestring ? sender->valuestring : "{}") : "{}");
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
                            cJSON_AddStringToObject(cjson_room_info, "_id", "");
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
        }

        cJSON_Delete(cjson_request);
    }

    return cjson_response;
}
