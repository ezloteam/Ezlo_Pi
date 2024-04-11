#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_ezlopi_broadcast.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_settings.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_webprov.h"

int ezlopi_device_value_updated_from_device_v3(l_ezlopi_item_t *item)
{
    int ret = 0;

    if (item)
    {
        l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
        while (curr_device)
        {
            l_ezlopi_item_t *curr_item = curr_device->items;

            while (curr_item)
            {
                if (item == curr_item)
                {
                    cJSON *cj_response = ezlopi_cloud_items_updated_from_devices_v3(curr_device, item);
                    // CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);
                    if (cj_response)
                    {
                        if (!ezlopi_core_ezlopi_broadcast_add_to_queue(cj_response))
                        {
                            cJSON_Delete(cj_response);
                        }
                    }
                    break;
                }

                curr_item = curr_item->next;
            }

            curr_device = curr_device->next;
        }
    }

    return ret;
}

int ezlopi_device_value_updated_from_device_item_id_v3(uint32_t item_id)
{
    int ret = 0;

    l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
    while (curr_device)
    {
        l_ezlopi_item_t *curr_item = curr_device->items;

        while (curr_item)
        {
            if (item_id == curr_item->cloud_properties.item_id)
            {
                cJSON *cj_response = ezlopi_cloud_items_updated_from_devices_v3(curr_device, curr_item);
                CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);

                ret = ezlopi_core_ezlopi_broadcast_add_to_queue(cj_response);

                if (0 == ret)
                {
                    cJSON_Delete(cj_response);
                }

                break;
            }

            curr_item = curr_item->next;
        }

        curr_device = curr_device->next;
    }

    return ret;
}

int ezlopi_setting_value_updated_from_device_v3(l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    if (setting)
    {
        l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
        while (curr_device)
        {
            l_ezlopi_device_settings_v3_t *curr_setting = curr_device->settings;
            while (curr_setting)
            {
                if (setting == curr_setting)
                {
                    cJSON *cj_response = ezlopi_cloud_settings_updated_from_devices_v3(curr_device, setting);
                    CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);
                    ret = ezlopi_core_ezlopi_broadcast_add_to_queue(cj_response);

                    if (0 == ret)
                    {
                        cJSON_Delete(cj_response);
                    }
                    break;
                }
                curr_setting = curr_setting->next;
            }
            curr_device = curr_device->next;
        }
    }

    return ret;
}

int ezlopi_setting_value_updated_from_device_settings_id_v3(uint32_t setting_id)
{
    int ret = 0;

    // if (setting)
    {
        l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
        while (curr_device)
        {
            l_ezlopi_device_settings_v3_t *curr_setting = curr_device->settings;
            while (curr_setting)
            {
                if (setting_id == curr_setting->cloud_properties.setting_id)
                {
                    cJSON *cj_response = ezlopi_cloud_settings_updated_from_devices_v3(curr_device, curr_setting);
                    CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);
                    ret = ezlopi_core_ezlopi_broadcast_add_to_queue(cj_response);

                    if (0 == ret)
                    {
                        cJSON_Delete(cj_response);
                    }
                    break;
                }
                curr_setting = curr_setting->next;
            }
            curr_device = curr_device->next;
        }
    }

    return ret;
}

int ezlopi_network_update_wifi_scan_process(cJSON *network_array)
{
    int ret = 0;
    if (network_array)
    {
        cJSON *cj_response = cJSON_CreateObject();
        if (cj_response)
        {
            cJSON_AddStringToObject(cj_response, ezlopi_id_str, ezlopi_ui_broadcast_str);
            cJSON_AddStringToObject(cj_response, ezlopi_msg_subclass_str, method_hub_network_wifi_scan_progress);
            cJSON_AddNumberToObject(cj_response, ezlopi_msg_id_str, ezlopi_service_web_provisioning_get_message_count());

            cJSON *result = cJSON_AddObjectToObject(cj_response, "result");
            if (result)
            {
                cJSON_AddStringToObject(result, "interfaceId", "wlan0");
                cJSON_AddStringToObject(result, "status", "process");
                cJSON_AddItemToObject(result, "networks", network_array);
            }
            else
            {
                ret = 1;
            }

            CJSON_TRACE("----------------- broadcasting - cj_response", cj_response);
            ret = ezlopi_core_ezlopi_broadcast_add_to_queue(cj_response);

            if (0 == ret)
            {
                cJSON_Delete(cj_response);
            }
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }
    return ret;
}
