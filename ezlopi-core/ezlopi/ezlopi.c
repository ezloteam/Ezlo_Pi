#include "esp_netif.h"
#include "esp_event.h"

#include "trace.h"

#include "ezlopi.h"
#include "ezlopi_wifi.h"
#include "ezlopi_devices.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_event_queue.h"
#include "ezlopi_nvs.h"
#include "ezlopi_timer.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_system_info.h"
#include "ezlopi_ethernet.h"
#include "ezlopi_event_group.h"
#include "ezlopi_ping.h"

static void ezlopi_initialize_devices(void);

void ezlopi_init(void)
{
    // Init memories
    ezlopi_nvs_init();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
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

    // ezlopi_initialize_settings();
    // vTaskDelay(10);

    ezlopi_wifi_initialize();
    vTaskDelay(10);
#if (EZLOPI_DEVICE_TYPE_AMBIENT_TRACKER_PRO == EZLOPI_DEVICE_TYPE)
    ezlopi_ethernet_init();
#endif
    uint32_t boot_count = ezlopi_system_info_get_boot_count();
#if 0
    if (boot_count > 1)
    {
        ezlopi_wifi_connect_from_nvs();
    }
    else
    {
        ezlopi_wifi_connect_from_id_bin();
    }
#endif

    ezlopi_wifi_connect_from_id_bin();
    ezlopi_nvs_set_boot_count(boot_count + 1);

    ezlopi_event_queue_init();
    // #if CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32
    //     ezlopi_timer_start_50ms();
    // #else
    // ezlopi_timer_start_1000ms();
    // #endif
    ezlopi_timer_start_1000ms();

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