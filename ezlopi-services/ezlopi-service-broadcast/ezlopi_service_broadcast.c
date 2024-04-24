#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "ezlopi_service_broadcast.h"
#include "ezlopi_core_ezlopi_broadcast.h"
#include "ezlopi_core_processes.h"

static QueueHandle_t __broadcast_queue = NULL;

static void __broadcast_process(void* pv);
static int ezlopi_service_broadcast_send_to_queue(char* data);


void ezlopi_service_broadcast_init(void)
{
    __broadcast_queue = xQueueCreate(sizeof(char*), 30);
    if (__broadcast_queue)
    {
        TaskHandle_t ezlopi_service_broadcast_task_handle = NULL;
        xTaskCreate(__broadcast_process, "broadcast-service", EZLOPI_SERVICE_BROADCAST_TASK_DEPTH, NULL, 2, &ezlopi_service_broadcast_task_handle);
        ezlopi_core_process_set_process_info(ENUM_EZLOPI_SERVICE_BROADCAST_TASK, &ezlopi_service_broadcast_task_handle, EZLOPI_SERVICE_BROADCAST_TASK_DEPTH);
        ezlopi_core_ezlopi_broadcast_methods_set_queue(ezlopi_service_broadcast_send_to_queue);
    }
}

static void __broadcast_process(void* pv) {
    while (1)
    {
        char* data = NULL;
        if (pdTRUE == xQueueReceive(__broadcast_queue, &data, portMAX_DELAY))
        {
            if (data)
            {
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                ezlopi_core_ezlopi_broadcast_execute(data);
                free(data);
            }
        }
    }
}

static int ezlopi_service_broadcast_send_to_queue(char* data)
{
    int ret = 0;
    if (__broadcast_queue && data)
    {
        if (xQueueIsQueueFullFromISR(__broadcast_queue))
        {
            char* tmp_data = NULL;
            if (pdTRUE == xQueueReceive(__broadcast_queue, &tmp_data, 0))
            {
                if (tmp_data)
                {
                    free(tmp_data);
                }
            }
        }

        char* tmp_data = data;
        if (pdTRUE == xQueueSend(__broadcast_queue, &tmp_data, 1000 / portTICK_PERIOD_MS))
        {
            ret = 1;
        }
    }
    return ret;
}