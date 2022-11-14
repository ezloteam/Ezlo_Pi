#include "ezlopi_event_queue.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "trace.h"

static QueueHandle_t generic_queue = NULL;

void ezlopi_event_queue_init(void)
{
    if (NULL == generic_queue)
    {
        generic_queue = xQueueCreate(20, sizeof(s_ezlo_event_t *));
    }
}

int ezlopi_event_queue_send(s_ezlo_event_t *event_data, int from_isr)
{
    int ret = 0;

    if (NULL != generic_queue)
    {
        if (xQueueIsQueueFullFromISR(generic_queue)) // 'FromISR' or not
        {
            s_ezlo_event_t *tmp_evt_data = NULL;
            if (from_isr)
            {
                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                xQueueReceiveFromISR(generic_queue, &tmp_evt_data, &xHigherPriorityTaskWoken);
            }
            else
            {
                xQueueReceive(generic_queue, &tmp_evt_data, 0);
            }

            if (tmp_evt_data)
            {
                if (tmp_evt_data->arg)
                {
                    free(tmp_evt_data->arg);
                }
                free(tmp_evt_data);
            }
        }

        if (from_isr)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            ret = xQueueSendFromISR(generic_queue, &event_data, &xHigherPriorityTaskWoken);
        }
        else
        {
            ret = xQueueSend(generic_queue, &event_data, 0);
        }
    }

    return ret;
}

int ezlopi_event_queue_receive(s_ezlo_event_t **event_data, int time_out_ms)
{
    int ret = 0;
    if (generic_queue)
    {
        ret = xQueueReceive(generic_queue, event_data, (time_out_ms / portTICK_RATE_MS));
    }
    return ret;
}