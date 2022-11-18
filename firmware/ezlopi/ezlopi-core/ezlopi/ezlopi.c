#include "ezlopi.h"
#include "ezlopi_wifi.h"
#include "ezlopi_devices.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_event_queue.h"
#include "ezlopi_nvs.h"
#include "ezlopi_timer.h"
#include "ezlopi_devices_list.h"
#include "trace.h"

static void ezlopi_initialize_devices(void);

void ezlopi_init(void)
{
    // Init memories
    ezlopi_nvs_init();
    // ezlopi_factory_info_init();

    // // Init devices
    // ezlopi_device_prepare();
    // ezlopi_initialize_devices();

    // ezlopi_wifi_initialize();
    // ezlopi_wifi_connect_from_nvs();
    // ezlopi_event_queue_init();

    // ezlopi_timer_start_50ms();
}

static void ezlopi_initialize_devices(void)
{
    l_ezlopi_configured_devices_t *registered_device = ezlopi_devices_list_get_configured_items();
    while (NULL != registered_device)
    {
        registered_device->device->func(EZLOPI_ACTION_INITIALIZE, registered_device->properties, NULL, NULL);
        registered_device = registered_device->next;
    }
}