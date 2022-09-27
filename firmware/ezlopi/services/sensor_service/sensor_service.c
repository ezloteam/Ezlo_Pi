#include "string.h"
#include "stdint.h"
#include "time.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "sensor_service.h"
#include "ezlopi_actions.h"
#include "ezlopi_sensors.h"

static QueueHandle_t event_queue = NULL;

int sensor_service_add_event_to_queue(s_ezlo_event_t *event, int from_isr)
{
    int ret = 0;
    if (NULL != event_queue)
    {
        if (event)
        {
            if (from_isr)
            {
                ret = (pdTRUE == xQueueSendFromISR(event_queue, &event, pdFALSE)) ? 1 : 0;
            }
            else
            {
                ret = (pdTRUE == xQueueSend(event_queue, &event, 50)) ? 1 : 0;
            }
        }
    }

    return ret;
}

static void event_process(void *pv);

void sensor_service(void)
{
    event_queue = xQueueCreate(20, sizeof(s_ezlo_event_t *));
    xTaskCreate(event_process, "event_process", 4 * 1024, NULL, 4, NULL);
}

static void event_process(void *pv)
{
    const f_sensor_call_t *sensor_list = ezlopi_sensor_get_list();

    while (1)
    {
        s_ezlo_event_t *event = NULL; //  = {.action = EZLOPI_ACTION_NONE, .arg = NULL};
        if (pdTRUE == xQueueReceive(event_queue, &event, UINT32_MAX / portTICK_PERIOD_MS))
        {
            if (event)
            {
                int idx = 0;
                while (NULL != sensor_list[idx])
                {
                    sensor_list[idx](event->action, event->arg);
                    idx++;
                }
            }
            else
            {
                free(event);
            }
        }
    }
}