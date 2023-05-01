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
#include "ota_service.h"
#include "web_provisioning.h"
#include "gpio_isr_service.h"
#include "ezlopi_ble_service.h"
#include "ezlopi_system_info.h"
#include "029_IR_blaster_remote.h"

#include "mac_uuid.h"

static void blinky(void *pv);

void app_main(void)
{
    TRACE_B("sizeof long: %u", sizeof(long));
    TRACE_B("sizeof long-long: %u", sizeof(long long));
    TRACE_B("sizeof time_t: %u", sizeof(time_t));
    qt_serial_init();
    gpio_isr_service_init();
    ezlopi_init();
    web_provisioning_init();
    ota_service_init();
    ezlopi_ble_service_init();
    timer_service_init();
    TRACE_B("Boot count: %d", ezlopi_system_info_get_boot_count());
    xTaskCreate(blinky, "blinky", 2 * 2048, NULL, 1, NULL);

    // IR_Blaster_prepare();
}

static void blinky(void *pv)
{
    // gpio_config_t io_conf = {
    //     .pin_bit_mask = (1ULL << GPIO_NUM_2),
    //     .mode = GPIO_MODE_OUTPUT,
    //     .pull_up_en = GPIO_PULLUP_DISABLE,
    //     .pull_down_en = GPIO_PULLDOWN_DISABLE,
    //     .intr_type = GPIO_INTR_DISABLE,
    // };

    // uint32_t state = 0;
    uint32_t count = 0;

    // adc1_config_width(ADC_WIDTH_BIT_12);
    // gpio_config(&io_conf);
    char uuid[37] = {0};
    while (1)
    {
        // state ^= 1;
        // gpio_set_level(GPIO_NUM_2, state);
        // int hall_sensor_value = hall_sensor_read();
        // int hall_sensor_value = 0;
        // TRACE_D("Hall Sensor value: %d\r\n", hall_sensor_value);

        vTaskDelay(1000 / portTICK_RATE_MS);

        if (count++ > 2)
        {
            TRACE_D("-----------------------------------------");
            ezlopi_generate_UUID(uuid);
            TRACE_D("UUID: %s", uuid);
            TRACE_D("esp_get_free_heap_size - %d", esp_get_free_heap_size());
            TRACE_D("esp_get_minimum_free_heap_size: %u", esp_get_minimum_free_heap_size());
            TRACE_D("-----------------------------------------");
            count = 0;
        }
    }
}
