#include <string.h>
#include "trace.h"
#include "room.h"

#include "cJSON.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_devices_list.h"

void room_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result_array = cJSON_AddArrayToObject(cj_response, ezlopi_result_str);
    if (cjson_result_array)
    {
        l_ezlopi_device_t *devices = ezlopi_device_get_head();
        while (NULL != devices)
        {
            cJSON *cjson_room_info = cJSON_CreateObject();
            if (cjson_room_info)
            {
                cJSON_AddStringToObject(cjson_room_info, ezlopi__id_str, ezlopi__str);
                cJSON_AddStringToObject(cjson_room_info, ezlopi_name_str, ezlopi__str);

                if (!cJSON_AddItemToArray(cjson_result_array, cjson_room_info))
                {
                    cJSON_Delete(cjson_room_info);
                }
            }

            devices = devices->next;
        }
    }
}
