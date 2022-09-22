#include "string.h"
#include "stdint.h"
#include "time.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "ezlopi_actions.h"

static QueueHandle_t event_queue = NULL;

static void event_process(void *pv);

void sensor_service(void)
{
    event_queue = xQueueCreate(20, sizeof(e_ezlopi_actions_t));
    xTaskCreate(event_process, "event_process", 4 * 1024, NULL, 4, NULL);
}

static void event_process(void *pv)
{
    while (1)
    {
        e_ezlopi_actions_t action = EZLOPI_ACTION_NONE;
        if (pdTRUE == xQueueReceive(event_queue, &action, UINT32_MAX / portTICK_PERIOD_MS))
        {
            s_sensors_schedule_t *sensor = sensor_schedule_head;
            while (sensor)
            {
                sensor->sensor_call(action, NULL);
                sensor = sensor->next;
            }
        }
    }
}