#include <string.h>
#include <stdint.h>
#include <time.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "trace.h"

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_event_queue.h"

#include "ezlopi_service_timer.h"

static void event_process_v3(void *pv);

void timer_service_init(void)
{
    // xTaskCreate(event_process, "event_process", 2 * 2048, NULL, 4, NULL);
    xTaskCreate(event_process_v3, "event_process_v3", 2 * 2048, NULL, 4, NULL);
}

static void event_process_v3(void *pv)
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
                l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
                while (curr_device)
                {
                    l_ezlopi_item_t *curr_item = curr_device->items;
                    while (curr_item)
                    {
                        curr_item->func(event->action, curr_item, event->arg, curr_item->user_arg);
                        curr_item = curr_item->next;
                        vTaskDelay(5 / portTICK_RATE_MS);
                    }

                    curr_device = curr_device->next;
                }

                free(event);
                event = NULL;
            }
        }
        else
        {
            vTaskDelay(5);
        }
    }
}

#if 0 // v2.x
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
#endif
