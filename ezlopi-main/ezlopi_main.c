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
#include "EZLOPI_USER_CONFIG.h"

#include "ezlopi_core_ezlopi.h"
#include "ezlopi_service_ota.h"

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
#include "ezlopi_util_heap.h"

#include "pt.h"

static void __blinky(void* pv);


void app_main(void)
{
#if 1
    ezlopi_service_led_indicator_init();
    gpio_install_isr_service(0);

    gpio_isr_service_init();

    ezlopi_init();

#if defined(CONFIG_EZPI_ENABLE_UART_PROVISIONING)
    // EZPI_SERVICE_uart_init();
#endif

    timer_service_init();

#if defined(CONFIG_EZLOPI_BLE_ENABLE)
    ezlopi_ble_service_init();
#endif

#if defined(CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER) || defined(CONFIG_EZPI_WEBSOCKET_CLIENT)
    ezlopi_service_broadcast_init();
#endif

    ezlpi_service_ws_server_dummy();

#if defined(CONFIG_EZPI_LOCAL_WEBSOCKET_SERVER)
    ezlopi_service_ws_server_start();
#endif

#if defined(CONFIG_EZPI_WEBSOCKET_CLIENT)
    ezlopi_service_web_provisioning_init();
#endif

#if defined(CONFIG_EZPI_ENABLE_OTA)
    ezlopi_service_ota_init();
#endif

#if CONFIG_EZLPI_SERV_ENABLE_MODES
    ezlopi_service_modes_init();
#endif

#if CONFIG_EZPI_SERV_ENABLE_MESHBOTS
    ezlopi_scenes_meshbot_init();
#endif
#endif
    xTaskCreate(__blinky, "__blinky", 5 * 2048, NULL, 0, NULL);
}


static void __blinky(void* pv)
{
    uint32_t low_heap_start_time = xTaskGetTickCount();

    while (1)
    {
        float free_heap_kb = esp_get_free_heap_size() / 1024.0;

        trace_wb("----------------------------------------------");
        trace_wb("esp_get_free_heap_size - %.02f kB", free_heap_kb);
        trace_wb("esp_get_minimum_free_heap_size: %.02f kB", esp_get_minimum_free_heap_size() / 1024.0);
        trace_wb("----------------------------------------------");

        ezlopi_util_heap_trace();
        
        if (free_heap_kb <= 10)
        {
            TRACE_W("CRITICAL-WARNING: low heap detected..");
            ezlopi_util_heap_trace();
        }
        else if ((xTaskGetTickCount() - low_heap_start_time) > (15000 / portTICK_PERIOD_MS))
        {
            ezlopi_util_heap_trace();
            vTaskDelay(2000 / portTICK_RATE_MS);
            TRACE_E("CRITICAL-ERROR: low heap time-out detected!");
            TRACE_W("Rebooting.....");
            esp_restart();
        }
        else
        {
            low_heap_start_time = xTaskGetTickCount();
        }

        ezlopi_util_heap_flush();
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}
