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
#include "ezlopi_event_group.h"

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
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    TRACE_I("Device registration successful.");
    ezlopi_event_group_set_event(EZLOPI_EVENT_NMA_REG);
}

static void registration_process(void *pv)
{
    char mac_str[18];
    uint8_t mac_addr[6];
    char reg_str[300] = "";

    esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
    snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    snprintf(reg_str, sizeof(reg_str),
             "{\"id\":\"%u\",\"method\":\"register\",\"params\":"
             "{\"firmware\":\"%s\",\"timeOffset\":18000, \"media\":\"radio\","
             "\"hubType\":\"32.1\",\"mac_address\":\"%s\",\"maxFrameSize\":4096}}",
             esp_random(), VERSION_STR, mac_str);

    cJSON *cjson_data = cJSON_Parse(reg_str);

    while (false == ezlopi_websocket_client_is_connected())
    {
        vTaskDelay(200 / portTICK_RATE_MS);
    }

    while (0 >= ezlopi_event_group_wait_for_event(EZLOPI_EVENT_NMA_REG, 2000 / portTICK_RATE_MS, false))
    {
        web_provisioning_send_to_nma_websocket(cjson_data, TRACE_TYPE_B);
        // vTaskDelay(2000 / portTICK_RATE_MS);
    }

    if (cjson_data)
    {
        cJSON_Delete(cjson_data);
    }

    vTaskDelete(NULL);
}
