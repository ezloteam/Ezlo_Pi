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

static void blinky(void *pv);
// static int (*sensor_list[])(e_ezlopi_actions_t action, void *arg) = {
//     // #ifdef SENSOR_0010_BME280
//     // &sensor_bme280,
//     sensor_bme280,
//     // #endif
//     NULL, // Do not remove 'NULL' from list, because the element of the list is compared with NULL for termination of loop
// };

void inline print_timer_counter(uint64_t counter_value)
{
    printf("Counter: 0x%08x%08x\r\n", (uint32_t)(counter_value >> 32),
           (uint32_t)(counter_value));
    printf("Time   : %.8f s\r\n", (double)counter_value / EZLOPI_TIMER_SCALE);
}

void app_main(void)
{
    ezlopi_event_queue_init();
    sensor_service_init();
    ezlopi_timer_start_500ms();
    xTaskCreate(blinky, "blinky", 2048, NULL, 1, NULL);

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = 21,           // select GPIO specific to your project
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = 22,         // select GPIO specific to your project
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = 1000000,  // select frequency specific to your project
        .clk_flags = 0,                          // you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here
    };

    if(ESP_OK != i2c_param_config(I2C_NUM_0, &conf))
    {
        ESP_LOGI("EZLOPI BME280", "Error configuring i2c");
    }
    else
    {
        ESP_LOGI("EZLOPI BME280", "Successfully configured i2c");
    }

    if(ESP_OK != i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0))
    {
        ESP_LOGI("EZLOPI BME280", "Error installing i2c driver");
    }
    else
    {
        ESP_LOGI("EZLOPI BME280", "Successfully installede i2c driver");
    }


    sensor_bme280(EZLOPI_ACTION_INITIALIZE, NULL);
    sensor_bme280(EZLOPI_ACTION_GET_VALUE, NULL);
    sensor_bme280(EZLOPI_ACTION_NOTIFY_500_MS, NULL);
    // ESP_LOGE("EZLOPI_TIMER", "Initializing timer...");
    // xTaskCreate(ezlopi_timer_task, "ezlopi_timer_task", 3072, NULL, 0, NULL);
    
    // sensor_service();
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
        {
            // TRACE_D("-----------------------------------------");
            // TRACE_D("esp_get_free_heap_size - %d", esp_get_free_heap_size());
            // TRACE_D("esp_get_minimum_free_heap_size: %u", esp_get_minimum_free_heap_size());
            // TRACE_D("-----------------------------------------");
        }

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
