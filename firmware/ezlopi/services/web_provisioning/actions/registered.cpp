#include "frozen.h"
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "websocket_client.h"
#include "factory_info.h"
#include "devices_common.h"
#include "esp_netif_types.h"
#include "wifi.h"
#include "debug.h"

using namespace std;

static volatile uint32_t reg_status = 0;
static uint32_t device_value;
const char *type = "device"; // device type
const char *controller_serial = "100003933";
const char *device_mac = "a2:d6:e0:de:e5:bb";

static void register_to_cloud(void *pv);

uint32_t get_registration_status(void)
{
    return reg_status;
}

// {"id":"61a78618-c60d-431e-987e-e8489542b401","jsonrpc":"1.0","method":"hub.data.list","params":{"settings":{"names":["first_start"],"fields":{"include":["value"]}}}}

string set_device_value(const char *data, uint32_t len, struct json_token *method_tok, uint32_t msg_count)
{
    struct json_token j_msg_id = JSON_INVALID_TOKEN;
    if (json_scanf(data, len, "{_id: %T}", &j_msg_id))
    {
        if (0 == strncmp(j_msg_id.ptr, controller_serial, strlen(controller_serial)))
        {
            json_scanf(data, len, "{value: %d}", &device_value);
        }
    }

    return "";
}

string set_device_name(const char *data, uint32_t len, struct json_token *method_tok, uint32_t msg_count)
{
    char data_buf[200] = {'\0'};
    struct json_token msg_id = JSON_INVALID_TOKEN;
    json_scanf(data, len, "{id: %T}", &msg_id);

    snprintf(data_buf, 200, "{\"api\": \"1.0\",\"msg_id\":%d,\"error\": null,\"id\": \"%.*s\",\"result\": {}}", msg_count, msg_id.len, msg_id.ptr);

    string ret = data_buf;
    return ret;
}

#if 0
const char *dev_list1 = "{\"method\":\"hub.devices.list\",\"result\":{\"devices\":[\0";
const char *dev_list2 = "{\"_id\":\"%s\","
                        "\"deviceTypeId\":\"plughub\",\"parentDeviceId\":\"\",\"category\""
                        ":\"switch\",\"subcategory\":\"in_wall\",\"gatewayId\":\"plughub\",\"name\":\"%s"
                        "\",\"type\":\"switch.inwall\",\"batteryPowered\":"
                        "false,\"reachable\":true,\"persistent\":true,\"serviceNotification\":false,\"roomId\":"
                        "\"\",\"security\":\"\",\"ready\":true}\0";
const char *dev_list3 = "]},\"error\":null,\"id\":\"%.*s\"%s}\0";
#endif

static const char *json_scenes_list = "{\"method\":\"%.*s\",\"result\":{\"scenes\":[]},\"error\":null,\"id\":\"%.*s\"%s}";

string send_scenes_list(const char *data, uint32_t len, struct json_token *method_tok)
{
    string ret = "";
    // char buf1[1024] = {'\0'};
    char send_buf[2048] = {'\0'};

    struct json_token msg_id;
    json_scanf(data, len, "{id: %T}", &msg_id);

    sprintf(send_buf, json_scenes_list, method_tok->len, method_tok->ptr, msg_id.len, msg_id.ptr, "");

    ret = send_buf;

    // TRACE_D("Response[hub.scenes.list]: %s\r\n", ret.c_str());
    TRACE_B(">> WS Tx - '%.*s' [%d]\n\rdata-%s\n\r", method_tok->len, method_tok->ptr, ret.length(), ret.c_str());

    return ret;
}

const char *dev_1_start = "{\"method\":\"hub.devices.list\",\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{\"devices\":[";
// const char *dev_1_start = "{\"method\":\"hub.devices.list\",\"msg_id\":\"%d\",\"api\":\"1.0\",\"error\":null,\"id\":\"%.*s\",\"result\":{\"devices\":[";
const char *dev_1_devs = "{\"_id\":\"d%s\",\"batteryPowered\":false,\"category\":\"switch\",\"subcategory\":\"in_wall\","
                         "\"deviceTypeId\":\"plughub\",\"gatewayId\":\"plughub\",\"name\":\"%s\","
                         "\"type\":\"switch.inwall\",\"reachable\":true,\"persistent\":true,"
                         "\"serviceNotification\":false,\"roomId\":\"\",\"security\":\"\",\"ready\":true}";
