#include <string.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include "ezlopi_util_trace.h"
#include "ezlopi_util_version.h"
#include "ezlopi_util_version.h"

#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_registration.h"

#include "ezlopi_core_broadcast.h"
#include "ezlopi_core_processes.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_websocket_client.h"

#include "ezlopi_service_webprov.h"

static TaskHandle_t __registration_task_handle = NULL;

static void registration_process(void* pv);

void registration_init(void)
{
    if (NULL == __registration_task_handle)
    {
        xTaskCreate(registration_process, "registration_process", EZLOPI_CLOUD_REGISTRATION_PROCESS_STACK_DEPTH, NULL, 2, &__registration_task_handle);
        ezlopi_core_process_set_process_info(ENUM_EZLOPI_CLOUD_REGISTRATION_PROCESS_STACK, &__registration_task_handle, EZLOPI_CLOUD_REGISTRATION_PROCESS_STACK_DEPTH);
    }
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
    cJSON* cj_register = cJSON_CreateObject(__FUNCTION__);
    if (cj_register)
    {
        char mac_str[18];
        uint8_t mac_addr[6];

        esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
        snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
            mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

        cJSON_AddStringToObject(__FUNCTION__, cj_register, "id", "__ID__");
        cJSON_AddStringToObject(__FUNCTION__, cj_register, ezlopi_method_str, "register");

        cJSON* cj_params = cJSON_AddObjectToObject(__FUNCTION__, cj_register, ezlopi_params_str);

        if (cj_params)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_firmware_str, VERSION_STR);
            cJSON_AddNumberToObject(__FUNCTION__, cj_params, "timeOffset", 20700);
            cJSON_AddStringToObject(__FUNCTION__, cj_params, "media", "radio");
            cJSON_AddStringToObject(__FUNCTION__, cj_params, "hubType", "32.1");
            cJSON_AddStringToObject(__FUNCTION__, cj_params, "mac_address", mac_str);
            cJSON_AddNumberToObject(__FUNCTION__, cj_params, "maxFrameSize", (20 * 1024));
        }

        while (ezlopi_event_group_wait_for_event(EZLOPI_EVENT_NMA_REG, 5000, false) <= 0)
        {
            cJSON* cj_register_dup = cJSON_CreateObjectReference(__FUNCTION__, cj_register->child);
            if (cj_register_dup)
            {
                CJSON_TRACE("----------------- sent to broadcast - cj_register_dup", cj_register_dup);
                if (!ezlopi_core_broadcast_add_to_queue(cj_register_dup))
                {
                    TRACE_E("Error adding to broadcast queue!");
                    cJSON_Delete(__FUNCTION__, cj_register_dup);
                }
            }
        }

        cJSON_Delete(__FUNCTION__, cj_register);
    }

    __registration_task_handle = NULL;
    ezlopi_core_process_set_is_deleted(ENUM_EZLOPI_CLOUD_REGISTRATION_PROCESS_STACK);
    vTaskDelete(NULL);

}

void ezpi_cloud_dummy()
{
    TRACE_S("I am just a dummy");
}