#include "string.h"
#include "stdint.h"
#include "time.h"
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "trace.h"
#include "timer_service.h"
#include "ezlopi_actions.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_event_queue.h"

static void event_process(void *pv);

void timer_service_init(void)
{
    xTaskCreate(event_process, "event_process", 2 * 2048, NULL, 4, NULL);
}

static void event_process(void *pv)
{
    TickType_t old_tick = xTaskGetTickCount();

    while (1)
    {
        s_ezlo_event_t *event = NULL;
        if (pdTRUE == ezlopi_event_queue_receive(&event, 2000 / portTICK_PERIOD_MS))
        {
            TRACE_D("event tick: %d", xTaskGetTickCount() - old_tick);
            old_tick = xTaskGetTickCount();

            if (NULL != event)
            {
                l_ezlopi_configured_devices_t *registered_device = ezlopi_devices_list_get_configured_items();
                while (NULL != registered_device)
                {
                    registered_device->device->func(event->action, registered_device->properties, event->arg, registered_device->user_arg);
                    registered_device = registered_device->next;
                    vTaskDelay(50);
                }

                free(event);
                event = NULL;
            }
        }
    }
}