const char *dev_1_end = "]}}";
string send_device_list(const char *data, uint32_t len, struct json_token *method_tok)
{
    string ret = "";
    char send_buf[2048] = {'\0'};
    // factory_info *factory = factory_info::get_instance();
    devices_common *devices_ctx = devices_common::get_instance();
    s_device_properties_t *devices = devices_ctx->device_list();

    struct json_token msg_id = JSON_INVALID_TOKEN;
    json_scanf(data, len, "{id: %T}", &msg_id);

    snprintf(send_buf, sizeof(send_buf), dev_1_start, msg_id.len, msg_id.ptr);

    if (devices[0].name[0])
    {
        for (int i = 0; i < MAX_DEV; i++)
        {
            int len_b = strlen(send_buf);
            snprintf(&send_buf[len_b], sizeof(send_buf) - len_b, dev_1_devs,
                     sizeof(devices[i].device_id), devices[i].device_id,
                     sizeof(devices[i].name), devices[i].name);
            if (i < MAX_DEV - 1)
            {
                if (devices[i + 1].name[0])
                {
                    len_b = strlen(send_buf);
                    send_buf[len_b] = ',';
                    send_buf[len_b + 1] = 0;
                }
                else
                {
                    break;
                }
            }
        }
    }

    int len_b = strlen(send_buf);
    snprintf(&send_buf[len_b], sizeof(send_buf) - len_b, "%s", dev_1_end);
    len_b = strlen(send_buf);
    send_buf[len_b] = 0;

    ret = send_buf;

    TRACE_D(">> WS Tx - '%.*s' [%d]\n\r%s", method_tok->len, method_tok->ptr, ret.length(), ret.c_str());

    return ret;
}

const char *json_items1 = "{\"method\":\"hub.items.list\",\"result\":{\"items\":[";
const char *json_itemsn =
    "{"
    "\"_id\":\"a%s\","
    "\"deviceId\":\"%s\","
    "\"hasGetter\":%s,"
    "\"hasSetter\":%s,"
    "\"name\":\"%s\","
    "\"show\":true,"
    "\"valueType\":\"%s\","
    "\"valueFormatted\":\"%s\","
    "\"value\":\"%s\","
    "\"status\": \"synced\","
    "\"syncNotification\":true"
    "}";
#define json_items2 "]},\"error\":null,\"id\":\"%.*s\"%s}"
const char *json_room_list = "{\"method\":\"%.*s\",\"result\":[%s],\"error\":null,\"id\":\"%.*s\",\"sender\":%.*s}";

string send_hub_room_list(const char *payload, uint32_t len, struct json_token *method_tok)
{
    string ret = "";
    char send_buf[2048] = {'\0'};
    char tmp_buf1[500] = {'\0'};

    struct json_token msg_id;
    json_scanf(payload, len, "{id: %T}", &msg_id);

    struct json_token sender;
    int sender_status = json_scanf(payload, len, "{sender: %T}", &sender);

    devices_common *devices_ctx = devices_common::get_instance();
    s_device_properties_t *devise = devices_ctx->device_list();

    if (devise[0].name[0])
    {
        for (int i = 0; i < MAX_DEV; i++)
        {
            int len_d = strlen(tmp_buf1);
            sprintf(&tmp_buf1[len_d],
                    "{\"_id\": \"%s\", \"name\":\"%s\"}",
                    devise[i].roomId, "");

            if (i < MAX_DEV - 1)
            {
                if (devise[i + 1].name[0] != 0)
                {
                    len_d = strlen(tmp_buf1);
                    tmp_buf1[len_d] = ',';
                    tmp_buf1[len_d + 1] = 0;
                }
                else
                {
                    break;
                }
            }
        }
    }
    int len_d = strlen(send_buf);
    sprintf(&send_buf[len_d], json_room_list, method_tok->len, method_tok->ptr, tmp_buf1, msg_id.len, msg_id.ptr,
            sender_status ? sender.len : 2, sender_status ? sender.ptr : "{}");
    len_d = strlen(send_buf);
    send_buf[len_d] = 0;

    ret = send_buf;

    TRACE_B(">> WS Tx - '%.*s' [%d]\n\r%s", method_tok->len, method_tok->ptr, ret.length(), ret.c_str());

    return ret;
}

