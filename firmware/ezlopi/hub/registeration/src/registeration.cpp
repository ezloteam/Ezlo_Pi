#include <string>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "websocket_client.h"
#include "registeration.h"
#include "debug.h"

using namespace std;

static volatile uint32_t is_registered = 0;
registeration *registeration::registeration_ = nullptr;

registeration *registeration::get_instance(void)
{
    if (nullptr == registeration_)
    {
        registeration_ = new registeration();
    }

    return registeration_;
}

void registeration::registeration_process(void *pv)
{
    char mac_str[18];
    uint8_t mac_addr[6];
    char reg_str[300] = "";
    websocket_client *ws_client = (websocket_client *)pv;

    esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
    snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    snprintf(reg_str, sizeof(reg_str),
             "{\"id\":\"%u\",\"method\":\"register\",\"params\":"
             "{\"firmware\":\"0.1\",\"timeOffset\":18000, \"media\":\"radio\","
             "\"hubType\":\"32.1\",\"mac_address\":\"%s\"}}",
             esp_random(), "a2:97:1e:74:0b:52");

    while (false == ws_client->is_connected())
    {
        vTaskDelay(200);
    }

    while (0 == is_registered)
    {
        /*Send registeration packet in some interval*/
        ws_client->send(reg_str);
        TRACE_D(">>>> WSS-TS: 'register':\r\n%s", reg_str);
        vTaskDelay(2000);
    }

    vTaskDelete(NULL);
}

void registeration::init(websocket_client *ws_client)
{
    is_registered = 0;
    xTaskCreate(registeration_process, "registeration_process", 2 * 2048, (void *)ws_client, 2, NULL);
}

string registeration::registered(const char *payload, uint32_t len, struct json_token *method, uint32_t msg_count)
{
    string ret = "";
    TRACE_I("Device registeration successful.");
    is_registered = 1;
    return ret;
}