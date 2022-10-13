#include <string.h>
#include <stdio.h>

#include "nvs.h"
#include "nvs_flash.h"
#include "mbedtls/config.h"
#include "driver/gpio.h"
#include "driver/adc.h"
#include "nvs_storage.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "sensor_service.h"
#include "ezlopi_timer.h"
#include "ezlopi_event_queue.h"
#include "sensor_bme280.h"

#include "driver/i2c.h"
#include "ezlopi_i2c_master.h"

static void blinky(void *pv);

void app_main(void)
{
    ezlopi_event_queue_init();
    sensor_service_init();
    ezlopi_timer_start_500ms();

    s_ezlopi_i2c_master_t i2c_master_conf = EZLOPI_I2C_MASTER_DEFAULT_CONF;
    ezlopi_i2c_master_init(&i2c_master_conf);
    xTaskCreate(blinky, "blinky", 2048, NULL, 1, NULL);

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

    adc1_config_width(ADC_WIDTH_BIT_12);
    gpio_config(&io_conf);

    while (1)
    {
        state ^= 1;
        gpio_set_level(GPIO_NUM_2, state);
        // int hall_sensor_value = hall_sensor_read();
        int hall_sensor_value = 0;
        printf("Hall Sensor value: %d\r\n", hall_sensor_value);

        vTaskDelay(1000 / portTICK_RATE_MS);

        // if (count++ > 20)
        // {
        //     TRACE_D("-----------------------------------------");
        //     TRACE_D("esp_get_free_heap_size - %d", esp_get_free_heap_size());
        //     TRACE_D("esp_get_minimum_free_heap_size: %u", esp_get_minimum_free_heap_size());
        //     TRACE_D("-----------------------------------------");
        // }

        // s_ezlo_event_t *event = malloc(sizeof(s_ezlo_event_t));
        // if (event)
        // {
        //     event->action = EZLOPI_ACTION_GET_VALUE;
        //     event->arg = NULL;

        //     if (0 == ezlopi_event_queue_send(&event, 0))
        //     {
        //         printf("Error: failed to send to the queue!\n");
        //         // free(event);
        //     }
        // }

        // float humidity, temperature;
        // dht_read_float_data(DHT_TYPE_AM2301, dht22_pin, &humidity, &temperature);
        // printf(">>>>>> SN-002 real data -> Humidity: %.02f, Temperature: %.02f <<<<<<\n", humidity, temperature);
    }
}
