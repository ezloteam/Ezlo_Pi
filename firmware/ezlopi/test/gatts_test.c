#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "esp_system.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "cJSON.h"

#include "lwip/ip_addr.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "sdkconfig.h"
#include "trace.h"
#include "gatts_test.h"

static gatts_profile_inst_t gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_A_APP_ID] = {
        .gatts_cb = gatts_profile_a_event_handler,
        .gatts_if = ESP_GATT_IF_NONE, /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
    [PROFILE_B_APP_ID] = {
        .gatts_cb = gatts_profile_b_event_handler, /* This demo does not implement, similar as profile A */
        .gatts_if = ESP_GATT_IF_NONE,              /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

static void ezlopi_ble_start_secure_gatt_server(void);
static void __gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

void gatts_test_server_init(void)
{
    static esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    GATTS_CHECK_PRINT_ERROR(esp_bt_controller_init(&bt_cfg), "initialize controller failed");
    GATTS_CHECK_PRINT_ERROR(esp_bt_controller_enable(ESP_BT_MODE_BLE), "enable controller failed");
    GATTS_CHECK_PRINT_ERROR(esp_bluedroid_init(), "init bluetooth failed");
    GATTS_CHECK_PRINT_ERROR(esp_bluedroid_enable(), "enable bluetooth failed");
    GATTS_CHECK_PRINT_ERROR(esp_ble_gatts_register_callback(__gatts_event_handler), "gatts register error, error code");
    GATTS_CHECK_PRINT_ERROR(esp_ble_gap_register_callback(__gap_event_handler), "gap register error");
    GATTS_CHECK_PRINT_ERROR(esp_ble_gatts_app_register(PROFILE_A_APP_ID), "gatts app register error");
    GATTS_CHECK_PRINT_ERROR(esp_ble_gatts_app_register(PROFILE_B_APP_ID), "gatts app register error");
    GATTS_CHECK_PRINT_ERROR(esp_ble_gatt_set_local_mtu(517), "set local  MTU failed");
    ezlopi_ble_start_secure_gatt_server();
}

static void __gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    /* If event is register event, store the gatts_if for each profile */
    if (ESP_GATTS_REG_EVT == event)
    {
        if (param->reg.status == ESP_GATT_OK)
        {
            gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
        }
        else
        {
            TRACE_W("Reg app failed, app_id %04x, status %d", param->reg.app_id, param->reg.status);
            return;
        }
    }

    /* If the gatts_if equal to profile A, call profile A cb handler, so here call each profile's callback */

    do
    {
        int idx;
        for (idx = 0; idx < sizeof(gl_profile_tab) / sizeof(gatts_profile_inst_t); idx++)
        {
            if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
                gatts_if == gl_profile_tab[idx].gatts_if)
            {
                if (gl_profile_tab[idx].gatts_cb)
                {
                    gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
                }
            }
        }
    } while (0);
}

