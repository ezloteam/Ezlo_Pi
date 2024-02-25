#include <stdio.h>

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
#include "ezlopi_service_ble.h"
#include "ezlopi_service_uart.h"
#include "ezlopi_service_timer.h"
#include "ezlopi_service_modes.h"
#include "ezlopi_service_meshbot.h"
#include "ezlopi_service_gpioisr.h"
#include "ezlopi_service_webprov.h"
#include "ezlopi_service_ws_server.h"

#define ENABLE_HEARTBIT_LED 0

static void blinky(void *pv);

void app_main(void)
{
    gpio_install_isr_service(0);
    qt_serial_init();
    gpio_isr_service_init();
    ezlopi_init();
    ezlopi_ble_service_init();
    timer_service_init();
    ezlopi_scenes_meshbot_init();
    web_provisioning_init();
    ota_service_init();
    ezlopi_service_modes_init();
    ezlopi_service_ws_server_start();

    xTaskCreate(blinky, "blinky", 2 * 2048, NULL, 1, NULL);
}

static void blinky(void *pv)
{
#if (1 == ENABLE_HEARTBIT_LED)
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    uint32_t state = 0;
    gpio_config(&io_conf);
#endif

    uint32_t count = 0;
    while (1)
    {
#if (1 == ENABLE_HEARTBIT_LED)
        state ^= 1;
        gpio_set_level(GPIO_NUM_2, state);
#endif

        if (count++ > 10)
        {
            count = 0;

            trace_wb("-----------------------------------------");
            trace_wb("esp_get_free_heap_size - %f kB", esp_get_free_heap_size() / 1024.0);
            trace_wb("esp_get_minimum_free_heap_size: %f kB", esp_get_minimum_free_heap_size() / 1024.0);

            trace_wb("-----------------------------------------");

            char *sdfgh = "asdfghjkl";
            ezlopi_service_ws_server_broadcast(sdfgh, strlen(sdfgh));
        }

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
