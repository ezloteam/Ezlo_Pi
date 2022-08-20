#include <string.h>
#include <stdio.h>

#include "nvs.h"
#include "nvs_flash.h"
#include "mbedtls/config.h"
#include "driver/gpio.h"
#include "nvs_storage.h"
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "wifi.h"
#include "http.h"
#include "debug.h"
#include "frozen.h"
#include "qt_serial.h"
#include "factory_info.h"
#include "devices_common.h"
#include "web_provisioning.h"
#include "interface_common.h"
#include "switch_service.h"
#include "gatt_server.h"
#include "dht.h"
#include "MPU6050.h"

#include "wss.h"

static void blinky(void *pv);
static void main_task(void *pv)
{
    char url[128];
    s_factory_info_t *factory = factory_info_get_info();

    snprintf(url, 128, "%s/getserver?json=true", factory->cloud_server);
    TRACE_D("Calling Cloud Server api: %s\r\n", url);

    while (1)
    {
        char *ws_endpoint = NULL;
        struct json_token wss_uri_tok = JSON_INVALID_TOKEN;

        wait_for_wifi_to_connect();
        ws_endpoint = http_get_request(url, factory->ssl_private_key, factory->ssl_shared_key, factory->ca_certificate);
        if (ws_endpoint)
        {
            TRACE_D("ws_endpoint: %s\r\n", ws_endpoint);

            if (json_scanf(ws_endpoint, strlen(ws_endpoint), "{uri:%T}", &wss_uri_tok))
            {
                if (wss_uri_tok.len > 8)
                {
                    web_provisioning_init(&wss_uri_tok);
                    break;
                }
            }

            free(ws_endpoint);
        }

        vTaskDelay(2000 / portTICK_RATE_MS);
    }

    vTaskDelete(NULL);
}

void app_main(void)
{
    gpio_install_isr_service(0);
    qt_serial_init();
    factory_info_init();
    nvs_storage_init();
    devices_common_init_devices();
    interface_common_init();
    switch_service_init();
    GATT_SERVER_MAIN();

    wifi_initialize();
    wifi_connect_from_nvs();

    xTaskCreate(main_task, "main task", 20 * 1024, NULL, 2, NULL);
    xTaskCreate(blinky, "blinky", 2048, NULL, 1, NULL);
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
    // uint32_t count = 0;
    // gpio_config(&io_conf);
    // gpio_pad_select_gpio(dht22_pin);

    while (1)
    {
        // gpio_set_level(GPIO_NUM_2, state);
        vTaskDelay(5000 / portTICK_RATE_MS);
        // state ^= 1;

        // if (count++ > 20)
        {
            TRACE_D("-----------------------------------------");
            TRACE_D("esp_get_free_heap_size - %d", esp_get_free_heap_size());
            TRACE_D("esp_get_minimum_free_heap_size: %u", esp_get_minimum_free_heap_size());
            TRACE_D("-----------------------------------------");
        }

        // float humidity, temperature;
        // dht_read_float_data(DHT_TYPE_AM2301, dht22_pin, &humidity, &temperature);
        // printf(">>>>>> SN-002 real data -> Humidity: %.02f, Temperature: %.02f <<<<<<\n", humidity, temperature);
    }
}
