#include <stdio.h>
#include <cjext.h>

#include <esp_system.h>
#include <driver/adc.h>
#include <driver/i2c.h>
#include <driver/gpio.h>
#include <mbedtls/config.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_heap_caps.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_ezlopi.h"
#include "ezlopi_service_ota.h"
#include "ezlopi_core_log.h"
#include "ezlopi_core_reset.h"
#include "ezlopi_core_setting_commands.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_factory_info.h"

#include "ezlopi_service_ble.h"
#include "ezlopi_service_otel.h"
#include "ezlopi_service_uart.h"
#include "ezlopi_service_loop.h"
#include "ezlopi_service_modes.h"
#include "ezlopi_service_meshbot.h"
#include "ezlopi_service_gpioisr.h"
#include "ezlopi_service_webprov.h"
#include "ezlopi_service_ws_server.h"
#include "ezlopi_service_broadcast.h"
#include "ezlopi_service_led_indicator.h"
#include "ezlopi_service_system_temperature_sensor.h"

#include "pt.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_core_ble_config.h"

#include "EZLOPI_USER_CONFIG.h"

static void __blinky(void *pv);

static void __print_mac_address(void)
{
    uint8_t __base_mac[6] = { 0, 0, 0, 0, 0, 0 };

    esp_read_mac(__base_mac, ESP_MAC_WIFI_STA);
}

void app_main(void)
{

#ifdef CONFIG_EZPI_UTIL_TRACE_EN
    EZPI_core_set_log_upcalls();
#endif // CONFIG_EZPI_UTIL_TRACE_EN

    __print_mac_address();

#ifdef CONFIG_EZPI_ENABLE_LED_INDICATOR
    EZPI_service_led_indicator_init();
#endif // CONFIG_EZPI_ENABLE_LED_INDICATOR

    gpio_install_isr_service(0);

    EZPI_service_loop_init();
    EZPI_service_gpioisr_init(); // this is time critical, Do not add to loop

    EZPI_init();

    EZPI_core_setting_commands_read_settings();

    EZPI_SERV_uart_init();

#if !defined(CONFIG_IDF_TARGET_ESP32) && !defined(CONFIG_IDF_TARGET_ESP32C3)
    EZPI_SERV_cdc_init();
#endif // NOT defined CONFIG_IDF_TARGET_ESP32 or CONFIG_IDF_TARGET_ESP32C3

#if defined(CONFIG_EZPI_BLE_ENABLE)
    EZPI_ble_service_init();
#endif

#if defined(CONFIG_EZPI_SERV_ENABLE_MODES)
    ezlopi_service_modes_init();
#endif

#if defined(CONFIG_EZPI_SERV_ENABLE_MESHBOTS)
    EZPI_scenes_meshbot_init();
#endif

    TaskHandle_t ezlopi_main_blinky_task_handle = NULL;

#if defined(CONFIG_EZPI_HEAP_ENABLE)
    xTaskCreate(__blinky, "blinky", 3 * EZLOPI_MAIN_BLINKY_TASK_DEPTH, NULL, 1, &ezlopi_main_blinky_task_handle);

#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_process_info(ENUM_EZLOPI_MAIN_BLINKY_TASK, &ezlopi_main_blinky_task_handle, 3 * EZLOPI_MAIN_BLINKY_TASK_DEPTH);
#endif
#else
    xTaskCreate(__blinky, "blinky", EZLOPI_MAIN_BLINKY_TASK_DEPTH, NULL, tskIDLE_PRIORITY + 2, &ezlopi_main_blinky_task_handle);

#if defined(CONFIG_FREERTOS_USE_TRACE_FACILITY)
    EZPI_core_process_set_process_info(ENUM_EZLOPI_MAIN_BLINKY_TASK, &ezlopi_main_blinky_task_handle, EZLOPI_MAIN_BLINKY_TASK_DEPTH);
#endif

    ezlopi_service_otel_init();
#endif

    EZPI_core_wait_for_wifi_to_connect(portMAX_DELAY);
#if defined(CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER) || defined(CONFIG_EZPI_WEBSOCKET_CLIENT)
    EZPI_service_broadcast_init();
#endif

#if defined(CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER)
    EZPI_service_ws_server_start();
#else  // CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER
    EZPI_service_ws_server_dummy();
#endif // CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER

#if defined(CONFIG_EZPI_WEBSOCKET_CLIENT)
    EZPI_service_web_provisioning_init();
#endif

#ifdef CONFIG_EZPI_ENABLE_OTA
    EZPI_service_ota_init();
#endif // CONFIG_EZPI_ENABLE_OTA
}

static void __blinky(void *pv)
{
    uint32_t low_heap_start_time = xTaskGetTickCount();

    while (1)
    {
        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t watermark_heap = esp_get_minimum_free_heap_size();
        // uint32_t free_heap_internal = esp_get_free_internal_heap_size();

        uint32_t total_heap_size = heap_caps_get_total_size(MALLOC_CAP_DEFAULT);

        TRACE_I("----------------------------------------------");
        TRACE_W("Total heap:                %d B   %.4f KB", total_heap_size, total_heap_size / 1024.0);
        TRACE_W("Free Heap Size:            %d B    %.4f KB", free_heap, free_heap / 1024.0);
        TRACE_W("Heap Watermark:            %d B    %.4f KB", watermark_heap, watermark_heap / 1024.0);
        TRACE_I("----------------------------------------------");

        ezlopi_wifi_status_t *wifi_stat = EZPI_core_wifi_status();
        if (wifi_stat)
        {
            if (wifi_stat->wifi_connection == false)
            {
                char *ssid = EZPI_core_factory_info_v3_get_ssid();
                if (ssid)
                {
                    TRACE_E("Failed to connect to AP : %s, retrying ...", ssid);
                    ezlopi_free(__FUNCTION__, ssid);
                }
                else
                {
                    TRACE_E("ERROR : WiFi Details not set !");
                }
            }
            ezlopi_free(__FUNCTION__, wifi_stat);
        }

        // separating the scope
#ifdef CONFIG_EZPI_ENABLE_UART_PROVISIONING
        {
            char cmd99_str[100] = { 0 };
            snprintf(cmd99_str, 100, "{\"cmd\":99,\"free_heap\":%d,\"heap_watermark\":%d}", free_heap, watermark_heap);
            EZPI_SERV_uart_tx_data(strlen(cmd99_str), (uint8_t *)cmd99_str);
            TRACE_OTEL(ENUM_EZLOPI_TRACE_SEVERITY_DEBUG, "%s", cmd99_str);
        }
#endif

        if (free_heap <= (10 * 1024))
        {
            TRACE_W("CRITICAL-WARNING: low heap detected..");

            if ((xTaskGetTickCount() - low_heap_start_time) > (15000 / portTICK_PERIOD_MS))
            {
                vTaskDelay(2000 / portTICK_RATE_MS);
                TRACE_E("CRITICAL-ERROR: low heap time-out detected!");
                TRACE_W("Rebooting.....");
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                EZPI_core_reset_reboot();
            }
        }
        else
        {
            low_heap_start_time = xTaskGetTickCount();
        }

#ifdef CONFIG_EZPI_HEAP_ENABLE
        EZPI_core_util_heap_trace(false);
        EZPI_core_util_heap_flush();
#endif // CONFIG_EZPI_HEAP_ENABLE
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}
