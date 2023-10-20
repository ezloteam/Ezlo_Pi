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
#include "ezlopi_ping.h"
#include "ezlopi_event_group.h"
#include "ezlopi_ethernet.h"
#include "ezlopi_scenes_v2.h"

static void ezlopi_initialize_devices_v3(void);

void ezlopi_init(void)
{
    // Init memories
    ezlopi_nvs_init();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    vTaskDelay(10);
    ezlopi_factory_info_v2_init();
    print_factory_info_v2();
    vTaskDelay(10);

    // Init devices
    ezlopi_event_group_create();
    ezlopi_device_prepare();
    vTaskDelay(10);
    ezlopi_wifi_initialize();
    vTaskDelay(10);
    ezlopi_initialize_devices_v3();
    vTaskDelay(10);
    ezlopi_scenes_v2_init();

    // ezlopi_ethernet_init();

    uint32_t boot_count = ezlopi_system_info_get_boot_count();

    ezlopi_wifi_connect_from_id_bin();
    ezlopi_nvs_set_boot_count(boot_count + 1);

    ezlopi_event_queue_init();
    ezlopi_timer_start_1000ms();
    ezlopi_ping_init();
}

static void ezlopi_initialize_devices_v3(void)
{
    l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
    while (curr_device)
    {
        l_ezlopi_item_t *curr_item = curr_device->items;
        while (curr_item)
        {
            if (curr_item->func)
            {
                curr_item->func(EZLOPI_ACTION_INITIALIZE, curr_item, NULL, NULL);
            }
            else
            {
                TRACE_E("Function is not defined!");
            }
            curr_item = curr_item->next;
        }
        curr_device = curr_device->next;
    }
}
