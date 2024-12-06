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

/**
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
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
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/
typedef enum e_ble_security_commands
{
    BLE_CMD_UNDEFINED = 0, // 0
    BLE_CMD_REBOOT,        // 1
    BLE_CMD_FACTORY_RESET, // 2
    BLE_CMD_SOFTRESET,     // 3
    BLE_CMD_HARDREST,      // 4
    BLE_CMD_AUTHENTICATE,  // 5
    BLE_CMD_MAX,
} e_ble_security_commands_t;

#define CJ_GET_NUMBER(name) cJSON_GetNumberValue(cJSON_GetObjectItem(__FUNCTION__, root, name))

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
static s_gatt_char_t* factory_reset_characterstic = NULL;
static void __process_auth_command(cJSON* root);
static void __process_hard_reset_command(void);
// static void __process_factory_reset_command(void);

static void factory_reset_write_func(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);
static void ezlopi_serv_ble_factory_reset_write_func(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);

#if (1 == CONFIG_EZPI_BLE_ENABLE_PASSKEY)
static void passkey_write_func(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param);
#endif

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
#if (1 == CONFIG_EZPI_BLE_ENABLE_PASSKEY)
static uint32_t start_tick = 0;
static uint32_t authenticated_flag = 0;
#endif
static s_gatt_service_t* security_service = NULL;

#if (1 == CONFIG_EZPI_BLE_ENABLE_PASSKEY)
s_gatt_char_t* passkey_characterstic = NULL;
#endif

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/**
 * @brief Global/extern function template example
 * Convention : Use capital letter for initial word on extern function
 * @param arg
 */
void ezlopi_ble_service_security_init(void)
{
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t permission;
    esp_gatt_char_prop_t properties;

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = BLE_SECURITY_SERVICE_UUID;
    security_service = ezlopi_ble_gatt_create_service(BLE_SECURITY_SERVICE_HANDLE, &uuid);

#if (1 == CONFIG_EZPI_BLE_ENABLE_PASSKEY)
    uuid.uuid.uuid16 = BLE_SECURITY_CHAR_PASSKEY_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE;
    passkey_characterstic = ezlopi_ble_gatt_add_characteristic(security_service, &uuid, permission, properties, NULL, passkey_write_func, NULL);
#endif

    uuid.uuid.uuid16 = BLE_SECURITY_FACTORY_RESET_CHAR_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE;
    factory_reset_characterstic = ezlopi_ble_gatt_add_characteristic(security_service, &uuid, permission, properties, NULL, factory_reset_write_func, NULL);

    uuid.uuid.uuid16 = BLE_SECURITY_RESET_CHAR_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE;
    factory_reset_characterstic = ezlopi_ble_gatt_add_characteristic(security_service, &uuid, permission, properties, NULL, ezlopi_serv_ble_factory_reset_write_func, NULL);
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/
#if (1 == CONFIG_EZPI_BLE_ENABLE_PASSKEY)
static void passkey_write_func(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    if (param->write.len == 4)
    {
        uint32_t passkey = *((uint32_t*)param->write.value);
        if (passkey < 1000000)
        {
            TRACE_D("New passkey: %d", passkey);
            ezlopi_ble_gap_set_passkey(passkey);
            ezlopi_nvs_write_ble_passkey(passkey);
            ezlopi_ble_gap_dissociate_bonded_devices();
        }
    }
}
#endif

static void factory_reset_write_func(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    if (param && param->write.len && param->write.value)
    {
        cJSON* root = cJSON_ParseWithLength(__FUNCTION__, (const char*)param->write.value, param->write.len);
        if (root)
        {

            uint32_t cmd = CJ_GET_NUMBER(ezlopi_cmd_str);

            TRACE_D("cmd: %d", cmd);

            switch (cmd)
            {
            case BLE_CMD_REBOOT:
            {
                EZPI_CORE_reset_reboot();
                break;
            }
            case BLE_CMD_FACTORY_RESET: // factory reset command
            case BLE_CMD_HARDREST:
            {
                __process_hard_reset_command();
                break;
            }
            case BLE_CMD_AUTHENTICATE: // authentication request for soft-factory-reset
            {
                __process_auth_command(root);
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

static void ezlopi_serv_ble_factory_reset_write_func(esp_gatt_value_t* value, esp_ble_gatts_cb_param_t* param)
{
    if (param && param->write.len && param->write.value)
    {
        cJSON* root = cJSON_ParseWithLength(__FUNCTION__, (const char*)param->write.value, param->write.len);
        if (root)
        {
            cJSON* cj_sub_cmd = cJSON_GetObjectItem(__FUNCTION__, root, ezlopi_sub_cmd_str);
            if (cj_sub_cmd)
            {
                uint8_t sub_cmd = cj_sub_cmd->valuedouble;
                switch (sub_cmd)
                {
                case 0:
                {
                    TRACE_E("Factory restore command");
                    EZPI_CORE_reset_factory_restore();
                    break;
                }
                case 1:
                {
                    TRACE_E("Reboot only command");
                    EZPI_CORE_reset_reboot();
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


static void __process_hard_reset_command(void)
{

#if (1 == CONFIG_EZPI_BLE_ENABLE_PASSKEY)
    uint32_t current_tick = xTaskGetTickCount();
    if ((1 == authenticated_flag) && (current_tick - start_tick) < (30 * 1000 / portTICK_RATE_MS)) // once authenticated, valid for 30 seconds only
    {
#endif
        EZPI_CORE_reset_factory_restore();
#if (1 == CONFIG_EZPI_BLE_ENABLE_PASSKEY)
    }
    else
    {
        authenticated_flag = 0;
        TRACE_W("Not authenticated for factory-reset!");
    }
#endif
}

static void __process_auth_command(cJSON* root)
{
#if (1 == CONFIG_EZPI_BLE_ENABLE_PASSKEY)
    uint32_t passkey = CJ_GET_NUMBER("passkey");
    uint32_t original_passkey = 0;
    ezlopi_nvs_read_ble_passkey(&original_passkey);

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
