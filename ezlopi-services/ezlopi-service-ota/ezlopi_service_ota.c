#include "string.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_core_wifi.h"
#include "ezlopi_core_event_group.h"

#include "trace.h"
#include "ezlopi_cloud_ota.h"
#include "ezlopi_service_ota.h"
#include "ezlopi_service_webprov.h"

static void ota_service_process(void *pv);

void ota_service_init(void)
{
    xTaskCreate(ota_service_process, "ota-service-process", 2 * 2048, NULL, 2, NULL);
}
static void ota_service_process(void *pv)
{
    ezlopi_wait_for_wifi_to_connect(portMAX_DELAY);
    ezlopi_event_group_set_event(EZLOPI_EVENT_OTA);
    vTaskDelay(5000 / portTICK_RATE_MS);
    while (1)
    {

        int ret_nma_reg = ezlopi_event_group_wait_for_event(EZLOPI_EVENT_NMA_REG, 60000, false);
        int ret_ota = ezlopi_event_group_wait_for_event(EZLOPI_EVENT_OTA, 86400 * 1000, 1); // 86400 seconds in a day (24 hrs)
        TRACE_D("Configuration Selection NMA Reg: %d", ret_nma_reg);
        TRACE_D("Configuration Selection OTA Trigger : %d", ret_ota);
        if ((-1 != ret_nma_reg) || (-1 != ret_ota))
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
                    ret_ota = web_provisioning_send_str_data_to_nma_websocket(data_to_send, TRACE_TYPE_D);
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
