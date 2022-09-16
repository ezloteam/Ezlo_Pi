#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/adc.h"

#include "items.h"
#include "devices.h"
#include "web_provisioning.h"
#include "wss.h"

static void hall_sensor_process(void *pv);
static int hall_sensor_value = 0;
static uint32_t device_index = 0;

int hall_sensor_value_get(void)
{
    return hall_sensor_value;
}

void hall_sensor_service_init(uint32_t dev_idx)
{
    device_index = dev_idx;
    xTaskCreate(hall_sensor_process, "hall sensor process", 2048, NULL, 4, NULL);
}

static void hall_sensor_process(void *pv)
{
    adc1_config_width(ADC_WIDTH_BIT_12);

    while (1) {
        hall_sensor_value = hall_sensor_read();
        char *ret = items_update_with_device_index(NULL, 0, NULL, web_provisioning_get_message_count(), device_index);
        printf("Value: %d\r\n", hall_sensor_value);

        if (ret)
        {
            // TRACE_W(">> DHT-service TX(ret): %s", ret);
            wss_client_send(ret, strlen(ret));
            vPortFree(ret);
            ret = NULL;
        }

        vTaskDelay(2000/portTICK_RATE_MS);
    }
}