/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_ezlopi.c
 * @brief   Function to initialize ezlopi modules and services
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "../../build/config/sdkconfig.h"

#include "esp_event.h"

#include "EZLOPI_USER_CONFIG.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_mdns.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_ping.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_room.h"
#include "ezlopi_core_modes.h"
#include "ezlopi_core_buffer.h"
#include "ezlopi_core_event_queue.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_device_group.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_scenes_scripts.h"
#include "ezlopi_core_scenes_expressions.h"
#include "ezlopi_core_setting_commands.h"
#include "ezlopi_core_ezlopi.h"
#include "ezlopi_core_log.h"
// #include "ezlopi_core_errors.h"

#ifdef CONFIG_EZPI_CORE_ETHERNET_EN
#include "ezlopi_core_ethernet.h"
#endif // CONFIG_EZPI_CORE_ETHERNET_EN

#include "ezlopi_hal_system_info.h"
#include "ezlopi_service_loop.h"
#include "ezlopi_service_system_temperature_sensor.h"
/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static void __device_loop(void *arg);
static void __EZPI_initialize_devices_v3(void);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
void EZPI_init(void)
{
    // Init memories
    EZPI_nvs_init();
    EZPI_core_setting_commands_read_settings();

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    EZPI_core_read_set_log_severities();
    // #warning "remove this in release"
    EZPI_core_read_set_log_severities_internal(ENUM_EZLOPI_TRACE_SEVERITY_TRACE);
#endif // CONFIG_EZPI_UTIL_TRACE_EN
    EZPI_uart_main_init();
#if defined(CONFIG_EZPI_WEBSOCKET_CLIENT) || defined(CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER)
    EZPI_core_buffer_init(CONFIG_EZPI_CORE_STATIC_BUFFER_SIZE); // allocate 10kB
#endif

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    EZPI_factory_info_v3_init();
    EZPI_print_factory_info_v3();
    EZPI_event_group_create();

#if defined(CONFIG_EZPI_ENABLE_WIFI)
    EZPI_wifi_initialize();
#endif

    vTaskDelay(10);
    // Init devices
    EZPI_device_prepare();
#if defined(CONFIG_EZPI_ENABLE_SYSTEM_TEMPERATURE)
    EZPI_system_temperature_device(EZLOPI_ACTION_PREPARE, NULL, NULL, NULL);
#endif // EZPI_ENABLE_SYSTEM_TEMPERATURE
    vTaskDelay(10);
    // Init device_groups
    EZPI_device_group_init();
    // Init item_groups
    EZPI_item_group_init();
    vTaskDelay(10);
    __EZPI_initialize_devices_v3();
    vTaskDelay(10);

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)
    EZPI_core_modes_init();
#endif

    EZPI_room_init();

#if defined(CONFIG_EZPI_SERV_ENABLE_MESHBOTS)
    EZPI_scenes_scripts_init();
    EZPI_scenes_expressions_init();
    EZPI_scenes_init_v2();
#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#if defined(CONFIG_EZPI_CORE_ENABLE_ETH)
    EZPI_ethernet_init();
#endif // CONFIG_EZPI_CORE_ETHERNET_EN

    EZPI_core_nvs_set_boot_count(EZPI_hal_system_info_get_boot_count() + 1);

#if defined(CONFIG_EZPI_ENABLE_WIFI)
    EZPI_core_wifi_connect_from_id_bin();
#endif

#if (defined(CONFIG_EZPI_ENABLE_WIFI) || defined(CONFIG_EZPI_CORE_ENABLE_ETH))
    EZPI_core_sntp_init();
#ifdef CONFIG_EZPI_ENABLE_PING
    EZPI_ping_init();
#endif // CONFIG_EZPI_ENABLE_PING
#endif

#ifdef CONFIG_EZPI_SERV_MDNS_EN
    EZPI_init_mdns();
#endif // CONFIG_EZPI_SERV_MDNS_EN

    EZPI_service_loop_add("core-device-loop", __device_loop, 1000, NULL);
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/
static l_ezlopi_device_t *__link_next_parent_id(uint32_t target_to_clear_parent_id)
{
    l_ezlopi_device_t *pre_devs = EZPI_core_device_get_head();
    while (pre_devs)
    {
        if ((NULL != pre_devs->next) &&
            pre_devs->next->cloud_properties.parent_device_id == 0 &&
            pre_devs->next->cloud_properties.device_id > target_to_clear_parent_id)
        {
            return (pre_devs->next);
        }
        pre_devs = pre_devs->next;
    }
    return NULL;
}

static void __EZPI_initialize_devices_v3(void)
{
    int device_init_ret = 0;
    l_ezlopi_device_t *curr_device = EZPI_core_device_get_head();

    while (curr_device)
    {

        TRACE_S("Device_id_curr_device : [0x%x] ", curr_device->cloud_properties.device_id);
        l_ezlopi_item_t *curr_item = curr_device->items;
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
        // TRACE_D("ret = %d", device_init_ret);
        if (0 > device_init_ret)
        {
            device_init_ret = 0;
            l_ezlopi_device_t *device_to_free = curr_device;

            if (NULL != curr_device->next &&
                curr_device->cloud_properties.parent_device_id == 0 &&
                curr_device->cloud_properties.device_id == curr_device->next->cloud_properties.parent_device_id)
            {
                /* if 'device_to_free' is parent_with_child_nodes */
                curr_device = __link_next_parent_id(curr_device->cloud_properties.device_id);
            }
            else
            {
                curr_device = curr_device->next;
            }

            EZPI_core_device_free_device(device_to_free);
        }
        else
        {
            curr_device = curr_device->next;
        }
    }
}

static void __device_loop(void *arg)
{
    static l_ezlopi_device_t *device_node;
    if (NULL == device_node)
    {
        device_node = EZPI_core_device_get_head();
    }
    else
    {
        if (device_node && device_node->items)
        {
            l_ezlopi_item_t *item_node = device_node->items;
            while (item_node)
            {
                if (item_node->func)
                {
                    item_node->func(EZLOPI_ACTION_NOTIFY_1000_MS, item_node, NULL, item_node->user_arg);
                    vTaskDelay(1 / portTICK_PERIOD_MS);
                }

                item_node = item_node->next;
            }
        }

        device_node = device_node->next;
    }
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
