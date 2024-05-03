#include <string.h>
#include <bootloader_random.h>

#include "cjext.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_wifi.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_ezlopi_broadcast.h"

#include "ezlopi_cloud_ota.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_ota.h"
#include "ezlopi_service_webprov.h"
#include "ezlopi_core_ezlopi_broadcast.h"
#include "ezlopi_core_processes.h"


#if defined(CONFIG_EZPI_ENABLE_OTA)
static volatile bool __ota_busy = false;

static void ota_service_process(void* pv);

bool ezlopi_service_ota_get_busy_state(void)
{
    return __ota_busy;
}

void ezlopi_service_ota_init(void)
{
    TaskHandle_t ezlopi_service_ota_process_task_handle = NULL;
    xTaskCreate(ota_service_process, "ota-service-process", EZLOPI_SERVICE_OTA_PROCESS_TASK_DEPTH, NULL, 2, &ezlopi_service_ota_process_task_handle);
    ezpi_core_process_set_process_info(ENUM_EZLOPI_SERVICE_OTA_PROCESS_TASK, &ezlopi_service_ota_process_task_handle, EZLOPI_SERVICE_OTA_PROCESS_TASK_DEPTH);
}

static void ota_service_process(void* pv)
{
    ezlopi_wait_for_wifi_to_connect(portMAX_DELAY);
    ezlopi_event_group_set_event(EZLOPI_EVENT_OTA);
    vTaskDelay(5000 / portTICK_RATE_MS);

    while (1)
    {
        __ota_busy = true;
        int ret_nma_reg = ezlopi_event_group_wait_for_event(EZLOPI_EVENT_NMA_REG, 60000, false);
        int ret_ota = ezlopi_event_group_wait_for_event(EZLOPI_EVENT_OTA, 86400 * 1000, 1); // 86400 seconds in a day (24 hrs)

        TRACE_D("Configuration Selection NMA Reg: %d", ret_nma_reg);
        TRACE_D("Configuration Selection OTA Trigger : %d", ret_ota);

        if ((-1 != ret_nma_reg) || (-1 != ret_ota))
        {
            TRACE_D("Sending firmware check request...");
            // uint32_t message_counter = ezlopi_service_web_provisioning_get_message_count();
            cJSON* cj_firmware_info_request = firmware_send_firmware_query_to_nma_server(esp_random());

            // CJSON_TRACE("----------------- broadcasting - cj_firmware_info_request", cj_firmware_info_request);

            if (0 == ezlopi_core_ezlopi_broadcast_add_to_queue(cj_firmware_info_request))
            {
                cJSON_Delete(__FUNCTION__, cj_firmware_info_request);
            }
            __ota_busy = false; // must clear immediately ; if OTA-event is serviced
        }
        else
        {
            __ota_busy = false; // must clear before the delay
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
    }
}
#endif // CONFIG_EZPI_ENABLE_OTA