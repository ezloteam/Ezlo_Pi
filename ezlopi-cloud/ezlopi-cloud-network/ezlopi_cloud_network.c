#include <string.h>
#include "esp_wifi.h"
#include "esp_netif_types.h"
#include "ezlopi_cloud_network.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_ethernet.h"
#include "ezlopi_util_trace.h"

#include "cJSON.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"

#define MAC_ADDR_EXPANDED(mac) mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]

static int get_ethernet_properties(cJSON* interface_array)
{
    int ret = 0;
    cJSON* ethernet_properties = cJSON_CreateObject();
    if (ethernet_properties)
    {
        char tmp_string[64];
        cJSON_AddStringToObject(ethernet_properties, ezlopi__id_str, ezlopi_ethernet_str);
        cJSON_AddStringToObject(ethernet_properties, ezlopi_enabled_str, ezlopi_auto_str);

        uint8_t mac_addr[6];
        esp_read_mac(mac_addr, ESP_MAC_ETH);
        snprintf(tmp_string, sizeof(tmp_string), "%02x:%02x:%02x:%02x:%02x:%02x", MAC_ADDR_EXPANDED(mac_addr));
        cJSON_AddStringToObject(ethernet_properties, ezlopi_hwaddr_str, tmp_string);
        cJSON_AddBoolToObject(ethernet_properties, ezlopi_internetAvailable_str, false);
        cJSON* ethernet_ipv4 = cJSON_CreateObject();
        if (ethernet_ipv4)
        {
            esp_netif_ip_info_t* ip_info = ezlopi_ethernet_get_ip_info();
            snprintf(tmp_string, sizeof(tmp_string), IPSTR, IP2STR(&ip_info->gw));
            cJSON_AddStringToObject(ethernet_ipv4, ezlopi_gateway_str, tmp_string);
            snprintf(tmp_string, sizeof(tmp_string), IPSTR, IP2STR(&ip_info->ip));
            cJSON_AddStringToObject(ethernet_ipv4, ezlopi_ip_str, tmp_string);
            snprintf(tmp_string, sizeof(tmp_string), IPSTR, IP2STR(&ip_info->netmask));
            cJSON_AddStringToObject(ethernet_ipv4, ezlopi_mask_str, tmp_string);
            cJSON_AddStringToObject(ethernet_ipv4, ezlopi_mode_str, ezlopi_dhcp_str);

            if (!cJSON_AddItemToObjectCS(ethernet_properties, ezlopi_ipv4_str, ethernet_ipv4))
            {
                cJSON_Delete(ethernet_ipv4);
                ethernet_ipv4 = NULL;
            }
        }
        cJSON_AddStringToObject(ethernet_properties, ezlopi_network_str, ezlopi_wan_str);
        cJSON_AddStringToObject(ethernet_properties, ezlopi_status_str, ezlopi_down_str);
        cJSON_AddStringToObject(ethernet_properties, ezlopi_type_str, ezlopi_ethernet_str);
        if (!cJSON_AddItemToArray(interface_array, ethernet_properties))
        {
            cJSON_Delete(ethernet_properties);
        }
    }
    else
    {
        ret = 1;
    }
    return ret;
}

static int get_wifi_properties(cJSON* interface_array)
{
    int ret = 0;

    cJSON* wifi_properties = cJSON_CreateObject();
    if (wifi_properties)
    {
        char tmp_string[64];
        cJSON_AddStringToObject(wifi_properties, ezlopi__id_str, ezlopi_wifi_str);
        cJSON_AddStringToObject(wifi_properties, ezlopi_enabled_str, ezlopi_auto_str);

        uint8_t mac_addr[6];
        esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
        snprintf(tmp_string, sizeof(tmp_string), "%02x:%02x:%02x:%02x:%02x:%02x", MAC_ADDR_EXPANDED(mac_addr));
        cJSON_AddStringToObject(wifi_properties, ezlopi_hwaddr_str, tmp_string);
        cJSON_AddBoolToObject(wifi_properties, ezlopi_internetAvailable_str, true);

        cJSON* wifi_ipv4 = cJSON_CreateObject();
        if (wifi_ipv4)
        {
            esp_netif_ip_info_t* ip_info = ezlopi_wifi_get_ip_infos();
            snprintf(tmp_string, sizeof(tmp_string), IPSTR, IP2STR(&ip_info->gw));
            cJSON_AddStringToObject(wifi_ipv4, ezlopi_gateway_str, tmp_string);
            snprintf(tmp_string, sizeof(tmp_string), IPSTR, IP2STR(&ip_info->ip));
            cJSON_AddStringToObject(wifi_ipv4, ezlopi_ip_str, tmp_string);
            snprintf(tmp_string, sizeof(tmp_string), IPSTR, IP2STR(&ip_info->netmask));
            cJSON_AddStringToObject(wifi_ipv4, ezlopi_mask_str, tmp_string);
            cJSON_AddStringToObject(wifi_ipv4, ezlopi_mode_str, ezlopi_dhcp_str);

            if (!cJSON_AddItemToObjectCS(wifi_properties, ezlopi_ipv4_str, wifi_ipv4))
            {
                cJSON_Delete(wifi_ipv4);
                wifi_ipv4 = NULL;
            }
        }
        cJSON_AddStringToObject(wifi_properties, ezlopi_network_str, ezlopi_wan_str);
        cJSON_AddStringToObject(wifi_properties, ezlopi_status_str, ezlopi_up_str);
        cJSON_AddStringToObject(wifi_properties, ezlopi_type_str, ezlopi_wifi_str);

        if (!cJSON_AddItemToArray(interface_array, wifi_properties))
        {
            cJSON_Delete(wifi_properties);
        }
    }
    else
    {
        ret = 1;
    }
    return ret;
}

void network_get(cJSON* cj_request, cJSON* cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_method_str, cJSON_GetObjectItem(cj_request, ezlopi_method_str));
    cJSON* cjson_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON* interfaces_array = cJSON_AddArrayToObject(cjson_result, ezlopi_interfaces_str);
        if (interfaces_array)
        {
            get_ethernet_properties(interfaces_array);
            get_wifi_properties(interfaces_array);
        }
    }
}

