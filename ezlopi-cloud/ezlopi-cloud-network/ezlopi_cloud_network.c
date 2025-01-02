#include <string.h>
#include "esp_wifi.h"
#include "esp_netif_types.h"
#include "ezlopi_cloud_network.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_ethernet.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_util_trace.h"

#include "cjext.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"

#define MAC_ADDR_EXPANDED(mac) mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]

void network_get(cJSON* cj_request, cJSON* cj_response)
{
    cJSON* cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON* interfaces_array = cJSON_AddArrayToObject(__FUNCTION__, cjson_result, ezlopi_interfaces_str);
        if (interfaces_array)
        {
            cJSON* wifi_properties = cJSON_CreateObject(__FUNCTION__);
            if (wifi_properties)
            {
                char tmp_string[54];
                cJSON_AddStringToObject(__FUNCTION__, wifi_properties, ezlopi__id_str, "wlan0");
                cJSON_AddStringToObject(__FUNCTION__, wifi_properties, ezlopi_enabled_str, ezlopi_auto_str);

                uint8_t mac_addr[6];
                esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
                snprintf(tmp_string, sizeof(tmp_string), "%02x:%02x:%02x:%02x:%02x:%02x", MAC_ADDR_EXPANDED(mac_addr));
                cJSON_AddStringToObject(__FUNCTION__, wifi_properties, ezlopi_hwaddr_str, tmp_string);
                cJSON_AddBoolToObject(__FUNCTION__, wifi_properties, ezlopi_internetAvailable_str, true);

                cJSON* wifi_ipv4 = cJSON_CreateObject(__FUNCTION__);
                ezlopi_wifi_status_t *wifi_status = ezlopi_wifi_status();
                if (wifi_ipv4)
                {
                    cJSON *cj_dns = cJSON_AddArrayToObject(__FUNCTION__, wifi_ipv4, "dns");
                    if (cj_dns)
                    {
                        memset(tmp_string, 0, sizeof(tmp_string));
                        snprintf(tmp_string, sizeof(tmp_string), IPSTR, IP2STR(&wifi_status->ip_info->gw));
                        cJSON_AddItemToArray(cj_dns, cJSON_CreateString(__FUNCTION__, tmp_string));
                    }

                    snprintf(tmp_string, sizeof(tmp_string), IPSTR, IP2STR(&wifi_status->ip_info->ip));
                    cJSON_AddStringToObject(__FUNCTION__, wifi_ipv4, ezlopi_ip_str, tmp_string);
                    snprintf(tmp_string, sizeof(tmp_string), IPSTR, IP2STR(&wifi_status->ip_info->netmask));
                    cJSON_AddStringToObject(__FUNCTION__, wifi_ipv4, ezlopi_mask_str, tmp_string);
                    snprintf(tmp_string, sizeof(tmp_string), IPSTR, IP2STR(&wifi_status->ip_info->gw));
                    cJSON_AddStringToObject(__FUNCTION__, wifi_ipv4, ezlopi_gateway_str, tmp_string);
                    cJSON_AddStringToObject(__FUNCTION__, wifi_ipv4, ezlopi_mode_str, ezlopi_dhcp_str);


                    if (!cJSON_AddItemToObjectCS(__FUNCTION__, wifi_properties, ezlopi_ipv4_str, wifi_ipv4))
                    {
                        cJSON_Delete(__FUNCTION__, wifi_ipv4);
                        wifi_ipv4 = NULL;
                    }
                }

                cJSON_AddNumberToObject(__FUNCTION__, wifi_properties, "priority", 0);
                cJSON_AddStringToObject(__FUNCTION__, wifi_properties, ezlopi_network_str, ezlopi_wan_str);
                cJSON_AddStringToObject(__FUNCTION__, wifi_properties, ezlopi_status_str, ((1 == wifi_status->wifi_connection) ? ezlopi_up_str : ezlopi_down_str));
                cJSON_AddStringToObject(__FUNCTION__, wifi_properties, ezlopi_type_str, ezlopi_wifi_str);

                if (wifi_status->wifi_connection)
                {
                    cJSON *cj_wifi = cJSON_AddObjectToObject(__FUNCTION__, wifi_properties, ezlopi_wifi_str);
                    if (cj_wifi)
                    {
                        char *str_mode = wifi_status->wifi_mode == WIFI_MODE_STA ? "sta" : "ap";
                        cJSON_AddStringToObject(__FUNCTION__, cj_wifi, ezlopi_mode_str, str_mode);

                        cJSON *cj_network = cJSON_AddObjectToObject(__FUNCTION__, cj_wifi, ezlopi_network_str);
                        if (cj_network)
                        {
                            wifi_ap_record_t ap_info;
                            esp_wifi_sta_get_ap_info(&ap_info);

                            memset(tmp_string, 0, sizeof(tmp_string));
                            snprintf(tmp_string, sizeof(tmp_string), "%d", ap_info.primary);
                            cJSON_AddStringToObject(__FUNCTION__, cj_wifi, "channel", tmp_string);

                            memset(tmp_string, 0, sizeof(tmp_string));
                            snprintf(tmp_string, sizeof(tmp_string), "%02x:%02x:%02x:%02x:%02x:%02x", ap_info.bssid[0], ap_info.bssid[1], ap_info.bssid[2], ap_info.bssid[3], ap_info.bssid[4], ap_info.bssid[5]);
                            cJSON_AddStringToObject(__FUNCTION__, cj_network, "bssid", tmp_string);

                            memset(tmp_string, 0, sizeof(tmp_string));
                            get_auth_mode_str(tmp_string, ap_info.authmode);
                            cJSON_AddStringToObject(__FUNCTION__, cj_network, "encryption", tmp_string);

                            char* wifi_ssid = ezlopi_factory_info_v3_get_ssid();
                            char* wifi_password = ezlopi_factory_info_v3_get_password();

                            if ((NULL != wifi_ssid) && ('\0' != wifi_ssid[0]) &&
                                (NULL != wifi_password) && ('\0' != wifi_password[0]))
                            {
                                cJSON_AddStringToObject(__FUNCTION__, cj_network, "key", wifi_password);
                                cJSON_AddStringToObject(__FUNCTION__, cj_network, "ssid", wifi_ssid);
                            }

                            if (wifi_ssid) ezlopi_free(__FUNCTION__, wifi_ssid);
                            if (wifi_password) ezlopi_free(__FUNCTION__, wifi_password);
                        }
                        cJSON_AddStringToObject(__FUNCTION__, cj_wifi, "region", "00");
                    }
                }

                if (!cJSON_AddItemToArray(interfaces_array, wifi_properties))
                {
                    cJSON_Delete(__FUNCTION__, wifi_properties);
                }


                ezlopi_free(__FUNCTION__, wifi_status);
            }
        }
    }
}

void network_wifi_scan_start(cJSON* cj_request, cJSON* cj_response)
{
    ezlopi_wifi_scan_start();
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void network_wifi_scan_stop(cJSON* cj_request, cJSON* cj_response)
{
    ezlopi_wifi_scan_stop();
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void network_wifi_try_connect(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON* cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        char interfaceId[16];
        CJSON_GET_VALUE_STRING_BY_COPY(cj_params, "interfaceId", interfaceId);
        if (0 == strncmp("wlan0", interfaceId, 6))
        {
            cJSON* cj_network = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_network_str);
            if (cj_network)
            {
                ezlopi_wifi_try_connect(cj_network);
            }
        }
    }
}
