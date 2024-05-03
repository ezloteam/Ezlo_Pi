#include <string.h>

#include "esp_heap_trace.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_util_trace.h"
#include "ezlopi_util_version.h"
#include "ezlopi_util_version.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_registration.h"

#include "ezlopi_core_event_group.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_websocket_client.h"
#include "ezlopi_core_ezlopi_broadcast.h"
#include "ezlopi_core_processes.h"

#include "ezlopi_service_webprov.h"

static void registration_process(void* pv);

void registration_init(void)
{
    TaskHandle_t ezlopi_cloud_registration_process_task_handle = NULL;
    xTaskCreate(registration_process, "registration_process", EZLOPI_CLOUD_REGISTRATION_PROCESS_STACK_DEPTH, NULL, 2, &ezlopi_cloud_registration_process_task_handle);
    ezlopi_core_process_set_process_info(ENUM_EZLOPI_CLOUD_REGISTRATION_PROCESS_STACK, &ezlopi_cloud_registration_process_task_handle, EZLOPI_CLOUD_REGISTRATION_PROCESS_STACK_DEPTH);
}

void register_repeat(cJSON* cj_request, cJSON* cj_response)
{
    registration_init();
}

void registered(cJSON* cj_request, cJSON* cj_response)
{
    TRACE_S("Device registration successful.");
    ezlopi_event_group_set_event(EZLOPI_EVENT_NMA_REG);
}


static void registration_process(void* pv)
{

    char mac_str[18];
    uint8_t mac_addr[6];

    esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
    snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
        mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    bool flag_break = false;
    while (1)
    {
        cJSON* cj_register = cJSON_CreateObject();
        if (cj_register)
        {
            cJSON_AddStringToObject(cj_register, "id", "__ID__");
            cJSON_AddStringToObject(cj_register, ezlopi_method_str, "register");

            cJSON* cj_params = cJSON_AddObjectToObject(cj_register, ezlopi_params_str);

            if (cj_params)
            {
                cJSON_AddStringToObject(cj_params, ezlopi_firmware_str, VERSION_STR);
                cJSON_AddNumberToObject(cj_params, "timeOffset", 20700);
                cJSON_AddStringToObject(cj_params, "media", "radio");
                cJSON_AddStringToObject(cj_params, "hubType", "32.1");
                cJSON_AddStringToObject(cj_params, "mac_address", mac_str);
                cJSON_AddNumberToObject(cj_params, "maxFrameSize", (20 * 1024));
            }

            if (!ezlopi_core_ezlopi_broadcast_add_to_queue(cj_register))
            {
                cJSON_Delete(cj_register);
            }

            e_ezlopi_event_t events = ezlopi_get_event_bit_status();
            if ((EZLOPI_EVENT_NMA_REG & events) == EZLOPI_EVENT_NMA_REG)
            {
                flag_break = true;
            }
        }
        if (flag_break)
        {
            break;
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    ezlopi_core_process_set_is_deleted(ENUM_EZLOPI_CLOUD_REGISTRATION_PROCESS_STACK);
    vTaskDelete(NULL);

}
