#include "string.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_wifi.h"
#include "ezlopi_event_group.h"

#include "trace.h"
#include "firmware.h"
#include "ota_service.h"
#include "web_provisioning.h"

static void ota_service_process(void *pv);

void ota_service_init(void)
{
    xTaskCreate(ota_service_process, "ota-service-process", 2 * 2048, NULL, 2, NULL);
}
static void ota_service_process(void *pv)
{
    ezlopi_wait_for_wifi_to_connect(portTICK_RATE_MS);
    ezlopi_event_group_set_event(EZLOPI_EVENT_OTA);
    while (1)
    {
        int ret = ezlopi_event_group_wait_for_event(EZLOPI_EVENT_OTA, 30 * 1000, 1);
        TRACE_D("Configuration Selection %d", ret);
        if (-1 != ret)
        {
            TRACE_D("Sending firmware check request...");
            uint32_t message_counter = web_provisioning_get_message_count();
            cJSON *firmware_info_request = firmware_send_firmware_query_to_nma_server(message_counter);
            if (NULL != firmware_info_request)
            {
                char *data_to_send = cJSON_Print(firmware_info_request);
                cJSON_Delete(firmware_info_request);
                firmware_info_request = NULL;

                if (data_to_send)
                {
                    cJSON_Minify(data_to_send);
                    ret = web_provisioning_send_str_data_to_nma_websocket(data_to_send, TRACE_TYPE_D);
                    free(data_to_send);
                }
            }
        }
        else
        {
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
    }
}
