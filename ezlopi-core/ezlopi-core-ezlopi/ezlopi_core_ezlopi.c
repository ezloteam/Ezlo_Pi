#include "esp_event.h"

#include "EZLOPI_USER_CONFIG.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_ping.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_room.h"
#include "ezlopi_core_timer.h"
#include "ezlopi_core_modes.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_event_queue.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_scenes_scripts.h"
#include "ezlopi_core_scenes_expressions.h"
#include "ezlopi_core_mdns.h"

#ifdef CONFIG_EZPI_CORE_ENABLE_ETH
#include "ezlopi_core_ethernet.h"
#endif // CONFIG_EZPI_CORE_ENABLE_ETH

#include "ezlopi_hal_system_info.h"

static void ezlopi_initialize_devices_v3(void);

void ezlopi_init(void)
{


    // Init memories
    ezlopi_nvs_init();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    ezlopi_factory_info_v3_init();
    print_factory_info_v3();

    ezlopi_event_group_create();
    ezlopi_wifi_initialize();
    vTaskDelay(10);

    // Init devices
    ezlopi_device_prepare();
    vTaskDelay(10);
    ezlopi_initialize_devices_v3();
    vTaskDelay(10);

    ezlopi_core_modes_init();
    ezlopi_room_init();


#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS
    ezlopi_scenes_scripts_init();
    ezlopi_scenes_expressions_init();
    ezlopi_scenes_init_v2();
#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#ifdef CONFIG_EZPI_CORE_ENABLE_ETH
    ezlopi_ethernet_init();
#endif // CONFIG_EZPI_CORE_ENABLE_ETH

    uint32_t boot_count = ezlopi_system_info_get_boot_count();

    ezlopi_wifi_connect_from_id_bin();
    ezlopi_nvs_set_boot_count(boot_count + 1);

    ezlopi_event_queue_init();
    ezlopi_ping_init();
    ezlopi_timer_start_1000ms();

    EZPI_core_init_mdns();
}

static void ezlopi_initialize_devices_v3(void)
{
    int device_init_ret = 0;
    l_ezlopi_device_t* curr_device = ezlopi_device_get_head();
    while (curr_device)
    {
        l_ezlopi_item_t* curr_item = curr_device->items;
        while (curr_item)
        {
            if (curr_item->func)
            {
                if ((device_init_ret = curr_item->func(EZLOPI_ACTION_INITIALIZE, curr_item, NULL, NULL)) < 0)
                {
                    break;
                }
            }
            else
            {
                TRACE_E("Function is not defined!");
            }

            curr_item = curr_item->next;
        }

        if (device_init_ret < 0)
        {
            l_ezlopi_device_t* device_to_free = curr_device;
            curr_device = curr_device->next;
            device_to_free->next = NULL;
            ezlopi_device_free_device(device_to_free);
        }
        else
        {
            curr_device = curr_device->next;
        }
    }
}
