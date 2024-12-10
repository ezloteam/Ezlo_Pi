
/**
 * @file    ezlopi_service_ble_wifi.c
 * @brief   WiFi service related functionalities
 * @author
 * @version
 * @date
 */
/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
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

#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_BLE_ENABLE

#include <string.h>

#include "lwip/ip_addr.h"

#include "esp_event_base.h"

#include "cjext.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_ping.h"
#include "ezlopi_core_ble_gatt.h"
#include "ezlopi_core_ble_buffer.h"
#include "ezlopi_core_ble_profile.h"
#include "ezlopi_core_factory_info.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_ble_ble_auth.h"
#include "ezlopi_service_ble.h"

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
/**
 * @brief Function callback that responds to ESP_GATTS_WRITE_EVT gatts event for WiFi service
 */
static void ezlpi_wifi_creds_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
/**
 * @brief Function callback that responds to ESP_GATTS_EXEC_WRITE_EVT gatts event for WiFi service
 */
static void ezpi_wifi_creds_write_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
/**
 * @brief Function to parse incoming WiFi credential JSON and connect with provided credentials
 *
 * @param[in] value Pointer to the credential JSON
 * @param[in] len  size of the credential JOSN
 */
static void ezpi_wifi_creds_parse_and_connect(uint8_t *value, uint32_t len);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static s_linked_buffer_t *wifi_creds_linked_buffer = NULL;
static s_gatt_service_t *wifi_ble_service;

void EZPI_ble_service_wifi_profile_init(void)
{
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t permission;
    esp_gatt_char_prop_t properties;

    // wifi credentials
    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = BLE_WIFI_SERVICE_UUID;
    wifi_ble_service = ezlopi_ble_gatt_create_service(BLE_WIFI_SERVICE_HANDLE, &uuid);

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = BLE_WIFI_CHAR_CREDS_UUID;
    permission = ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE;
    ezlopi_ble_gatt_add_characteristic(wifi_ble_service, &uuid, permission, properties, NULL, ezlpi_wifi_creds_write_func, ezpi_wifi_creds_write_exec_func);
}

static void ezlpi_wifi_creds_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (0 == param->write.is_prep) // Data received in single packet
    {
        if ((NULL != param->write.value) && (param->write.len > 0))
        {
            ezpi_wifi_creds_parse_and_connect(param->write.value, param->write.len);
        }
    }
    else
    {
        if (NULL == wifi_creds_linked_buffer)
        {
            wifi_creds_linked_buffer = ezlopi_ble_buffer_create(param);
        }
        else
        {
            ezlopi_ble_buffer_add_to_buffer(wifi_creds_linked_buffer, param);
        }
    }
}

static void ezpi_wifi_creds_write_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (wifi_creds_linked_buffer)
    {
        ezlopi_ble_buffer_accumulate_to_start(wifi_creds_linked_buffer);
        ezpi_wifi_creds_parse_and_connect(wifi_creds_linked_buffer->buffer, wifi_creds_linked_buffer->len);
        ezlopi_ble_buffer_free_buffer(wifi_creds_linked_buffer);
        wifi_creds_linked_buffer = NULL;
    }
}

static void ezpi_wifi_creds_parse_and_connect(uint8_t *value, uint32_t len)
{
    if ((NULL != value) && (len > 0))
    {
        cJSON *root = cJSON_Parse(__FUNCTION__, (const char *)value);

        if (root)
        {
            cJSON *cj_ssid = cJSON_GetObjectItemCaseSensitive(root, ezlopi_wifi_ssid_str);
            cJSON *cj_user_id = cJSON_GetObjectItemCaseSensitive(root, ezlopi_user_id_str);
            cJSON *cj_password = cJSON_GetObjectItemCaseSensitive(root, ezlopi_wifi_password_str);

            if (cj_user_id && cj_user_id->valuestring && cj_password && cj_password->valuestring && cj_ssid && cj_ssid->valuestring)
            {
                char *ssid = cj_ssid->valuestring;
                char *password = cj_password->valuestring;
                char *user_id_str = cj_user_id->valuestring;

                if (user_id_str && ssid && password)
                {
                    e_auth_status_t l_ble_auth_status = ezlopi_ble_auth_check_user_id(user_id_str);

                    if (BLE_AUTH_SUCCESS == l_ble_auth_status)
                    {
                        ezlopi_wifi_connect(ssid, password);
                        ezlopi_factory_info_v3_set_wifi(ssid, password);
                    }
                    else if (BLE_AUTH_USER_ID_NOT_FOUND == l_ble_auth_status)
                    {
                        ezlopi_wifi_connect(ssid, password);
                        ezlopi_factory_info_v3_set_wifi(ssid, password);
                        ezlopi_ble_auth_store_user_id(user_id_str);
                    }
                }
            }

            cJSON_Delete(__FUNCTION__, root);
        }
    }
}
#endif // CONFIG_EZPI_BLE_ENABLE

/*******************************************************************************
 *                          End of File
 *******************************************************************************/