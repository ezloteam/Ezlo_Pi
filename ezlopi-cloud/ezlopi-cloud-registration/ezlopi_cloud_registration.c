#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_util_version.h"
#include "ezlopi_util_trace.h"
#include "ezlopi_util_version.h"
#include "ezlopi_cloud_registration.h"
#include "ezlopi_service_webprov.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_core_websocket_client.h"
#include "ezlopi_core_event_group.h"

static void registration_process(void *pv);

void registration_init(void)
{
    xTaskCreate(registration_process, "registration_process", 2 * 2048, NULL, 2, NULL);
}

void register_repeat(cJSON *cj_request, cJSON *cj_response)
{
    registration_init();
}

void registered(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_method_str, cJSON_GetObjectItem(cj_request, ezlopi_method_str));
    TRACE_S("Device registration successful.");
    ezlopi_event_group_set_event(EZLOPI_EVENT_NMA_REG);
}

static void registration_process(void *pv)
{
    cJSON *cj_register = cJSON_CreateObject();
    if (cj_register)
    {
        char mac_str[18];
        uint8_t mac_addr[6];

        esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
        snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
                 mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

        cJSON_AddStringToObject(cj_register, "id", "__ID__");
        cJSON_AddStringToObject(cj_register, ezlopi_method_str, "register");
        cJSON *cj_params = cJSON_AddObjectToObject(cj_register, ezlopi_params_str);
        if (cj_params)
        {
            cJSON_AddStringToObject(cj_params, ezlopi_firmware_str, VERSION_STR);
            cJSON_AddNumberToObject(cj_params, "timeOffset", 18000);
            cJSON_AddStringToObject(cj_params, "media", "radio");
            cJSON_AddStringToObject(cj_params, "hubType", "32.1");
            cJSON_AddStringToObject(cj_params, "mac_address", mac_str);
            cJSON_AddNumberToObject(cj_params, "maxFrameSize", (20 * 1024));
        }

        while (false == ezlopi_websocket_client_is_connected())
        {
            vTaskDelay(200 / portTICK_RATE_MS);
        }

        while (0 >= ezlopi_event_group_wait_for_event(EZLOPI_EVENT_NMA_REG, 2000, true))
        {
            web_provisioning_send_to_nma_websocket(cj_register, TRACE_TYPE_B);
        }

        cJSON_Delete(cj_register);
    }

    vTaskDelete(NULL);
}
