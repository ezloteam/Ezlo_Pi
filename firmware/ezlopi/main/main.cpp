#include <iostream>
#include <string.h>
#include <stdio.h>

#include "nvs.h"
#include "nvs_flash.h"
#include "mbedtls/config.h"
#include "driver/gpio.h"
#include "nvs_storage.h"

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
#include "websocket_client.h"
#include "interface_common.h"
#include "switch_service.h"
#include "gatt_server.h"
#include "cJSON.h"

using namespace std;

http http_inst;

static void blinky(void *pv);
static void main_task(void *pv)
{
    char url[128];
    web_provisioning *web_provisioning_ctx = web_provisioning::get_instance();
    factory_info *factory_data = factory_info::get_instance();

    snprintf(url, 128, "%s/getserver?json=true", factory_data->cloud_server);
    TRACE_D("Calling Cloud Server api: %s\r\n", url);

    while (1)
    {
        string ws_endpoint = "";
        struct json_token wss_uri_tok = JSON_INVALID_TOKEN;

        wait_for_wifi_to_connect();
        ws_endpoint = http_inst.http_get_request(url, factory_data->ssl_private_key, factory_data->ssl_shared_key, factory_data->ca_certificate);
        TRACE_D("ws_endpoint: %s\r\n", ws_endpoint.c_str());

        if (json_scanf(ws_endpoint.c_str(), ws_endpoint.length(), "{uri:%T}", &wss_uri_tok))
        {
            if (wss_uri_tok.len > 8)
            {
                string s_uri = "";
                s_uri.assign(wss_uri_tok.ptr, wss_uri_tok.len);
                web_provisioning_ctx->init(s_uri);
                break;
            }
        }

        vTaskDelay(2000);
    }

    vTaskDelete(NULL);
}

extern "C" void app_main(void)
{
    gpio_install_isr_service(0);
    qt_serial::get_instance();
    factory_info::get_instance();
    nvs_storage::get_instance();
    devices_common::get_instance();
    interface_common::get_instance();
    switch_service::get_instance();
    GATT_SERVER_MAIN();

    wifi_initialize();
    wifi_connect_from_nvs();

    xTaskCreate(main_task, "main task", 20 * 1024, NULL, 2, NULL);
    // xTaskCreate(blinky, "blinky", 2048, NULL, 1, NULL);
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

    gpio_config(&io_conf);
    uint32_t state = 0;

    vTaskDelay(5000);
    uint32_t count = 0;

    while (1)
    {
        gpio_set_level(GPIO_NUM_2, state);
        vTaskDelay(100);
        state ^= 1;

        if (count++ > 20)
        {
            // TRACE_D("Total heaps: %d\r\nFree heap: %d\r\n", heap_caps_get_total_size());
        }
    }
}

void ble_hub(void)
{
    char prov_cred_buf[256];
    // Initialize NVS.
    esp_err_t ret;

    size_t required_size = 0;

    nvs_handle_t my_handle;
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &my_handle);
    if (err != ESP_OK)
    {
        printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
    }
    else
    {
        printf("Done\n");
        nvs_get_str(my_handle, "recv_buff", NULL, &required_size);

        nvs_get_str(my_handle, "recv_buff", prov_cred_buf, &required_size);
        printf("%d", required_size);
        if (required_size > 0)
        {
            switch (err)
            {
            case ESP_OK:
#ifdef _BLE_HUB
                GATT_CLIENT_MAIN();
#else
#endif
                break;
            case ESP_ERR_NVS_NOT_FOUND:
                printf("The value is not initialized yet!\n");
#ifndef _BLE_HUB
                GATT_SERVER_MAIN();
#endif
                break;
            default:
                printf("Error (%s) reading!\n", esp_err_to_name(err));
            }
        }
    }
}