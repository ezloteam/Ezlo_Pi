#include <string.h>

#include "scenes.h"
#include "trace.h"
#include "frozen.h"
#include "cJSON.h"
#include "ezlopi_scenes.h"
#include "ezlopi_cloud_constants.h"

void scenes_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);
    if (cjson_result)
    {
        cJSON *cjson_scenes_array = cJSON_AddArrayToObject(cjson_result, "scenes");
        if (cjson_scenes_array)
        {
            cJSON *cj_scene = cJSON_CreateObject();
            if (cj_scene)
            {

                while (0)
                {
                    cJSON_AddStringToObject(cj_scene, "_id", );

                    if (!cJSON_AddItemToArray(cjson_scenes_array, cj_scene))
                    {
                        cJSON_Delete(cj_scene);
                    }
                }
            }
        }

#if 1
        cJSON *cjson_devices_array = cJSON_AddArrayToObject(cjson_result, "devices");
        if (cjson_devices_array)
        {
            l_ezlopi_device_t *curr_device = ezlopi_device_get_head();

            while (NULL != curr_device)
            {
                cJSON *cj_properties = cJSON_CreateObject();
                if (cj_properties)
                {
                    char tmp_string[64];
                    snprintf(tmp_string, sizeof(tmp_string), "%08x", curr_device->cloud_properties.device_id);
                    cJSON_AddStringToObject(cj_properties, "_id", tmp_string);
                    cJSON_AddStringToObject(cj_properties, "deviceTypeId", "ezlopi");
                    cJSON_AddStringToObject(cj_properties, "parentDeviceId", "");
                    cJSON_AddStringToObject(cj_properties, "category", curr_device->cloud_properties.category);
                    cJSON_AddStringToObject(cj_properties, "subcategory", curr_device->cloud_properties.subcategory);
                    cJSON_AddStringToObject(cj_properties, "gatewayId", "");
                    cJSON_AddBoolToObject(cj_properties, "batteryPowered", false);
                    cJSON_AddStringToObject(cj_properties, "name", curr_device->cloud_properties.device_name);
                    cJSON_AddStringToObject(cj_properties, "type", curr_device->cloud_properties.device_type);
                    cJSON_AddBoolToObject(cj_properties, "reachable", true);
                    cJSON_AddBoolToObject(cj_properties, "persistent", true);
                    cJSON_AddBoolToObject(cj_properties, "serviceNotification", false);
                    cJSON_AddBoolToObject(cj_properties, "armed", false);
                    cJSON_AddStringToObject(cj_properties, "roomId", "");
                    cJSON_AddStringToObject(cj_properties, "security", "");
                    cJSON_AddBoolToObject(cj_properties, "ready", true);
                    cJSON_AddStringToObject(cj_properties, "status", "synced");
                    cJSON_AddObjectToObject(cj_properties, "info");

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
#endif
    }
}
