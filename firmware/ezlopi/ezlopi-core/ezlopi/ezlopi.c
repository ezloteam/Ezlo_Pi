#include "ezlopi.h"
#include "ezlopi_wifi.h"
#include "ezlopi_devices.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_event_queue.h"
#include "ezlopi_nvs.h"
#include "ezlopi_timer.h"
#include "ezlopi_devices_list.h"
#include "trace.h"
#include "ezlopi_system_info.h"
#include "ezlopi_ping.h"
#include "ezlopi_event_group.h"
#include "mac_uuid.h"

static void ezlopi_initialize_devices(void);

void ezlopi_init(void)
{
    // Init memories
    ezlopi_nvs_init();
    vTaskDelay(10);
    // ezlopi_factory_info_init();
    print_factory_info_v2();
    vTaskDelay(10);

    // Init devices
    ezlopi_event_group_create();
    ezlopi_device_prepare();
    vTaskDelay(10);
    ezlopi_initialize_devices();
    vTaskDelay(10);
    ezlopi_wifi_initialize();
    vTaskDelay(10);

    uint32_t boot_count = ezlopi_system_info_get_boot_count();
    // if (boot_count > 1)
    // {
    //     ezlopi_wifi_connect_from_nvs();
    // }
    // else
    // {
    //     ezlopi_wifi_connect_from_id_bin();
    // }
    ezlopi_wifi_connect_from_id_bin();
    ezlopi_nvs_set_boot_count(boot_count + 1);

    ezlopi_event_queue_init();
    ezlopi_timer_start_50ms();
    ezlopi_ping_init();
}

static void ezlopi_initialize_devices(void)
{
    l_ezlopi_configured_devices_t *registered_device = ezlopi_devices_list_get_configured_items();
    while (NULL != registered_device)
    {
        registered_device->device->func(EZLOPI_ACTION_INITIALIZE, registered_device->properties, NULL, NULL);
        registered_device = registered_device->next;
        vTaskDelay(1);
    }
}
