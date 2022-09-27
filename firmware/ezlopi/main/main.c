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

static void blinky(void *pv);

void inline print_timer_counter(uint64_t counter_value)
{
    printf("Counter: 0x%08x%08x\r\n", (uint32_t)(counter_value >> 32),
           (uint32_t)(counter_value));
    printf("Time   : %.8f s\r\n", (double)counter_value / EZLOPI_TIMER_SCALE);
}

void ezlopi_timer_task(void *args)
{
    s_timer_queue = xQueueCreate(10, sizeof(ezlopi_timer_event_t));

    ESP_LOGE("EZLOPI_TIMER", "The timer frequency is: %d", TIMER_BASE_CLK);
    ezlopi_tg_timer_init(EZLOPI_ACTIONS_NOTIFY_50_MS);
    ezlopi_tg_timer_init(EZLOPI_ACTIONS_NOTIFY_100_MS);
    ezlopi_tg_timer_init(EZLOPI_ACTIONS_NOTIFY_200_MS);
    ezlopi_tg_timer_init(EZLOPI_ACTIONS_NOTIFY_500_MS);

    while (1)
    {
        ezlopi_timer_event_t evt;
        xQueueReceive(s_timer_queue, &evt, portMAX_DELAY);

        /* Print information that the timer reported an event */
        if (evt.info.auto_reload)
        {
            printf("Timer Group with auto reload\n");
        }
        else
        {
            printf("Timer Group without auto reload\n");
        }
        printf("Group[%d], timer[%d] alarm event\n", evt.info.timer_group, evt.info.timer_idx);

        /* Print the timer values passed by event */
        printf("------- EVENT TIME --------\n");
        print_timer_counter(evt.timer_counter_value);

        /* Print the timer values as visible by this task */
        printf("-------- TASK TIME --------\n");
        uint64_t task_counter_value;
        timer_get_counter_value(evt.info.timer_group, evt.info.timer_idx, &task_counter_value);
        print_timer_counter(task_counter_value);

        printf("-------- NOTIFY ENUM --------\n");
        ESP_LOGE("EZLOPI_TIMER", "The notify enum val is: %d", evt.info.notify);
    }
}

void app_main(void)
{

    ESP_LOGE("EZLOPI_TIMER", "Initializing timer...");
    xTaskCreate(ezlopi_timer_task, "ezlopi_timer_task", 3072, NULL, 0, NULL);
    
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
        int hall_sensor_value = hall_sensor_read();
        printf("Hall Sensor value: %d\r\n", hall_sensor_value);

        vTaskDelay(200 / portTICK_RATE_MS);

        // if (count++ > 20)
        {
            // TRACE_D("-----------------------------------------");
            // TRACE_D("esp_get_free_heap_size - %d", esp_get_free_heap_size());
            // TRACE_D("esp_get_minimum_free_heap_size: %u", esp_get_minimum_free_heap_size());
            // TRACE_D("-----------------------------------------");
        }

        s_ezlo_event_t *event = malloc(sizeof(s_ezlo_event_t));
        if (event)
        {
            if (0 == sensor_service_add_event_to_queue(event, 0))
            {
                free(event);
            }
        }

        // float humidity, temperature;
        // dht_read_float_data(DHT_TYPE_AM2301, dht22_pin, &humidity, &temperature);
        // printf(">>>>>> SN-002 real data -> Humidity: %.02f, Temperature: %.02f <<<<<<\n", humidity, temperature);
    }
}
