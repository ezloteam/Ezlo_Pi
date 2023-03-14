#include "items.h"
#include "web_provisioning.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"

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
                    char *response = ezlopi_cloud_items_updated_from_devices(registered_devices);
                    if (response)
                    {
                        ret = web_provisioning_send_to_nma_websocket(response);
                        free(response);
                    }
                }
            }

            registered_devices = registered_devices->next;
        }
    }

    return ret;
}