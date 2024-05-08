#include <stdio.h>
#include <cjext.h>

#include <esp_system.h>
#include <driver/adc.h>
#include <driver/i2c.h>
#include <driver/gpio.h>
#include <mbedtls/config.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_ezlopi.h"
#include "ezlopi_service_ota.h"
#include "ezlopi_core_log.h"

#include "ezlopi_service_ble.h"
#include "ezlopi_service_uart.h"
#include "ezlopi_service_timer.h"
#include "ezlopi_service_modes.h"
#include "ezlopi_service_meshbot.h"
#include "ezlopi_service_gpioisr.h"
#include "ezlopi_service_webprov.h"
#include "ezlopi_service_ws_server.h"
#include "ezlopi_service_broadcast.h"
#include "ezlopi_service_led_indicator.h"

#include "pt.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_core_ble_config.h"

#include "EZLOPI_USER_CONFIG.h"

static void blinky(void* pv);

void app_main(void)
{

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    ezlopi_core_set_log_upcalls();
#endif  // CONFIG_EZPI_UTIL_TRACE_EN

#ifdef CONFIG_EZPI_ENABLE_LED_INDICATOR
    ezlopi_service_led_indicator_init();
#endif // CONFIG_EZPI_ENABLE_LED_INDICATOR

    gpio_install_isr_service(0);
    gpio_isr_service_init();

    ezlopi_init();

#if defined(CONFIG_EZPI_ENABLE_UART_PROVISIONING)
    EZPI_SERV_uart_init();
#endif

    timer_service_init();

#if defined(CONFIG_EZPI_BLE_ENABLE)
    ezlopi_ble_service_init();
#endif

#if defined(CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER) || defined(CONFIG_EZPI_WEBSOCKET_CLIENT)
    ezlopi_service_broadcast_init();
#endif

#if defined(CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER)
    ezlopi_service_ws_server_start();
#else // CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER
    ezlpi_service_ws_server_dummy();
#endif // CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER

#if defined(CONFIG_EZPI_WEBSOCKET_CLIENT)
    ezlopi_service_web_provisioning_init();
#endif

#ifdef CONFIG_EZPI_ENABLE_OTA
    ezlopi_service_ota_init();
#endif // CONFIG_EZPI_ENABLE_OTA

#if defined (CONFIG_EZPI_SERV_ENABLE_MODES)
    ezlopi_service_modes_init();
#endif

#if defined (CONFIG_EZPI_SERV_ENABLE_MESHBOTS)
    ezlopi_scenes_meshbot_init();
#endif


    TaskHandle_t ezlopi_main_blinky_task_handle = NULL;
#if defined(CONFIG_EZPI_HEAP_ENABLE)
    xTaskCreate(blinky, "blinky", 3 * EZLOPI_MAIN_BLINKY_TASK_DEPTH, NULL, 1, &ezlopi_main_blinky_task_handle);
    ezlopi_core_process_set_process_info(ENUM_EZLOPI_MAIN_BLINKY_TASK, &ezlopi_main_blinky_task_handle, 3 * EZLOPI_MAIN_BLINKY_TASK_DEPTH);
#else
    xTaskCreate(blinky, "blinky", EZLOPI_MAIN_BLINKY_TASK_DEPTH, NULL, 1, &ezlopi_main_blinky_task_handle);
    ezlopi_core_process_set_process_info(ENUM_EZLOPI_MAIN_BLINKY_TASK, &ezlopi_main_blinky_task_handle, EZLOPI_MAIN_BLINKY_TASK_DEPTH);
#endif

}

static void blinky(void* pv)
{
    uint32_t low_heap_start_time = xTaskGetTickCount();

    while (1)
    {
        float free_heap_kb = esp_get_free_heap_size() / 1024.0;

        trace_wb("----------------------------------------------");
        trace_wb("esp_get_free_heap_size - %.02f kB", free_heap_kb);
        trace_wb("esp_get_minimum_free_heap_size: %.02f kB", esp_get_minimum_free_heap_size() / 1024.0);

#ifdef CONFIG_EZPI_HEAP_ENABLE
        ezlopi_util_heap_trace(false);
#endif // CONFIG_EZPI_HEAP_ENABLE

        if (free_heap_kb <= 10)
        {
            TRACE_W("CRITICAL-WARNING: low heap detected..");

            if ((xTaskGetTickCount() - low_heap_start_time) > (15000 / portTICK_PERIOD_MS))
            {
                vTaskDelay(2000 / portTICK_RATE_MS);
                TRACE_E("CRITICAL-ERROR: low heap time-out detected!");
                // TRACE_W("Rebooting.....");
                // vTaskDelay(1000 / portTICK_PERIOD_MS);
                // esp_restart();
            }
        }
        else
        {
            low_heap_start_time = xTaskGetTickCount();
        }
        TRACE_D("----------------------------------------------");

#ifdef CONFIG_EZPI_HEAP_ENABLE
        ezlopi_util_heap_flush();
#endif // CONFIG_EZPI_HEAP_ENABLE

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
