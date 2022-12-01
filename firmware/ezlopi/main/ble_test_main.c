#include <stdio.h>
#include "nvs.h"
#include "nvs_flash.h"
#include "esp_system.h"
#include "driver/adc.h"
#include "driver/i2c.h"
#include "driver/gpio.h"
#include "mbedtls/config.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_timer.h"
#include "timer_service.h"
#include "gpio_isr_service.h"
#include "ezlopi_event_queue.h"

#include "trace.h"
#include "ezlopi.h"
#include "qt_serial.h"
#include "web_provisioning.h"
#include "ezlopi_ble_gatt_server.h"
#include "gpio_isr_service.h"

#include "ezlopi_ble_v2.h"
#include "ezlopi_ble_gatt.h"

static void blinky(void *pv);

void app_main(void)
{
    esp_err_t err = nvs_flash_init();

    if (ESP_ERR_NVS_NO_FREE_PAGES == err || ESP_ERR_NVS_NEW_VERSION_FOUND == err)
    {
        TRACE_D("NVS Init Failed once!, Error: %s", esp_err_to_name(err));
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    ezlopi_ble_v2_init();
    ezlopi_ble_gatt_print_profiles();
    xTaskCreate(blinky, "blinky", 2 * 2048, NULL, 1, NULL);
}

static void blinky(void *pv)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << GPIO_NUM_2),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };

    uint32_t state = 0;
    uint32_t count = 0;
    gpio_config(&io_conf);
    uint32_t prev_free_heap = 0;
    uint32_t prev_water_mark = 0;

    while (1)
    {
        state ^= 1;
        gpio_set_level(GPIO_NUM_2, state);

        uint32_t free_heap = esp_get_free_heap_size();
        uint32_t water_mark = esp_get_minimum_free_heap_size();

        if ((count++ > 5) || (prev_free_heap != free_heap) || (prev_water_mark != water_mark))
        {
            prev_free_heap = free_heap;
            prev_water_mark = water_mark;

            TRACE_D("--------------------------------");
            TRACE_D("Free Heap - %d", free_heap);
            TRACE_D("Water Mark - %u", water_mark);
            TRACE_D("--------------------------------");
            count = 0;
        }

        vTaskDelay(1000 / portTICK_RATE_MS);
    }
}
