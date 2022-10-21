#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/adc.h"

#include "trace.h"
#include "items.h"
#include "devices.h"
#include "web_provisioning.h"
#include "ezlopi_wss.h"

static void hall_sensor_process(void *pv);
static uint32_t device_index = 0;
static int hall_sensor_value = 0;

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
    char value_buff[128];

    while (1)
    {
        // hall_sensor_value = hall_sensor_read();  /*!< ADC1 channel 3 is GPIO39 */ /*!< ADC1 channel 0 is GPIO36 */
        printf("Value: %d\r\n", hall_sensor_value);

        memset(value_buff, 0, sizeof(value_buff));
        snprintf(value_buff, sizeof(value_buff), "%s", (hall_sensor_value > 65 || hall_sensor_value < 15) ? "\"dw_is_closed\"" : "\"dw_is_opened\"");
        char *ret = items_update_from_sensor(device_index, value_buff);

        if (ret)
        {
            TRACE_B(">> WS Tx - 'hub.item.updated' [%d]\r\n%s", strlen(ret), ret);
            wss_client_send(ret, strlen(ret));
            vPortFree(ret);
            ret = NULL;
        }

        vTaskDelay(2000 / portTICK_RATE_MS);
    }
}