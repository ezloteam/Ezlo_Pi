#include "string.h"
#include "stdint.h"
#include "time.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "sensor_service.h"
#include "ezlopi_actions.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_event_queue.h"

static void event_process(void *pv);

void sensor_service_init(void)
{
    xTaskCreate(event_process, "event_process", 4 * 1024, NULL, 4, NULL);
}

static void event_process(void *pv)
{
    l_ezlopi_configured_devices_t *registered_device = ezlopi_devices_list_get_configured_items();
    while (NULL != registered_device)
    {
        registered_device->device->func(EZLOPI_ACTION_INITIALIZE, registered_device->properties, NULL);
        registered_device = registered_device->next;
    }

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
                printf("action received: %s\n", ezlopi_actions_to_string(event->action));

                registered_device = ezlopi_devices_list_get_configured_items();
                while (NULL != registered_device)
                {
                    registered_device->device->func(event->action, registered_device->properties, event->arg);
                    registered_device = registered_device->next;
                }
            }
            else
            {
                free(event);
            }
        }
    }
}