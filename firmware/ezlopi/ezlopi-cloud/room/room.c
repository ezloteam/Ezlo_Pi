#include <string.h>
#include "trace.h"
#include "room.h"
#include "frozen.h"
#include "cJSON.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_devices_list.h"

void room_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result_array = cJSON_AddArrayToObject(cj_response, ezlopi_result);
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
    }
}
