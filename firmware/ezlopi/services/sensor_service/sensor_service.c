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
#include "ezlopi_event_queue.h"

static void event_process(void *pv);

void sensor_service_init(void)
{
    xTaskCreate(event_process, "event_process", 4 * 1024, NULL, 4, NULL);
}

static void event_process(void *pv)
{
    const f_sensor_call_t *sensor_list = ezlopi_sensor_get_list();
    int old_tick = xTaskGetTickCount();

    while (1)
    {
        s_ezlo_event_t *event = NULL; //  = {.action = EZLOPI_ACTION_NONE, .arg = NULL};
        if (pdTRUE == ezlopi_event_queue_receive(&event, UINT32_MAX / portTICK_PERIOD_MS))
        {
            printf("Tick expired: %d\n", xTaskGetTickCount() - old_tick);
            old_tick = xTaskGetTickCount();

            if (event)
            {
                int idx = 0;
                printf("action received: %s\n", ezlopi_actions_to_string(event->action));

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