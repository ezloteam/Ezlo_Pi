#include <string.h>

#include "favorite.h"
#include "trace.h"
#include "frozen.h"

#include "cJSON.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_devices_list.h"

void favorite_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result);
    if (cj_result)
    {
        cJSON *cj_favorites = cJSON_AddObjectToObject(cj_result, "favorites");
        if (cj_favorites)
        {
            cJSON *cj_device_list = cJSON_AddArrayToObject(cj_favorites, "devices");
#if 0
                if (cj_device_list)
                {
                    l_ezlopi_configured_devices_t *registered_devices = ezlopi_devices_list_get_configured_items();
                    while (NULL != registered_devices)
                    {
                        if (NULL != registered_devices->properties)
                        {
                            cJSON *cj_device = cJSON_CreateObject();
                            if (cj_device)
                            {
                                cJSON_AddStringToObject(cj_device, "_id", "");
                                cJSON_AddStringToObject(cj_device, "name", "");

                                if (!cJSON_AddItemToArray(cj_device_list, cj_device))
                                {
                                    cJSON_Delete(cj_device);
                                }
                            }
                        }

                        registered_devices = registered_devices->next;
                    }
                }
#endif
        }
    }
}
