#include "trace.h"
#include "items.h"
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
                        ret = web_provisioning_send_to_nma_websocket(cj_response, TRACE_TYPE_D);
                        cJSON_Delete(cj_response);
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

#if 0 // v2.x
int ezlopi_device_value_updated_from_device(s_ezlopi_device_properties_t *device_properties)
{
    int ret = 0;

    if (device_properties)
    {
        l_ezlopi_configured_devices_t *registered_devices = ezlopi_devices_list_get_configured_items();
        while (registered_devices)
        {
            if (NULL != registered_devices->properties)
            {
                if (registered_devices->properties == device_properties)
                {
                    cJSON *cj_response = ezlopi_cloud_items_updated_from_devices(registered_devices);
                    if (cj_response)
                    {
                        ret = web_provisioning_send_to_nma_websocket(cj_response, TRACE_TYPE_D);
                        cJSON_Delete(cj_response);
                    }
                }
            }

            registered_devices = registered_devices->next;
        }
    }

    return ret;
}
#endif