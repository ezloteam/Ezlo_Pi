#include <string.h>
#include <stdint.h>
#include <time.h>

#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_event_queue.h"
#include "ezlopi_core_processes.h"

#include "ezlopi_service_timer.h"

static void event_process_v3(void* pv);

void timer_service_init(void)
{
    TaskHandle_t ezlopi_service_timer_task_handle = NULL;
    // xTaskCreate(event_process, "event_process", EZLOPI_SERVICE_TIMER_TASK_DEPTH, NULL, 4, &ezlopi_service_timer_task_handle);
    xTaskCreate(event_process_v3, "event_process_v3", EZLOPI_SERVICE_TIMER_TASK_DEPTH, NULL, 4, &ezlopi_service_timer_task_handle);
    ezlopi_core_process_set_process_info(ENUM_EZLOPI_SERVICE_TIMER_TASK, &ezlopi_service_timer_task_handle, EZLOPI_SERVICE_TIMER_TASK_DEPTH);
}

static void event_process_v3(void* pv)
{
    while (1)
    {
        s_ezlo_event_t* event = NULL;
        if (pdTRUE == ezlopi_event_queue_receive(&event, 2000 / portTICK_PERIOD_MS))
        {
            l_ezlopi_device_t* curr_device = ezlopi_device_get_head();
            while (curr_device)
            {
                l_ezlopi_item_t* curr_item = curr_device->items;
                while (curr_item)
                {
                    if (NULL != event)
                    {
                        curr_item->func(event->action, curr_item, event->arg, curr_item->user_arg);
                        free(event);
                        event = NULL;
                    }
                    else // in case of default we receive event == NULL, and we pass the action EZLOPI_ACTION_NOTIFY_1000_MS
                    {
                        curr_item->func(EZLOPI_ACTION_NOTIFY_1000_MS, curr_item, NULL, curr_item->user_arg);
                    }

                    curr_item = curr_item->next;
                    vTaskDelay(5 / portTICK_RATE_MS);
                }

                curr_device = curr_device->next;
            }
        }
        else
        {
            TRACE_E("failed to get timer-event");
            vTaskDelay(5);
        }
    }
}
