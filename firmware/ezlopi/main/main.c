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
#include "ezlopi_uart.h"
#include "ezlopi_adc.h"
#include "ezlopi_pwm.h"

static void blinky(void *pv);

extern int sensor_bme280(e_ezlopi_actions_t action, void *arg);


static void pwm_task(void* args)
{

    s_ezlopi_channel_speed_t* channel_speed = ezlopi_pwm_init(GPIO_NUM_2, LEDC_TIMER_12_BIT, 10000, 0);
    TRACE_B("channel is %d, speed is %d", channel_speed->channel, channel_speed->speed_mode);
    while(1)
    {
        for(uint32_t i = 0; i < 4096; i +=   100)
        {
            TRACE_E("Duty is: %d", i);
            ezlopi_pwm_change_duty(channel_speed->channel, channel_speed->speed_mode, i);
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
           
    }
}

void app_main(void)
{
    // qt_serial_init();
    // gpio_isr_service_init();
    // ezlopi_init();
    // web_provisioning_init();
    // GATT_SERVER_MAIN();
    // sensor_service_init();


    xTaskCreate(pwm_task, "pwm_task", 2 * 2048, NULL, 1, NULL);

    // xTaskCreate(blinky, "blinky", 2 * 2048, NULL, 1, NULL);
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
            TRACE_D("esp_get_free_heap_size - %d", esp_get_free_heap_size());
            TRACE_D("esp_get_minimum_free_heap_size: %u", esp_get_minimum_free_heap_size());
            TRACE_D("-----------------------------------------");
            count = 0;
        }
    }
}
