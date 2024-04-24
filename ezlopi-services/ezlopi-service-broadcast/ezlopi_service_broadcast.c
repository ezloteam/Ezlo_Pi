#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_service_broadcast.h"
#include "ezlopi_core_ezlopi_broadcast.h"

static QueueHandle_t __broadcast_queue = NULL;

static void __broadcast_process(void* pv);
static int ezlopi_service_broadcast_send_to_queue(cJSON* cj_broadcast_data);

void ezlopi_service_broadcast_init(void)
{
    __broadcast_queue = xQueueCreate(sizeof(char*), 10);
    if (__broadcast_queue)
    {
        xTaskCreate(__broadcast_process, "broadcast-service", 4 * 1024, NULL, 2, NULL);
        ezlopi_core_ezlopi_broadcast_methods_set_queue(ezlopi_service_broadcast_send_to_queue);
    }
}

static void __broadcast_process(void* pv)
{
    while (1)
    {
        cJSON* cj_data = NULL;
        if (pdTRUE == xQueueReceive(__broadcast_queue, &cj_data, portMAX_DELAY))
        {
            if (cj_data)
            {
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                ezlopi_core_ezlopi_broadcast_cjson(cj_data);
                cJSON_Delete(cj_data);
            }
        }
    }
}

static int ezlopi_service_broadcast_send_to_queue(cJSON* cj_broadcast_data)
{
    int ret = 0;

    if (__broadcast_queue && cj_broadcast_data)
    {
        if (xQueueIsQueueFullFromISR(__broadcast_queue))
        {
            cJSON* cj_tmp_data = NULL;
            if (pdTRUE == xQueueReceive(__broadcast_queue, &cj_tmp_data, 0))
            {
                if (cj_tmp_data)
                {
                    cJSON_Delete(cj_tmp_data);
                }
            }
        }

        cJSON* cj_data = cj_broadcast_data;
        if (pdTRUE == xQueueSend(__broadcast_queue, &cj_data, 1000 / portTICK_PERIOD_MS))
        {
            ret = 1;
        }
    }

    return ret;
}