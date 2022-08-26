#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "debug.h"
#include "dht.h"
#include "string.h"
#include "items.h"
#include "wss.h"

static float humidity = 0.0f;
static float temperature = 0.0f;
static uint32_t device_index = 0xFF;
static uint32_t dht_sensor_pin = 0xFF;
static void dht11_service_process(void *pv);
uint32_t web_provisioning_get_message_count(void);

float dht11_service_get_temperature(void)
{
    return temperature;
}

float dht11_service_get_humidity(void)
{
    return humidity;
}

void dht11_service_init(uint8_t dht_pin, uint32_t dev_idx)
{
    dht_sensor_pin = dht_pin;
    device_index = dev_idx;
    // xTaskCreatePinnedToCore(dht11_service_process, "dht11-service", 3072, NULL, 3, NULL, 1);
}

static void dht11_service_process(void *pv)
{
    while (1)
    {
        dht_read_float_data(DHT_TYPE_AM2301, dht_sensor_pin, &humidity, &temperature);
        TRACE_B("temperature: %f", temperature);
        TRACE_B("humidity: %f", humidity);
        char *ret = items_update_with_device_index(NULL, 0, NULL, web_provisioning_get_message_count(), device_index);

        if (ret)
        {
            // TRACE_W(">> DHT-service TX(ret): %s", ret);
            wss_client_send(ret, strlen(ret));
            vPortFree(ret);
            ret = NULL;
        }

        vTaskDelay(60000 / portTICK_RATE_MS);
    }
}