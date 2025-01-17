
/**
 * @file    ezlopi_service_ble_security.c
 * @brief   Security service related functionalities
 * @authors Krishna Kumar Sah (work.krishnasah@gmail.com)
 *          Lomas Subedi
 *          Riken Maharjan
 *          Nabin Dangi
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

#include "cjext.h"
#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_reset.h"
#include "ezlopi_core_ble_gap.h"
#include "ezlopi_core_ble_gatt.h"
#include "ezlopi_core_ble_buffer.h"
#include "ezlopi_core_ble_profile.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_reset.h"

#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_ble.h"

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/**
 * @brief Returns number from the the json `root` which contains name member
 * @note root is the JOSN and should exist before being called
 *
 */
#define CJ_GET_NUMBER(name) cJSON_GetNumberValue(cJSON_GetObjectItem(__FUNCTION__, root, name))

/**
 * @brief Enum for security commands
 *
 */
typedef enum e_ble_security_commands
{
    BLE_CMD_UNDEFINED = 0, /**< Command undefined(0) */
    BLE_CMD_REBOOT,        /**< Command reboot(1) */
    BLE_CMD_FACTORY_RESET, /**< Command facctory reset(2) */
    BLE_CMD_SOFTRESET,     /**< Command soft reset(3) */
    BLE_CMD_HARDREST,      /**< Command hard reset(4) */
    BLE_CMD_AUTHENTICATE,  /**< Command authentication(5) */
    BLE_CMD_MAX,           /**< Max value of enum(6) */
} e_ble_security_commands_t;

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
#if (1 == CONFIG_EZPI_BLE_ENALBE_PASSKEY)
/**
 * @brief Function to set new BLE pass key
 *
 * @param[in] value pointer to the value for pass key characteristics for security service
 * @param[in] param Pointer to the gatts callback parameter
 */
static void ezpi_passkey_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
#endif

/**
 * @brief Function that responds to authenticate command for BLE security service
 *
 * @param[in] root pointer to the incoming authentication JSON
 */
static void ezpi_process_auth_command(cJSON *root);
/**
 * @brief Function that responds to hard reset command for BLE security service
 *
 */
static void ezpi_process_hard_reset_command(void);
/**
 * @brief Function that responds to factory reset command
 *
 * @param[in] value pointer to the command value
 * @param[in] param Pointer to the gatts callback parameter
 */
static void ezpi_factory_reset_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
/**
 * @brief Function that responds to reset command
 *
 * @param[in] value Pointer to the command value
 * @param[in] param Pointer to the gatts callback parameter
 */
static void ezpi_serv_ble_factory_reset_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);

#if (1 == CONFIG_EZPI_BLE_ENALBE_PASSKEY)
s_gatt_char_t *passkey_characterstic = NULL;
static uint32_t start_tick = 0;
static uint32_t authenticated_flag = 0;
#endif
static s_gatt_char_t *factory_reset_characterstic = NULL;
static s_gatt_service_t *security_service = NULL;

void EZPI_ble_service_security_init(void)
{
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t permission;
    esp_gatt_char_prop_t properties;

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = BLE_SECURITY_SERVICE_UUID;
    security_service = EZPI_core_ble_gatt_create_service(BLE_SECURITY_SERVICE_HANDLE, &uuid);

#if (1 == CONFIG_EZPI_BLE_ENALBE_PASSKEY)
    uuid.uuid.uuid16 = BLE_SECURITY_CHAR_PASSKEY_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE;
    passkey_characterstic = EZPI_core_ble_gatt_add_characteristic(security_service, &uuid, permission, properties, NULL, ezpi_passkey_write_func, NULL);
#endif

    uuid.uuid.uuid16 = BLE_SECURITY_FACTORY_RESET_CHAR_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE;
    factory_reset_characterstic = EZPI_core_ble_gatt_add_characteristic(security_service, &uuid, permission, properties, NULL, ezpi_factory_reset_write_func, NULL);

    uuid.uuid.uuid16 = BLE_SECURITY_RESET_CHAR_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE;
    factory_reset_characterstic = EZPI_core_ble_gatt_add_characteristic(security_service, &uuid, permission, properties, NULL, ezpi_serv_ble_factory_reset_write_func, NULL);
}

