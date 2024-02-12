#include <stdio.h>

#include "EZLOPI_USER_CONFIG.h"

#include <esp_system.h>
#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "ezlopi_util_trace.h"
#include "ezlopi_core_ezlopi.h"

static void blinky(void* pv);

void app_main(void)
{

    xTaskCreate(blinky, "blinky", 2048, NULL, configMAX_PRIORITIES - 1, NULL);
    ezlopi_init();
}

static void blinky(void* pv)
{

#ifdef EZPI_USR_CONFIG_ENABLE_HEARTBIT_LED
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

#ifdef EZPI_USR_CONFIG_ENABLE_HEARTBIT_LED
        state ^= 1;
        gpio_set_level(GPIO_NUM_2, state);
#endif
        if (count++ > 5)
        {
            TRACE_D("-----------------------------------------");
            TRACE_D("esp_get_free_heap_size: %.2fKB", (float)(esp_get_free_heap_size() / 1024.0));
            TRACE_D("esp_get_minimum_free_heap_size: %.2fKB", (float)(esp_get_minimum_free_heap_size() / 1024.0));
            TRACE_D("-----------------------------------------");
            count = 0;
        }

        vTaskDelay(100 / portTICK_RATE_MS);
    }
}
