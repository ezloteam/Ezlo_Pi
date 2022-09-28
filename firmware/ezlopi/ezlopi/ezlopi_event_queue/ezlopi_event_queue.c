#include "ezlopi_event_queue.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

static QueueHandle_t generic_queue = NULL;

void ezlopi_event_queue_init(void)
{
    generic_queue = xQueueCreate(20, sizeof(s_ezlo_event_t));
}

int ezlopi_event_queue_send(s_ezlo_event_t *event_data, int from_isr)
{
    int ret = 0;

    if (xQueueIsQueueFullFromISR(generic_queue))
    {
        s_ezlo_event_t *tmp_evt_data = NULL;
        xQueueReceive(generic_queue, tmp_evt_data, 0);
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
        xQueueSendFromISR(generic_queue, event_data, &xHigherPriorityTaskWoken);
    }
    else
    {
        xQueueSend(generic_queue, event_data, 0);
    }
    return ret;
}

int ezlopi_event_queue_receive(s_ezlo_event_t **event_data, int time_out_ms)
{
    int ret = 0;
    ret = (pdTRUE == xQueueReceive(generic_queue, *event_data, (time_out_ms / portTICK_RATE_MS)) ? 1 : 0);
    return ret;
}