#if (1 == CONFIG_EZPI_BLE_ENALBE_PASSKEY)
static void ezpi_passkey_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (param->write.len == 4)
    {
        uint32_t passkey = *((uint32_t *)param->write.value);
        if (passkey < 1000000)
        {
            TRACE_D("New passkey: %d", passkey);
            EZPI_core_ble_gap_set_passkey(passkey);
            EZPI_core_nvs_write_ble_passkey(passkey);
            EZPI_core_ble_gap_dissociate_bonded_devices();
        }
    }
}
#endif

static void ezpi_factory_reset_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (param && param->write.len && param->write.value)
    {
        cJSON *root = cJSON_ParseWithLength(__FUNCTION__, (const char *)param->write.value, param->write.len);
        if (root)
        {

            uint32_t cmd = CJ_GET_NUMBER(ezlopi_cmd_str);

            TRACE_D("cmd: %d", cmd);

            switch (cmd)
            {
            case BLE_CMD_REBOOT:
            {
                EZPI_core_reset_reboot();
                break;
            }
            case BLE_CMD_FACTORY_RESET: // factory reset command
            case BLE_CMD_HARDREST:
            {
                ezpi_process_hard_reset_command();
                break;
            }
            case BLE_CMD_AUTHENTICATE: // authentication request for soft-factory-reset
            {
                ezpi_process_auth_command(root);
                break;
            }
            default:
            {
                TRACE_W("Command not valid -> {cmd: %u}.", cmd);
                break;
            }
            }

            cJSON_Delete(__FUNCTION__, root);
        }
    }
}

static void ezpi_serv_ble_factory_reset_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (param && param->write.len && param->write.value)
    {
        cJSON *root = cJSON_ParseWithLength(__FUNCTION__, (const char *)param->write.value, param->write.len);
        if (root)
        {
            cJSON *cj_sub_cmd = cJSON_GetObjectItem(__FUNCTION__, root, ezlopi_sub_cmd_str);
            if (cj_sub_cmd)
            {
                uint8_t sub_cmd = cj_sub_cmd->valuedouble;
                switch (sub_cmd)
                {
                case 0:
                {
                    TRACE_E("Factory restore command");
                    EZPI_core_reset_factory_restore();
                    break;
                }
                case 1:
                {
                    TRACE_E("Reboot only command");
                    EZPI_core_reset_reboot();
                    break;
                }
                default:
                {
                    break;
                }
                }
                cJSON_Delete(__FUNCTION__, cj_sub_cmd);
            }

            cJSON_Delete(__FUNCTION__, root);
        }
    }
}

static void ezpi_process_hard_reset_command(void)
{

#if (1 == CONFIG_EZPI_BLE_ENALBE_PASSKEY)
    uint32_t current_tick = xTaskGetTickCount();
    if ((1 == authenticated_flag) && (current_tick - start_tick) < (30 * 1000 / portTICK_RATE_MS)) // once authenticated, valid for 30 seconds only
    {
#endif
        EZPI_core_reset_factory_restore();
#if (1 == CONFIG_EZPI_BLE_ENALBE_PASSKEY)
    }
    else
    {
        authenticated_flag = 0;
        TRACE_W("Not authenticated for factory-reset!");
    }
#endif
}

static void ezpi_process_auth_command(cJSON *root)
{
#if (1 == CONFIG_EZPI_BLE_ENALBE_PASSKEY)
    uint32_t passkey = CJ_GET_NUMBER("passkey");
    uint32_t original_passkey = 0;
    EZPI_core_nvs_read_ble_passkey(&original_passkey);

    TRACE_D("Old passkey: %u, current_passkey: %u", original_passkey, passkey);

    if (passkey == original_passkey)
    {
        authenticated_flag = 1;
        TRACE_W("Authenticated!");
        start_tick = xTaskGetTickCount();
    }
    else
    {
        authenticated_flag = 0;
        TRACE_W("Not authenticated!");
    }
#endif
}
#endif // CONFIG_EZPI_BLE_ENABLE

/*******************************************************************************
 *                          End of File
 *******************************************************************************/