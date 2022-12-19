#include <string.h>
#include "esp_wifi.h"
#include "esp_netif_types.h"
#include "network.h"
#include "ezlopi_wifi.h"
#include "trace.h"
#include "frozen.h"

#include "cJSON.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"

#define MAC_ADDR_EXPANDED(mac) mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]

cJSON *network_get(const char *payload, uint32_t len, struct json_token *method_tok, uint32_t msg_count)
{
    cJSON *cjson_response = cJSON_CreateObject();
    cJSON *cjson_request = cJSON_ParseWithLength(payload, len);

    if (cjson_request)
    {
        cJSON *id = cJSON_GetObjectItem(cjson_request, ezlopi_id_str);
        cJSON *sender = cJSON_GetObjectItem(cjson_request, ezlopi_sender_str);

        if (cjson_response)
        {
            cJSON_AddStringToObject(cjson_response, ezlopi_key_method_str, method_hub_network_get);
            cJSON_AddNumberToObject(cjson_response, ezlopi_msg_id_str, msg_count);
            cJSON_AddStringToObject(cjson_response, ezlopi_id_str, id ? (id->valuestring ? id->valuestring : "") : "");
            cJSON_AddStringToObject(cjson_response, ezlopi_sender_str, sender ? (sender->valuestring ? sender->valuestring : "{}") : "{}");
            cJSON_AddNullToObject(cjson_response, "error");

            cJSON *cjson_result = cJSON_AddObjectToObject(cjson_response, "result");
            if (cjson_result)
            {
                cJSON *interfaces_array = cJSON_AddArrayToObject(cjson_result, "interfaces");
                if (interfaces_array)
                {
                    cJSON *wifi_properties = cJSON_CreateObject();
                    if (wifi_properties)
                    {
                        char tmp_string[64];
                        cJSON_AddStringToObject(wifi_properties, "_id", "wifi");
                        cJSON_AddStringToObject(wifi_properties, "enabled", "auto");

                        uint8_t mac_addr[6];
                        esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
                        snprintf(tmp_string, sizeof(tmp_string), "%02x:%02x:%02x:%02x:%02x:%02x", MAC_ADDR_EXPANDED(mac_addr));
                        cJSON_AddStringToObject(wifi_properties, "hwaddr", tmp_string);
                        cJSON_AddBoolToObject(wifi_properties, "internetAvailable", true);
                        cJSON_AddStringToObject(wifi_properties, "network", "wan");
                        cJSON_AddStringToObject(wifi_properties, "status", "up");
                        cJSON_AddStringToObject(wifi_properties, "type", "wifi");

                        cJSON *wifi_ipv4 = cJSON_CreateObject();
                        if (wifi_ipv4)
                        {
                            esp_netif_ip_info_t *ip_info = ezlopi_wifi_get_ip_infos();
                            snprintf(tmp_string, sizeof(tmp_string), IPSTR, IP2STR(&ip_info->gw));
                            cJSON_AddStringToObject(wifi_ipv4, "ip", tmp_string);
                            snprintf(tmp_string, sizeof(tmp_string), IPSTR, IP2STR(&ip_info->ip));
                            cJSON_AddStringToObject(wifi_ipv4, "mask", tmp_string);
                            snprintf(tmp_string, sizeof(tmp_string), IPSTR, IP2STR(&ip_info->netmask));
                            cJSON_AddStringToObject(wifi_ipv4, "gateway", tmp_string);
                            cJSON_AddStringToObject(wifi_ipv4, "mode", "dhcp");

                            if (!cJSON_AddItemToObjectCS(wifi_properties, "ipv4", wifi_ipv4))
                            {
                                cJSON_Delete(wifi_ipv4);
                                wifi_ipv4 = NULL;
                            }
                        }

                        if (!cJSON_AddItemToArray(interfaces_array, wifi_properties))
                        {
                            cJSON_Delete(wifi_properties);
                        }
                    }
                }
            }
        }

        cJSON_Delete(cjson_request);
    }

    return cjson_response;
}
