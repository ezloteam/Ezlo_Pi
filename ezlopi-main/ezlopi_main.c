#include <stdio.h>

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
#if CONFIG_EZPI_SERV_BLE_EN == 1
#endif

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

static void __blinky(void* pv);


void app_main(void)
{
    ezlopi_service_led_indicator_init();
    gpio_install_isr_service(0);

    gpio_isr_service_init();

    ezlopi_init();

    EZPI_SERV_uart_init();

    timer_service_init();

#if CONFIG_EZPI_SERV_BLE_EN == 1
    ezlopi_ble_service_init();
#endif

    ezlopi_service_broadcast_init();
    ezlopi_service_ws_server_start();
    ezlopi_service_web_provisioning_init();

    ezlopi_service_ota_init();
#if CONFIG_EZLPI_SERV_MODES_EN
    ezlopi_service_modes_init();
#endif
#if CONFIG_EZPI_SERV_MESHBOT_EN
    ezlopi_scenes_meshbot_init();
#endif

    xTaskCreate(__blinky, "__blinky", 2 * 2048, NULL, 0, NULL);
}


static void __blinky(void* pv)
{

    uint32_t count = 0;

    while (1)
    {
        if (count++ > 1000)
        {
            count = 0;

            trace_wb("----------------------------------------------");
            trace_wb("esp_get_free_heap_size - %f kB", esp_get_free_heap_size() / 1024.0);
            trace_wb("esp_get_minimum_free_heap_size: %f kB", esp_get_minimum_free_heap_size() / 1024.0);
            trace_wb("----------------------------------------------");
        }

        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
}


