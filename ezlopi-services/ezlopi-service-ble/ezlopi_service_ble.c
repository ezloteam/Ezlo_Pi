

/**
 * @file    ezlopi_service_ble.c
 * @brief   Service related functionalities
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

 /*******************************************************************************
  *                          Include Files
  *******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lwip/ip_addr.h"

#include "esp_system.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "cjext.h"
#include "ezlopi_util_trace.h"
#include "EZLOPI_USER_CONFIG.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_wifi.h"
#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_ble_gap.h"
#include "ezlopi_core_ble_gatt.h"
#include "ezlopi_core_ble_profile.h"
#include "ezlopi_core_ble_buffer.h"

#include "ezlopi_service_ble.h"

  /*******************************************************************************
   *                          Extern Function Declarations
   *******************************************************************************/
   /**
    * @brief Function to prepare BLE seurity services
    */
extern void EZPI_ble_service_security_init(void);
/**
 * @brief Function to prepare BLE WiFi services
 */
extern void EZPI_ble_service_wifi_profile_init(void);
/**
 * @brief Function to prepare BLE provisioning services
 */
extern void EZPI_ble_service_provisioning_init(void);
/**
 * @brief Function to prepare BLE device info services
 */
extern void EZPI_ble_service_device_info_init(void);
/**
 * @brief Function to prepare BLE dynamic device config services
 */
extern void EZPI_ble_service_dynamic_config_init(void);

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/
 /**
  * @brief Function to initialize BLE
  * @details This function performs folloiwing operations
  *  - Prepares and set BLE device name
  *  - Configures and enables BLE
  *  - Configures and enables bluedroid
  *  - Registers callbacks for BLE gatts and GAP
  */
static void ezlopi_ble_basic_init(void);
#if (1 == CONFIG_EZPI_BLE_ENALBE_PASSKEY)
/**
 * @brief Function to set BLE in pairing mode
 *
 */
static void ezlopi_ble_start_secure_gatt_server(void);
#endif
#if (1 == CONFIG_EZPI_BLE_ENALBE_PAIRING)
/**
 * @brief Function to set BLE pairing with pass key
 *
 */
static void ezlopi_ble_start_secure_gatt_server_open_pairing(void);
#endif

void EZPI_ble_service_init(void)
{
    EZPI_ble_service_wifi_profile_init();
    EZPI_ble_service_security_init();
    EZPI_ble_service_provisioning_init();
    EZPI_ble_service_device_info_init();
    EZPI_ble_service_dynamic_config_init();

    // EZPI_core_ble_profile_print();
    ezlopi_ble_basic_init();

    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(BLE_WIFI_SERVICE_HANDLE), "gatts 'wifi-app' register error");
    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(BLE_SECURITY_SERVICE_HANDLE), "gatts 'security-app' register error");
    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(BLE_PROVISIONING_ID_HANDLE), "gatts 'provisioning-app' register error");
    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(BLE_DEVICE_INFO_ID_HANDLE), "gatts 'device-info-app' register error");
    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(BLE_DYNAMIC_CONFIG_HANDLE), "gatts 'dynamic-config-app' register error");

    CHECK_PRINT_ERROR(esp_ble_gatt_set_local_mtu(517), "set local  MTU failed");

#if (1 == CONFIG_EZPI_BLE_ENALBE_PAIRING)
#if (1 == CONFIG_EZPI_BLE_ENALBE_PASSKEY)
    ezlopi_ble_start_secure_gatt_server();
#else
    ezlopi_ble_start_secure_gatt_server_open_pairing();
#endif // 1 == CONFIG_EZPI_BLE_ENALBE_PASSKEY
#endif // 1 == CONFIG_EZPI_BLE_ENALBE_PAIRING
}

