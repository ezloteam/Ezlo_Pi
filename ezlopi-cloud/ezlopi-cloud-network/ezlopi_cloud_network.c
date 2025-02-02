/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

/**
 * @file    ezlopi_cloud_network.c
 * @brief    Definitions for cloud netowrk functions
 * @author  Krishna Kumar Sah (work.krishnasah@gmail.com)
 * @version 1.0
 * @date    August 15th, 2022 11:57 AM
 */

#include <string.h>

#include "esp_wifi.h"
#include "esp_netif_types.h"

#include "cjext.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_wifi.h"
#include "ezlopi_core_ethernet.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_cloud_network.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_cloud_keywords.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
/**
 * @brief Macro to expand mac address
 *
 */
#define MAC_ADDR_EXPANDED(mac) mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
void EZPI_network_get(cJSON *cj_request, cJSON *cj_response)
{
    cJSON *cjson_result = cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
    if (cjson_result)
    {
        cJSON *interfaces_array = cJSON_AddArrayToObject(__FUNCTION__, cjson_result, ezlopi_interfaces_str);
        if (interfaces_array)
        {
            cJSON *wifi_properties = cJSON_CreateObject(__FUNCTION__);
            if (wifi_properties)
            {
                char tmp_string[54];
                cJSON_AddStringToObject(__FUNCTION__, wifi_properties, ezlopi__id_str, ezlopi_wlan0_str);
                cJSON_AddStringToObject(__FUNCTION__, wifi_properties, ezlopi_enabled_str, ezlopi_auto_str);

                uint8_t mac_addr[6];
                esp_read_mac(mac_addr, ESP_MAC_WIFI_STA);
                snprintf(tmp_string, sizeof(tmp_string), "%02x:%02x:%02x:%02x:%02x:%02x", MAC_ADDR_EXPANDED(mac_addr));
                cJSON_AddStringToObject(__FUNCTION__, wifi_properties, ezlopi_hwaddr_str, tmp_string);
                cJSON_AddBoolToObject(__FUNCTION__, wifi_properties, ezlopi_internetAvailable_str, true);

                cJSON *wifi_ipv4 = cJSON_CreateObject(__FUNCTION__);
                ezlopi_wifi_status_t *wifi_status = EZPI_core_wifi_status();
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
                            EZPI_core_wifi_get_auth_mode_str(tmp_string, ap_info.authmode);
                            cJSON_AddStringToObject(__FUNCTION__, cj_network, "encryption", tmp_string);

                            char *wifi_ssid = EZPI_core_factory_info_v3_get_ssid();
                            char *wifi_password = EZPI_core_factory_info_v3_get_password();

                            if ((NULL != wifi_ssid) && ('\0' != wifi_ssid[0]) &&
                                (NULL != wifi_password) && ('\0' != wifi_password[0]))
                            {
                                cJSON_AddStringToObject(__FUNCTION__, cj_network, ezlopi_key_str, wifi_password);
                                cJSON_AddStringToObject(__FUNCTION__, cj_network, ezlopi_ssid_str, wifi_ssid);
                            }

                            if (wifi_ssid)
                                ezlopi_free(__FUNCTION__, wifi_ssid);
                            if (wifi_password)
                                ezlopi_free(__FUNCTION__, wifi_password);
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

void EZPI_network_wifi_scan_start(cJSON *cj_request, cJSON *cj_response)
{
    EZPI_core_wifi_scan_start();
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void EZPI_network_wifi_scan_stop(cJSON *cj_request, cJSON *cj_response)
{
    EZPI_core_wifi_scan_stop();
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);
}

void EZPI_network_wifi_try_connect(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddObjectToObject(__FUNCTION__, cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(__FUNCTION__, cj_request, ezlopi_params_str);
    if (cj_params)
    {
        char interfaceId[16];
        CJSON_GET_VALUE_STRING_BY_COPY(cj_params, ezlopi_interfaceId_str, interfaceId, sizeof(interfaceId));
        if (0 == strncmp(ezlopi_wlan0_str, interfaceId, 6))
        {
            cJSON *cj_network = cJSON_GetObjectItem(__FUNCTION__, cj_params, ezlopi_network_str);
            if (cj_network)
            {
                EZPI_core_wifi_try_new_connect(cj_network);
            }
        }
    }
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
