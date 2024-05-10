#include <string.h>

#include "ezlopi_cloud_favorite.h"
#include "ezlopi_util_trace.h"

#include "cjext.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_core_devices_list.h"

void favorite_list_v3(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cj_devices_req = NULL;
    cJSON* cj_items_req = NULL;
    // cJSON *cj_rules_req = NULL;

    cJSON* cj_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    cJSON* cj_param = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);

    if (cj_param && cj_param->valuestring)
    {
        cJSON* cj_request_list = cJSON_GetObjectItem(__FUNCTION__, cj_param, "request");
        if (cj_request_list && cj_request_list->valuestring)
        {
            int array_size = cJSON_GetArraySize(cj_request_list);
            for (int i = 0; i < array_size; i++)
            {
                cJSON* elem = cJSON_GetArrayItem(cj_request_list, i);
                if (elem && elem->valuestring)
                {
                    if (strstr("devices", elem->valuestring))
                    {
                        cj_devices_req = elem;
                    }
                    else if (strstr(ezlopi_items_str, elem->valuestring))
                    {
                        cj_items_req = elem;
                    }
                    else if (strstr("rules", elem->valuestring))
                    {
                        // cj_rules_req = elem;
                    }
                }
            }
        }
    }

    if (cj_result)
    {
        cJSON* cj_favorites = cJSON_AddObjectToObject(__FUNCTION__, cj_result, "favorites");
        if (cj_favorites)
        {
            cJSON* cj_device_list = cj_devices_req ? cJSON_AddArrayToObject(__FUNCTION__, cj_favorites, "devices") : NULL;
            // cJSON *cj_items_list = cj_items_req ? cJSON_AddArrayToObject(__FUNCTION__, cj_favorites, ezlopi_items_str) : NULL;
            // cJSON *cj_rules_list = cj_rules_req ? cJSON_AddArrayToObject(__FUNCTION__, cj_favorites, "rules") : NULL;

            l_ezlopi_device_t* curr_device = ezlopi_device_get_head();
            while (curr_device)
            {
                l_ezlopi_item_t* curr_item = curr_device->items;
                while (curr_item)
                {
                    #warning "Needs works here [Krishna]"
                        cJSON* cj_device = cJSON_CreateObject(__FUNCTION__);
                    if (cj_device)
                    {
                        cJSON_AddStringToObject(__FUNCTION__, cj_device, ezlopi__id_str, ezlopi__str);

                        if (!cJSON_AddItemToArray(cj_device_list, cj_device))
                        {
                            cJSON_Delete(__FUNCTION__, cj_device);
                        }
                    }
                    curr_item = curr_item->next;
                }

                curr_device = curr_device->next;
            }
        }
    }
}