static void __gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    TRACE_W("GAP EVENT: %d", event);
    switch (event)
    {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT: // 0
    {
        adv_config_done &= (~ADV_CONFIG_FLAG);
        if (adv_config_done == 0)
        {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    }
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT: // 1
    {
        adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
        if (adv_config_done == 0)
        {
            esp_ble_gap_start_advertising(&adv_params);
        }
        break;
    }
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT: // 6
    {
        // advertising start complete event to indicate advertising start successfully or failed
        if (param->adv_start_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            TRACE_E("Advertising start failed, error status = %x", param->adv_start_cmpl.status);
        }
        else
        {
            TRACE_I("Start adv success.");
        }
        break;
    }
    case ESP_GAP_BLE_PASSKEY_REQ_EVT: // 12
    {
        TRACE_I("ESP_GAP_BLE_PASSKEY_REQ_EVT");
        break;
    }
    case ESP_GAP_BLE_OOB_REQ_EVT: // 13
    {
        TRACE_I("ESP_GAP_BLE_OOB_REQ_EVT");
        uint8_t tk[16] = {1};
        esp_ble_oob_req_reply(param->ble_security.ble_req.bd_addr, tk, sizeof(tk));
        break;
    }
    case ESP_GAP_BLE_LOCAL_IR_EVT: // 14
    {
        TRACE_I("ESP_GAP_BLE_LOCAL_IR_EVT");
        break;
    }
    case ESP_GAP_BLE_LOCAL_ER_EVT: // 15
    {
        TRACE_I("ESP_GAP_BLE_LOCAL_ER_EVT");
        break;
    }
    case ESP_GAP_BLE_NC_REQ_EVT: // 16
    {
        esp_ble_confirm_reply(param->ble_security.ble_req.bd_addr, true);
        TRACE_I("ESP_GAP_BLE_NC_REQ_EVT, the passkey Notify number: %d", param->ble_security.key_notif.passkey);
        break;
    }
    case ESP_GAP_BLE_SEC_REQ_EVT: // 10
    {
        esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, true);
        break;
    }
    case ESP_GAP_BLE_PASSKEY_NOTIF_EVT: // 11
    {
        TRACE_I("The passkey Notify number:%06d", param->ble_security.key_notif.passkey);
        break;
    }
    case ESP_GAP_BLE_KEY_EVT: // 9
    {
        TRACE_I("key type = %s", esp_key_type_to_str(param->ble_security.ble_key.key_type));
        break;
    }
    case ESP_GAP_BLE_AUTH_CMPL_EVT: // 8
    {
        esp_bd_addr_t bd_addr;
        memcpy(bd_addr, param->ble_security.auth_cmpl.bd_addr, sizeof(esp_bd_addr_t));
        dump("remote BD_ADDR", bd_addr, 0, 6);
        TRACE_I("address type = %d", param->ble_security.auth_cmpl.addr_type);
        if (!param->ble_security.auth_cmpl.success)
        {
            TRACE_W("pair status = fail");
            TRACE_W("fail reason = 0x%x", param->ble_security.auth_cmpl.fail_reason);
            esp_ble_gap_disconnect(bd_addr);
        }
        else
        {
            TRACE_I("pair status = success");
            TRACE_I("auth mode =  %s", esp_auth_req_to_str(param->ble_security.auth_cmpl.auth_mode));
        }
        show_bonded_devices();
        break;
    }
    case ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT: // 23
    {
        TRACE_I("ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT status = %d", param->remove_bond_dev_cmpl.status);
        TRACE_I("ESP_GAP_BLE_REMOVE_BOND_DEV");
        TRACE_I("-----ESP_GAP_BLE_REMOVE_BOND_DEV----");
        dump("", param->remove_bond_dev_cmpl.bd_addr, 0, sizeof(esp_bd_addr_t));
        TRACE_I("------------------------------------");
        break;
    }
    case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT: // 22
    {
        if (param->local_privacy_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            TRACE_E("config local privacy failed, error status = %x", param->local_privacy_cmpl.status);
            break;
        }

        esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
        if (ret)
        {
            TRACE_E("config adv data failed, error code = %x", ret);
        }
        else
        {
            adv_config_done |= ADV_CONFIG_FLAG;
        }

        ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
        if (ret)
        {
            TRACE_E("config adv data failed, error code = %x", ret);
        }
        else
        {
            adv_config_done |= SCAN_RSP_CONFIG_FLAG;
        }

        break;
    }
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT: // 17
    {
        if (param->adv_stop_cmpl.status != ESP_BT_STATUS_SUCCESS)
        {
            TRACE_E("Advertising stop failed");
        }
        else
        {
            TRACE_I("Stop adv successfully");
        }
        break;
    }
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT: // 20
    {
        TRACE_I("update connection params status = %d, min_int = %d, max_int = %d,conn_int = %d,latency = %d, timeout = %d",
                param->update_conn_params.status,
                param->update_conn_params.min_int,
                param->update_conn_params.max_int,
                param->update_conn_params.conn_int,
                param->update_conn_params.latency,
                param->update_conn_params.timeout);
        break;
    }
    default:
    {
        break;
    }
    }
}

static void ezlopi_ble_start_secure_gatt_server(void)
{
    /* set the security iocap & auth_req & key size & init key response key parameters to the stack*/
    uint32_t passkey = 123456;
    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_BOND_MITM; // bonding with peer device after authentication
    esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;                 // set the IO capability to No output No input
    uint8_t key_size = 16;                                   // the key size should be 7~16 bytes
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
    uint8_t oob_support = ESP_BLE_OOB_DISABLE;

    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t)),
                      "failed -set - ESP_BLE_SM_SET_STATIC_PASSKEY");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t)),
                      "failed -set - ESP_BLE_SM_AUTHEN_REQ_MODE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t)),
                      "failed -set - ESP_BLE_SM_IOCAP_MODE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t)),
                      "failed -set - ESP_BLE_SM_MAX_KEY_SIZE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t)),
                      "failed -set - ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof(uint8_t)),
                      "failed -set - ESP_BLE_SM_OOB_SUPPORT");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t)),
                      "failed -set - ESP_BLE_SM_SET_INIT_KEY");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t)),
                      "failed -set - ESP_BLE_SM_SET_RSP_KEY");
}