string respond_hub_items_list(const char *data, uint32_t len, struct json_token *method_tok)
{
    string ret = "";
    char send_buf[2048] = {'\0'};

    // devices_common *devices_ctx = devices_common::get_instance();
    // s_device_properties_t *devise = devices_ctx->device_list();

    struct json_token msg_id;
    json_scanf(data, len, "{id: %T}", &msg_id);

#if 0
    sprintf(send_buf, "%s", json_items1);

    if (devise[0].name[0])
    {
        for (int i = 0; i < MAX_DEV; i++)
        {
            int len_d = strlen(send_buf);
            sprintf(&send_buf[len_d],
                    json_itemsn,
                    "142F6C69",                 // devise[i].item_id,
                    "588b7eb528b12d03be86f36f", // devise[i].device_id,
                    "true",
                    "true",
                    "switch",
                    "bool",
                    devise[i].input_vol ? "true" : "false",
                    devise[i].input_vol ? "true" : "false");

            if (i < MAX_DEV - 1)
            {
                if (devise[i + 1].name[0] != 0)
                {
                    len_d = strlen(send_buf);
                    send_buf[len_d] = ',';
                    send_buf[len_d + 1] = 0;
                }
                else
                {
                    break;
                }
            }
        }
    }
    int len_d = strlen(send_buf);
    sprintf(&send_buf[len_d], json_items2, msg_id.len, msg_id.ptr, "");
    len_d = strlen(send_buf);
    send_buf[len_d] = 0;

    ret = send_buf;

    static const char *test =
        "{"
        "\"method\":\"hub.items.list\","
        "\"msg_id\":3364512107,"
        "\"result\":{"
        "\"items\":[{"
        "\"_id\":\"3811496\","
        "\"deviceId\":\"81915c2\","
        "\"deviceName\":\"SWITCH1\","
        "\"deviceArmed\":true,"
        "\"hasGetter\":true,"
        "\"hasSetter\":true,"
        "\"name\":\"switch\","
        "\"show\":true,"
        "\"valueType\":\"bool\","
        "\"value\":false,"
        "\"valueFormatted\":\"false\","
        "\"status\":\"synced\""
        "}]"
        "},"
        "\"error\":null,"
        "\"id\":\"%.*s\","
        "\"sender\":%.*s"
        "}";
#endif
    static const char *test =
        "{\"method\":\"hub.items.list\","
        "\"msg_id\":3364512107,"
        "\"result\":{\"items\":[{\"_id\":\"142F6C69\","
        "\"deviceId\":\"588b7eb528b12d03be86f36f\","
        "\"deviceName\": \"SWITCH1\","
        "\"deviceArmed\":true,"
        "\"hasGetter\":true,"
        "\"hasSetter\":true,"
        "\"name\":\"switch\","
        "\"show\":true,"
        "\"valueType\":\"bool\","
        "\"value\":false, \"valueFormatted\":\"false\","
        "\"status\":\"synced\"}]},"
        "\"error\":null,"
        "\"id\":\"%.*s\",\"sender\":%.*s}";

    ret = "";
    struct json_token sender = JSON_INVALID_TOKEN;
    if (json_scanf(data, len, "{sender:%T}", &sender))
    {
        snprintf(send_buf, sizeof(send_buf), test, msg_id.len, msg_id.ptr, sender.len, sender.ptr);
    }
    else
    {
        snprintf(send_buf, sizeof(send_buf), test, msg_id.len, msg_id.ptr, 2, "{}");
    }

    // int len_d = strlen(send_buf);

    ret = send_buf;

    TRACE_B(">> WS Tx - '%.*s' [%d]\n\r%s", method_tok->len, method_tok->ptr, ret.length(), ret.c_str());

    return ret;
}

