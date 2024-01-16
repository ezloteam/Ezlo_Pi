#include "trace.h"
#include "items.h"
#include "settings.h"
#include "web_provisioning.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"

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
                    if (cj_response)
                    {
                        char *data_to_send = cJSON_Print(cj_response);
                        cJSON_Delete(cj_response);
                        if (data_to_send)
                        {
                            cJSON_Minify(data_to_send);
                            ret = web_provisioning_send_str_data_to_nma_websocket(data_to_send, TRACE_TYPE_D);
                            // ret = web_provisioning_send_to_nma_websocket(cj_response, TRACE_TYPE_D);
                            free(data_to_send);
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

    // if (item)
    {
        l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
        while (curr_device)
        {
            l_ezlopi_item_t *curr_item = curr_device->items;
            while (curr_item)
            {
                if (item_id == curr_item->cloud_properties.item_id)
                {
                    cJSON *cj_response = ezlopi_cloud_items_updated_from_devices_v3(curr_device, curr_item);
                    if (cj_response)
                    {
                        char *data_to_send = cJSON_Print(cj_response);
                        cJSON_Delete(cj_response);
                        if (data_to_send)
                        {
                            cJSON_Minify(data_to_send);
                            ret = web_provisioning_send_str_data_to_nma_websocket(data_to_send, TRACE_TYPE_D);
                            // ret = web_provisioning_send_to_nma_websocket(cj_response, TRACE_TYPE_D);
                            free(data_to_send);
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
                    if (cj_response)
                    {
                        char *data_to_send = cJSON_Print(cj_response);
                        cJSON_Delete(cj_response);
                        if (data_to_send)
                        {
                            cJSON_Minify(data_to_send);
                            ret = web_provisioning_send_str_data_to_nma_websocket(data_to_send, TRACE_TYPE_D);
                            free(data_to_send);
                            // ret = web_provisioning_send_to_nma_websocket(cj_response, TRACE_TYPE_B);
                        }
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
                    if (cj_response)
                    {
                        char *data_to_send = cJSON_Print(cj_response);
                        cJSON_Delete(cj_response);
                        if (data_to_send)
                        {
                            cJSON_Minify(data_to_send);
                            ret = web_provisioning_send_str_data_to_nma_websocket(data_to_send, TRACE_TYPE_D);
                            free(data_to_send);
                            // ret = web_provisioning_send_to_nma_websocket(cj_response, TRACE_TYPE_B);
                        }
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
