#if 0
#include "ezlopi_core_event_queue.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "EZLOPI_USER_CONFIG.h"

#include "ezlopi_core_errors.h"

static QueueHandle_t generic_queue = NULL;

ezlopi_error_t ezlopi_event_queue_init(void)
{
    ezlopi_error_t error = EZPI_SUCCESS;
    if (NULL == generic_queue)
    {
        generic_queue = xQueueCreate(20, sizeof(s_ezlo_event_t *));
        error = (NULL != generic_queue) ? error : EZPI_ERR_EVENT_QUEUE_INIT_FAILED;
    }
    return error;
}

ezlopi_error_t ezlopi_event_queue_send(s_ezlo_event_t *event_data, int from_isr)
{
    ezlopi_error_t error = EZPI_ERR_EVENT_QUEUE_UNINITIALIZED;

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
                    ezlopi_free(__FUNCTION__, tmp_evt_data->arg);
                }
                ezlopi_free(__FUNCTION__, tmp_evt_data);
            }
        }

        BaseType_t send_error = pdTRUE;
        if (from_isr)
        {
            BaseType_t xHigherPriorityTaskWoken = pdFALSE;
            send_error = xQueueSendFromISR(generic_queue, &event_data, &xHigherPriorityTaskWoken);
        }
        else
        {
            send_error = xQueueSend(generic_queue, &event_data, 0);
        }
        error = pdTRUE == send_error ? EZPI_SUCCESS : EZPI_ERR_EVENT_QUEUE_RECV_SEND_ERROR;
    }

    return error;
}

ezlopi_error_t ezlopi_event_queue_receive(s_ezlo_event_t **event_data, int time_out_ms)
{
    ezlopi_error_t error = EZPI_ERR_EVENT_QUEUE_UNINITIALIZED;
    if (generic_queue)
    {
        error = (pdTRUE == xQueueReceive(generic_queue, event_data, (time_out_ms / portTICK_RATE_MS))) ? EZPI_SUCCESS : EZPI_ERR_EVENT_QUEUE_RECV_SEND_ERROR;
    }
    return error;
}
#endif