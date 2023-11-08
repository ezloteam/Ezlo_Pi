#include <string.h>

#include "favorite.h"
#include "trace.h"


#include "cJSON.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_devices_list.h"

void favorite_list_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cj_devices_req = NULL;
    cJSON *cj_items_req = NULL;
    cJSON *cj_rules_req = NULL;

    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    cJSON *cj_param = cJSON_GetObjectItem(cj_request, ezlopi_params_str);

    if (cj_param && cj_param->valuestring)
    {
        cJSON *cj_request_list = cJSON_GetObjectItem(cj_param, "request");
        if (cj_request_list && cj_request_list->valuestring)
        {
            int array_size = cJSON_GetArraySize(cj_request_list);
            for (int i = 0; i < array_size; i++)
            {
                cJSON *elem = cJSON_GetArrayItem(cj_request_list, i);
                if (elem && elem->valuestring)
                {
                    if (strstr("devices", elem->valuestring))
                    {
                        cj_devices_req = elem;
                    }
                    else if (strstr("items", elem->valuestring))
                    {
                        cj_items_req = elem;
                    }
                    else if (strstr("rules", elem->valuestring))
                    {
                        cj_rules_req = elem;
                    }
                }
            }
        }
    }

    if (cj_result)
    {
        cJSON *cj_favorites = cJSON_AddObjectToObject(cj_result, "favorites");
        if (cj_favorites)
        {
            cJSON *cj_device_list = cj_devices_req ? cJSON_AddArrayToObject(cj_favorites, "devices") : NULL;
            cJSON *cj_items_list = cj_items_req ? cJSON_AddArrayToObject(cj_favorites, "items") : NULL;
            cJSON *cj_rules_list = cj_rules_req ? cJSON_AddArrayToObject(cj_favorites, "rules") : NULL;

            l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
            while (curr_device)
            {
                l_ezlopi_item_t *curr_item = curr_device->items;
                while (curr_item)
                {
#warning "Needs works here"
                    cJSON *cj_device = cJSON_CreateObject();
                    if (cj_device)
                    {
                        cJSON_AddStringToObject(cj_device, "_id", "");

                        if (!cJSON_AddItemToArray(cj_device_list, cj_device))
                        {
                            cJSON_Delete(cj_device);
                        }
                    }
                    curr_item = curr_item->next;
                }

                curr_device = curr_device->next;
            }
        }
    }
}

#if 0 // v2.x
void favorite_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_favorites = cJSON_AddObjectToObject(cj_result, "favorites");
        if (cj_favorites)
        {
            cJSON *cj_device_list = cJSON_AddArrayToObject(cj_favorites, "devices");
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
        }
    }
}
#endif