static const char *network_1_start = "{\"error\":null,\"msg_id\":%d,\"id\":\"%.*s\",\"method\":\"hub.network.get\",\"result\":{\"interfaces\":[";
static const char *network_1_net = "{\"_id\":\"wifi\",\"enabled\":\"auto\",\"hwaddr\":\"%s\",\"internetAvailable\":true,"
                                   "\"ipv4\":{\"gateway\":\"" IPSTR "\",\"ip\":\"" IPSTR "\",\"mask\":\"" IPSTR "\",\"mode\":\"dhcp\"},"
                                   "\"network\":\"wan\",\"status\":\"up\",\"type\":\"wifi\"}";
static const char *network_1_end = "]},\"sender\":%.*s}";

string respond_hub_network_get(const char *data, uint32_t len, struct json_token *method_tok, uint32_t msg_count)
{
    string ret = "";
    char mac_str[20];
    uint8_t mac_addr[6];
    char send_buf[2048] = {'\0'};
    esp_netif_ip_info_t *ip_info = wifi_get_ip_infos();

    esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
    snprintf(mac_str, sizeof(mac_str), "%02x:%02x:%02x:%02x:%02x:%02x",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    struct json_token msg_id = JSON_INVALID_TOKEN;
    json_scanf(data, len, "{id: %T}", &msg_id);

    struct json_token sender = JSON_INVALID_TOKEN;
    int sender_state = json_scanf(data, len, "{sender: %T}", &sender);

    snprintf(send_buf + strlen(send_buf), sizeof(send_buf) - strlen(send_buf), network_1_start, msg_count, msg_id.len, msg_id.ptr);
    snprintf(send_buf + strlen(send_buf), sizeof(send_buf) - strlen(send_buf), network_1_net, mac_str,
             IP2STR(&ip_info->gw),       // ip4addr_ntoa((const ip4_addr_t *)&ip_info->gw.addr),
             IP2STR(&ip_info->ip),       // ip4addr_ntoa((const ip4_addr_t *)&ip_info->ip.addr),
             IP2STR(&ip_info->netmask)); // ip4addr_ntoa((const ip4_addr_t *)&ip_info->netmask.addr));

    snprintf(send_buf + strlen(send_buf), sizeof(send_buf) - strlen(send_buf), network_1_end, sender_state ? sender.len : 2, sender_state ? sender.ptr : "{}");

    ret = send_buf;
    TRACE_B(">> WS Tx - '%.*s' [%d]\n\r%s", method_tok->len, method_tok->ptr, ret.length(), ret.c_str());
    return ret;
}

string registered_action(const char *data, uint32_t len, struct json_token *method)
{
    string ret = "";
    reg_status = 1;
    TRACE_D("Device registered successfully.\r\n");
    return ret;
}
void start_registration_process(websocket_client *ws_client)
{
    xTaskCreate(register_to_cloud, "cloud registration process", 2048, (void *)ws_client, 2, NULL);
}
static void register_to_cloud(void *pv)
{
    char reg_str[300] = "";
    websocket_client *ws_client = (websocket_client *)pv;

    uint8_t mac_addr[6];
    char mac_str[18];
    esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
    snprintf(mac_str, sizeof(mac_str), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    snprintf(reg_str, sizeof(reg_str),
             "{\"id\":\"%u\",\"method\":\"register\",\"params\":"
             "{\"firmware\":\"0.1\",\"timeOffset\":18000, \"media\":\"radio\","
             "\"hubType\":\"32.1\",\"mac_address\":\"%s\"}}",
             esp_random(), device_mac);

    while (false == ws_client->is_connected())
    {
        vTaskDelay(100);
    }

    while (0 == reg_status)
    {
        TRACE_B(">> WS Tx 'register' [%d]:\r\n%s", strlen(reg_str), reg_str);
        ws_client->send(reg_str);
        vTaskDelay(10000);
    }

    vTaskDelete(NULL);
}