int EZPI_ble_service_get_ble_mac(uint8_t mac[6])
{
    int ret = 0;
    if (ESP_OK == esp_read_mac(mac, ESP_MAC_BT))
    {
        ret = 1;
    }
    return ret;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/
#if (1 == CONFIG_EZPI_BLE_ENALBE_PAIRING)
static void ezlopi_ble_start_secure_gatt_server_open_pairing(void)
{
    const esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND;
    // ESP_LE_AUTH_REQ_SC_MITM_BOND;
    // ESP_LE_AUTH_REQ_BOND_MITM;
    const esp_ble_io_cap_t iocap = ESP_IO_CAP_NONE; // ESP_IO_CAP_OUT;
    const uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
    const uint8_t oob_support = ESP_BLE_OOB_DISABLE; // ESP_BLE_OOB_ENABLE; // ESP_BLE_OOB_DISABLE;
    const uint8_t init_key = (ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    const uint8_t rsp_key = (ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, (void *)&auth_req, sizeof(uint8_t)),
        "failed -set - ESP_BLE_SM_AUTHEN_REQ_MODE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, (void *)&iocap, sizeof(uint8_t)),
        "failed -set - ESP_BLE_SM_IOCAP_MODE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, (void *)&auth_option, sizeof(uint8_t)),
        "failed -set - ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, (void *)&oob_support, sizeof(uint8_t)),
        "failed -set - ESP_BLE_SM_OOB_SUPPORT");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, (void *)&init_key, sizeof(uint8_t)),
        "failed -set - ESP_BLE_SM_SET_INIT_KEY");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, (void *)&rsp_key, sizeof(uint8_t)),
        "failed -set - ESP_BLE_SM_SET_RSP_KEY");
}
#endif

#if (1 == CONFIG_EZPI_BLE_ENALBE_PASSKEY)
static void ezlopi_ble_start_secure_gatt_server(void)
{
    const uint32_t default_passkey = 123456;
    uint32_t passkey;
    EZPI_core_nvs_read_ble_passkey(&passkey);
    passkey = (0 == passkey) ? default_passkey : passkey;
    passkey = (passkey > 999999) ? default_passkey : passkey;
    TRACE_D("Ble passkey: %d", passkey);

    const esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_SC_MITM_BOND; // ESP_LE_AUTH_REQ_BOND_MITM;
    const esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;
    const uint8_t key_size = 16;
    const uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
    const uint8_t oob_support = ESP_BLE_OOB_ENABLE; // ESP_BLE_OOB_DISABLE;
    const uint8_t init_key = (ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);
    const uint8_t rsp_key = (ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK);

    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t)), "failed -set - ESP_BLE_SM_SET_STATIC_PASSKEY");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_AUTHEN_REQ_MODE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_IOCAP_MODE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_MAX_KEY_SIZE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_OOB_SUPPORT");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_SET_INIT_KEY");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_SET_RSP_KEY");
}
#endif

static void ezlopi_ble_basic_init(void)
{
    // static const char *ble_device_name = "LED";
    char ble_device_name[32];
    memset(ble_device_name, 0, sizeof(ble_device_name));

    // s_ezlopi_factory_info_t *factory = ezlopi_factory_info_get_info();
    // snprintf(ble_device_name, sizeof(ble_device_name), "ezlopi_%llu", EZPI_core_factory_info_v3_get_id());

    const char *device_type = EZPI_core_factory_info_v3_get_device_type();

    if ((1 == EZPI_core_factory_info_v3_get_provisioning_status()) && (NULL != device_type))
    {
        snprintf(ble_device_name, sizeof(ble_device_name), "%s_%llu", device_type, EZPI_core_factory_info_v3_get_id());
    }
    else
    {
        uint8_t mac[6];
        memset(mac, 0, sizeof(mac));
        esp_read_mac(mac, ESP_MAC_BT);
        snprintf(ble_device_name, sizeof(ble_device_name), "ezlopi_%02x%02x%02x%02x%02x%02x",
            mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    }

    // dump("ble_device_name", ble_device_name, 0, sizeof(ble_device_name));

    static esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    CHECK_PRINT_ERROR(esp_bt_controller_init(&bt_cfg), "initialize controller failed");

#if defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)
    CHECK_PRINT_ERROR(esp_bt_controller_enable(ESP_BT_MODE_BLE), "enable controller failed");
#elif defined(CONFIG_IDF_TARGET_ESP32)
    CHECK_PRINT_ERROR(esp_bt_controller_enable(ESP_BT_MODE_BLE), "enable controller failed");
#endif

    CHECK_PRINT_ERROR(esp_bluedroid_init(), "init bluetooth failed");
    CHECK_PRINT_ERROR(esp_bluedroid_enable(), "enable bluetooth failed");
    CHECK_PRINT_ERROR(esp_ble_gatts_register_callback(ezlopi_ble_gatts_event_handler), "gatts register error, error code");
    CHECK_PRINT_ERROR(esp_ble_gap_register_callback(EZPI_core_ble_gap_event_handler), "gap register error");
    CHECK_PRINT_ERROR(esp_ble_gap_set_device_name(ble_device_name), "Set device name failed!");
}

#endif // CONFIG_EZPI_BLE_ENABLE

/*******************************************************************************
 *                          End of File
 *******************************************************************************/