#include "esp_bt.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_bt_main.h"
#include "esp_gatt_common_api.h"

#include "trace.h"
#include "ezlopi_ble_gap.h"

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

static char *ezlopi_ble_gap_event_to_str(esp_gap_ble_cb_event_t event);

void ezlopi_ble_gap_start_advertising(void)
{
    esp_ble_gap_start_advertising(&adv_params);
}

void ezlopi_ble_gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
{
    TRACE_W("BLE GAP Eevent: [%d]-%s", event, ezlopi_ble_gap_event_to_str(event));
    switch (event)
    {
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT: // 0
    {
        TRACE_W("param->adv_data_cmpl.status: %d", param->adv_data_cmpl.status);
        esp_ble_gap_start_advertising(&adv_params);
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

    // case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT: // 1
    // {
    //     adv_config_done &= (~SCAN_RSP_CONFIG_FLAG);
    //     if (adv_config_done == 0)
    //     {
    //         esp_ble_gap_start_advertising(&adv_params);
    //     }
    //     break;
    // }

#if 0

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
#endif
    default:
    {
        break;
    }
    }
}

static char *ezlopi_ble_gap_event_to_str(esp_gap_ble_cb_event_t event)
{
    char *ret = "BLE GAP Event Not defined!";
    switch (event)
    {
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT: /*!< When advertising data set complete, the event comes */
    {
        ret = "ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT: /*!< When scan response data set complete, the event comes */
    {
        ret = "ESP_GAP_BLE_SCAN_RSP_DATA_SET_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT: /*!< When scan parameters set complete, the event comes */
    {
        ret = "ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_SCAN_RESULT_EVT: /*!< When one scan result ready, the event comes each time */
    {
        ret = "ESP_GAP_BLE_SCAN_RESULT_EVT";
        break;
    }
    case ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT: /*!< When raw advertising data set complete, the event comes */
    {
        ret = "ESP_GAP_BLE_ADV_DATA_RAW_SET_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT: /*!< When raw advertising data set complete, the event comes */
    {
        ret = "ESP_GAP_BLE_SCAN_RSP_DATA_RAW_SET_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_ADV_START_COMPLETE_EVT: /*!< When start advertising complete, the event comes */
    {
        ret = "ESP_GAP_BLE_ADV_START_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_SCAN_START_COMPLETE_EVT: /*!< When start scan complete, the event comes */
    {
        ret = "ESP_GAP_BLE_SCAN_START_COMPLETE_EVT";
        break;
    }
#endif                              // #if (BLE_42_FEATURE_SUPPORT == TRUE)
    case ESP_GAP_BLE_AUTH_CMPL_EVT: /* Authentication complete indication. */
    {
        ret = "ESP_GAP_BLE_AUTH_CMPL_EVT";
        break;
    }
    case ESP_GAP_BLE_KEY_EVT: /* BLE  key event for peer device keys */
    {
        ret = "ESP_GAP_BLE_KEY_EVT";
        break;
    }
    case ESP_GAP_BLE_SEC_REQ_EVT: /* BLE  security request */
    {
        ret = "ESP_GAP_BLE_SEC_REQ_EVT";
        break;
    }
    case ESP_GAP_BLE_PASSKEY_NOTIF_EVT: /* passkey notification event */
    {
        ret = "ESP_GAP_BLE_PASSKEY_NOTIF_EVT";
        break;
    }
    case ESP_GAP_BLE_PASSKEY_REQ_EVT: /* passkey request event */
    {
        ret = "ESP_GAP_BLE_PASSKEY_REQ_EVT";
        break;
    }
    case ESP_GAP_BLE_OOB_REQ_EVT: /* OOB request event */
    {
        ret = "ESP_GAP_BLE_OOB_REQ_EVT";
        break;
    }
    case ESP_GAP_BLE_LOCAL_IR_EVT: /* BLE local IR event */
    {
        ret = "ESP_GAP_BLE_LOCAL_IR_EVT";
        break;
    }
    case ESP_GAP_BLE_LOCAL_ER_EVT: /* BLE local ER event */
    {
        ret = "ESP_GAP_BLE_LOCAL_ER_EVT";
        break;
    }
    case ESP_GAP_BLE_NC_REQ_EVT: /* Numeric Comparison request event */
    {
        ret = "ESP_GAP_BLE_NC_REQ_EVT";
        break;
    }
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT: /*!< When stop adv complete, the event comes */
    {
        ret = "ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT: /*!< When stop scan complete, the event comes */
    {
        ret = "ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT";
        break;
    }
#endif                                         // #if (BLE_42_FEATURE_SUPPORT == TRUE)
    case ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT: /*!< When set the static rand address complete, the event comes */
    {
        ret = "ESP_GAP_BLE_SET_STATIC_RAND_ADDR_EVT";
        break;
    }
    case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT: /*!< When update connection parameters complete, the event comes */
    {
        ret = "ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT";
        break;
    }
    case ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT: /*!< When set pkt length complete, the event comes */
    {
        ret = "ESP_GAP_BLE_SET_PKT_LENGTH_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT: /*!< When  Enable/disable privacy on the local device complete, the event comes */
    {
        ret = "ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT: /*!< When remove the bond device complete, the event comes */
    {
        ret = "ESP_GAP_BLE_REMOVE_BOND_DEV_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT: /*!< When clear the bond device clear complete, the event comes */
    {
        ret = "ESP_GAP_BLE_CLEAR_BOND_DEV_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT: /*!< When get the bond device list complete, the event comes */
    {
        ret = "ESP_GAP_BLE_GET_BOND_DEV_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT: /*!< When read the rssi complete, the event comes */
    {
        ret = "ESP_GAP_BLE_READ_RSSI_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT: /*!< When add or remove whitelist complete, the event comes */
    {
        ret = "ESP_GAP_BLE_UPDATE_WHITELIST_COMPLETE_EVT";
        break;
    }
#if (BLE_42_FEATURE_SUPPORT == TRUE)
    case ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT: /*!< When update duplicate exceptional list complete, the event comes */
    {
        ret = "ESP_GAP_BLE_UPDATE_DUPLICATE_EXCEPTIONAL_LIST_COMPLETE_EVT";
        break;
    }
#endif                                 //#if (BLE_42_FEATURE_SUPPORT == TRUE)
    case ESP_GAP_BLE_SET_CHANNELS_EVT: /*!< When setting BLE channels complete, the event comes */
    {
        ret = "ESP_GAP_BLE_SET_CHANNELS_EVT";
        break;
    }
#if (BLE_50_FEATURE_SUPPORT == TRUE)
    case ESP_GAP_BLE_READ_PHY_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_READ_PHY_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_SET_PREFERED_DEFAULT_PHY_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_SET_PREFERED_DEFAULT_PHY_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_SET_PREFERED_PHY_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_SET_PREFERED_PHY_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_EXT_ADV_STOP_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_EXT_ADV_SET_REMOVE_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_EXT_ADV_SET_REMOVE_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_EXT_ADV_SET_CLEAR_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_EXT_ADV_SET_CLEAR_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_PERIODIC_ADV_STOP_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_PERIODIC_ADV_SYNC_CANCEL_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_PERIODIC_ADV_SYNC_TERMINATE_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_PERIODIC_ADV_ADD_DEV_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_PERIODIC_ADV_REMOVE_DEV_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_PERIODIC_ADV_CLEAR_DEV_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_EXT_SCAN_STOP_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_PREFER_EXT_CONN_PARAMS_SET_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_PREFER_EXT_CONN_PARAMS_SET_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_PHY_UPDATE_COMPLETE_EVT:
    {
        ret = "ESP_GAP_BLE_PHY_UPDATE_COMPLETE_EVT";
        break;
    }
    case ESP_GAP_BLE_EXT_ADV_REPORT_EVT:
    {
        ret = "ESP_GAP_BLE_EXT_ADV_REPORT_EVT";
        break;
    }
    case ESP_GAP_BLE_SCAN_TIMEOUT_EVT:
    {
        ret = "ESP_GAP_BLE_SCAN_TIMEOUT_EVT";
        break;
    }
    case ESP_GAP_BLE_ADV_TERMINATED_EVT:
    {
        ret = "ESP_GAP_BLE_ADV_TERMINATED_EVT";
        break;
    }
    case ESP_GAP_BLE_SCAN_REQ_RECEIVED_EVT:
    {
        ret = "ESP_GAP_BLE_SCAN_REQ_RECEIVED_EVT";
        break;
    }
    case ESP_GAP_BLE_CHANNEL_SELETE_ALGORITHM_EVT:
    {
        ret = "ESP_GAP_BLE_CHANNEL_SELETE_ALGORITHM_EVT";
        break;
    }
    case ESP_GAP_BLE_PERIODIC_ADV_REPORT_EVT:
    {
        ret = "ESP_GAP_BLE_PERIODIC_ADV_REPORT_EVT";
        break;
    }
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT:
    {
        ret = "ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT";
        break;
    }
    case ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT:
    {
        ret = "ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT";
        break;
    }

#endif // #if (BLE_50_FEATURE_SUPPORT == TRUE)
    case ESP_GAP_BLE_EVT_MAX:
    {
        ret = "ESP_GAP_BLE_EVT_MAX";
        break;
    }
    }

    return ret;
}