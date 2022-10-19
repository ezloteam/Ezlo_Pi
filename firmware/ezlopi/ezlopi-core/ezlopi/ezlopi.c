#include "ezlopi.h"
#include "ezlopi_wifi.h"
#include "ezlopi_devices.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_event_queue.h"
#include "ezlopi_nvs.h"

void ezlopi_init(void)
{
    // Init memories
    ezlopi_nvs_init();
    ezlopi_factory_info_init();

    // Init devices
    ezlopi_device_init();

    ezlopi_wifi_initialize();
    ezlopi_wifi_connect_from_nvs();
    ezlopi_event_queue_init();
}