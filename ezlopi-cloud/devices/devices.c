#include <string.h>

#include "devices.h"
#include "trace.h"

#include "ezlopi_factory_info.h"
#include "ezlopi_devices.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_cloud_methods_str.h"
#include "cJSON.h"

void devices_list_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON *cjson_devices_array = cJSON_AddArrayToObject(cjson_result, "devices");
        if (cjson_devices_array)
        {
            l_ezlopi_device_t *curr_device = ezlopi_device_get_head();

            while (NULL != curr_device)
            {
                cJSON *cj_properties = ezlopi_device_create_device_table_from_prop(curr_device);
                if (cj_properties)
                {
                    if (!cJSON_AddItemToArray(cjson_devices_array, cj_properties))
                    {
                        cJSON_Delete(cj_properties);
                    }
                }

                curr_device = curr_device->next;
            }
        }
        else
        {
            TRACE_E("Failed to create devices-array");
        }
    }
    else
    {
        TRACE_E("Failed to create 'result'");
    }
}
