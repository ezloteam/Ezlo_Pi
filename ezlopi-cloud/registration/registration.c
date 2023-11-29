#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "version.h"
#include "trace.h"
#include "version.h"
#include "registration.h"
#include "web_provisioning.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_websocket_client.h"

static volatile uint32_t is_registered = 0;
static void registration_process(void *pv);

void registration_init(void)
{
    is_registered = 0;
    xTaskCreate(registration_process, "registration_process", 2 * 2048, NULL, 2, NULL);
}

void register_repeat(cJSON *cj_request, cJSON *cj_response)
{
    is_registered = 0;
    registration_init();
}

void registered(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    TRACE_I("Device registration successful.");
    is_registered = 1;
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
        cJSON_AddStringToObject(cj_register, "method", "register");
        cJSON *cj_params = cJSON_AddObjectToObject(cj_register, "params");
        if (cj_params)
        {
            cJSON_AddStringToObject(cj_params, "firmware", VERSION_STR);
            cJSON_AddNumberToObject(cj_params, "timeOffset", 18000);
            cJSON_AddStringToObject(cj_params, "media", "radio");
            cJSON_AddStringToObject(cj_params, "hubType", "32.1");
            cJSON_AddStringToObject(cj_params, "mac_address", mac_str);
            cJSON_AddNumberToObject(cj_params, "maxFrameSize", 4096);
        }

        while (false == ezlopi_websocket_client_is_connected())
        {
            vTaskDelay(200 / portTICK_RATE_MS);
        }

        while (0 == is_registered)
        {
            web_provisioning_send_to_nma_websocket(cj_register, TRACE_TYPE_D);
            vTaskDelay(2000 / portTICK_RATE_MS);
        }

        cJSON_Delete(cj_register);
    }

    vTaskDelete(NULL);
}
