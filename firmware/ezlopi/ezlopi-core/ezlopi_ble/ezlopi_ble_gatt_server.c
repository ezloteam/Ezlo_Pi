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
#include "ezlopi_ble_gatt_server.h"
#include "ezlopi_nvs.h"
#include "ezlopi_wifi.h"
#include "ezlopi_factory_info.h"

static char TEST_DEVICE_NAME[32];

#define GATTS_DEMO_CHAR_VAL_LEN_MAX 0x40
#define PREPARE_BUF_MAX_SIZE 1024
static uint8_t char1_str[] = {0x11, 0x22, 0x33};

static esp_gatt_char_prop_t a_property = 0;

/// Declare the static function

static void show_bonded_devices(void);
static char *esp_auth_req_to_str(esp_ble_auth_req_t auth_req);
static char *esp_key_type_to_str(esp_ble_key_type_t key_type);
static void ezlopi_ble_start_secure_gatt_server(void);
static esp_err_t ezlopi_ble_getts_parse_and_connect_wifi(uint8_t *data, uint32_t len);
static void gatts_exec_wifi_connect_event(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
static void gatts_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
static void __gatts_profile_wifi_info_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
static void __gatts_profile_wifi_status_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
static void __gatts_profile_wifi_error_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

static esp_attr_value_t gatts_demo_char1_val = {
    .attr_max_len = GATTS_DEMO_CHAR_VAL_LEN_MAX,
    .attr_len = sizeof(char1_str),
    .attr_value = char1_str,
};

static uint8_t adv_config_done = 0;
#define ADV_CONFIG_FLAG (1 << 0)
#define SCAN_RSP_CONFIG_FLAG (1 << 1)

#define PROFILE_NUM 3
#define PROFILE_WIFI_INFO_APP_ID 0
#define PROFILE_WIFI_STATUS_APP_ID 1
#define PROFILE_WIFI_ERROR_APP_ID 2

#ifdef CONFIG_SET_RAW_ADV_DATA
static uint8_t raw_adv_data[] = {
    0x02, 0x01, 0x06, 0x02, 0x0a, 0xeb, 0x03, 0x03, 0xab, 0xcd};
static uint8_t raw_scan_rsp_data[] = {
    0x0f, 0x09, 0x45, 0x53, 0x50, 0x5f, 0x47, 0x41, 0x54, 0x54, 0x53, 0x5f, 0x44, 0x45, 0x4d, 0x4f};
#else

/*                                       LSB <-----------------------------------------------------------------------------------> MSB */
static uint8_t adv_service_uuid128[] = {0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xEE, 0x00, 0x00, 0x00,  // first uuid, 16bit, [12],[13] is the value
                                        0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xFF, 0x00, 0x00, 0x00,  // second uuid, 32bit, [12], [13], [14], [15] is the value
                                        0xfb, 0x34, 0x9b, 0x5f, 0x80, 0x00, 0x00, 0x80, 0x00, 0x10, 0x00, 0x00, 0xF1, 0x00, 0x00, 0x00}; // third uuid, 32bit, [12], [13], [14], [15] is the value

// The length of adv data must be less than 31 bytes
static uint8_t test_manufacturer[] = {'e', 'z', 'l', 'o', 'p', 'i'};
// adv data
static esp_ble_adv_data_t adv_data = {
    .set_scan_rsp = false,
    .include_name = true,
    .include_txpower = false,
    .min_interval = 0x0006, // slave connection min interval, Time = min_interval * 1.25 msec
    .max_interval = 0x0010, // slave connection max interval, Time = max_interval * 1.25 msec
    .appearance = 0x00,
    .manufacturer_len = 0,       // TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = NULL, //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};
// scan response data
static esp_ble_adv_data_t scan_rsp_data = {
    .set_scan_rsp = true,
    .include_name = true,
    .include_txpower = true,
    //.min_interval = 0x0006,
    //.max_interval = 0x0010,
    .appearance = 0x00,
    .manufacturer_len = sizeof(test_manufacturer), // TEST_MANUFACTURER_DATA_LEN,
    .p_manufacturer_data = test_manufacturer,      //&test_manufacturer[0],
    .service_data_len = 0,
    .p_service_data = NULL,
    .service_uuid_len = sizeof(adv_service_uuid128),
    .p_service_uuid = adv_service_uuid128,
    .flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT),
};

#endif /* CONFIG_SET_RAW_ADV_DATA */

static esp_ble_adv_params_t adv_params = {
    .adv_int_min = 0x20,
    .adv_int_max = 0x40,
    .adv_type = ADV_TYPE_IND,
    .own_addr_type = BLE_ADDR_TYPE_PUBLIC,
    //.peer_addr            =
    //.peer_addr_type       =
    .channel_map = ADV_CHNL_ALL,
    .adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY,
};

/* One gatt-based profile one app_id and one gatts_if, this array will store the gatts_if returned by ESP_GATTS_REG_EVT */
static gatts_profile_inst_t gl_profile_tab[PROFILE_NUM] = {
    [PROFILE_WIFI_INFO_APP_ID] = {
        .gatts_cb = __gatts_profile_wifi_info_event_handler,
        .gatts_if = ESP_GATT_IF_NONE, /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
    [PROFILE_WIFI_STATUS_APP_ID] = {
        .gatts_cb = __gatts_profile_wifi_status_event_handler, /* This demo does not implement, similar as profile A */
        .gatts_if = ESP_GATT_IF_NONE,                          /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
    [PROFILE_WIFI_ERROR_APP_ID] = {
        .gatts_cb = __gatts_profile_wifi_error_event_handler, /* This demo does not implement, similar as profile A */
        .gatts_if = ESP_GATT_IF_NONE,                         /* Not get the gatt_if, so initial is ESP_GATT_IF_NONE */
    },
};

static prepare_type_env_t a_prepare_write_env;

static void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param)
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

void gatts_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{
    esp_gatt_status_t status = ESP_GATT_OK;
    TRACE_D("param->write.is_prep: %d", param->write.is_prep);
    TRACE_D("param->write.need_rsp: %d", param->write.need_rsp);
    TRACE_D("param->write.len: %d", param->write.len);
    TRACE_D("param->write.offset: %d", param->write.offset);
    TRACE_D("param->write.handle: %d", param->write.handle);
    TRACE_D("param->write.conn_id: %d", param->write.conn_id);
    dump("param->write.bda", param->write.bda, 0, 6);
    dump("param->write.data:", param->write.value, 0, param->write.len);

    if (param->write.need_rsp)
    {
        // TRACE_D("param->write.is_prep: %d", param->write.is_prep);
        if (param->write.is_prep)
        {
            TRACE_D("prepare_write_env->prepare_buf: %s", prepare_write_env->prepare_buf ? "Not NULL" : "NULL");
            if (NULL == prepare_write_env->prepare_buf)
            {
#warning "WARNING: freed while executing (i.e. after prep), still needs to confirm in the condition of failure"
                prepare_write_env->prepare_buf = (uint8_t *)malloc(PREPARE_BUF_MAX_SIZE * sizeof(uint8_t));
                prepare_write_env->prepare_len = 0;

                if (NULL == prepare_write_env->prepare_buf)
                {
                    TRACE_E("Gatt_server prep no mem");
                    status = ESP_GATT_NO_RESOURCES;
                }
            }
            else
            {
                if (param->write.offset > PREPARE_BUF_MAX_SIZE)
                {
                    status = ESP_GATT_INVALID_OFFSET;
                }
                else if ((param->write.offset + param->write.len) > PREPARE_BUF_MAX_SIZE)
                {
                    status = ESP_GATT_INVALID_ATTR_LEN;
                }
            }

            esp_gatt_rsp_t *gatt_rsp = (esp_gatt_rsp_t *)malloc(sizeof(esp_gatt_rsp_t));
            if (NULL != gatt_rsp)
            {
                gatt_rsp->attr_value.len = param->write.len;
                gatt_rsp->attr_value.handle = param->write.handle;
                gatt_rsp->attr_value.offset = param->write.offset;
                gatt_rsp->attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
                memcpy(gatt_rsp->attr_value.value, param->write.value, param->write.len);

                esp_err_t response_err = esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, gatt_rsp);
                if (response_err != ESP_OK)
                {
                    TRACE_E("Send response error");
                }

                if (status == ESP_GATT_OK)
                {
                    memcpy(prepare_write_env->prepare_buf + param->write.offset, param->write.value, param->write.len);
                    prepare_write_env->prepare_len += param->write.len;
                }

                free(gatt_rsp);
            }
        }
        else
        {
            esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, status, NULL);
        }
    }
}

static esp_err_t ezlopi_ble_getts_parse_and_connect_wifi(uint8_t *data, uint32_t len)
{
    esp_err_t err = ESP_OK;

    if ((NULL != data) && (len > 0))
    {
        cJSON *root = cJSON_Parse((const char *)data);
        if (root)
        {
            char *ssid = cJSON_GetObjectItemCaseSensitive(root, "SSID")->valuestring;
            char *password = cJSON_GetObjectItemCaseSensitive(root, "PSD")->valuestring;
            esp_err_t wifi_error = ezlopi_wifi_connect(ssid, password);
            cJSON_Delete(root);
        }
        else
        {
            err = ESP_ERR_NVS_INVALID_STATE;
        }
    }

    return err;
}

void gatts_exec_wifi_connect_event(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param)
{
    if (ESP_GATT_PREP_WRITE_EXEC == param->exec_write.exec_write_flag)
    {
        TRACE_I("Prep data[%d]: %s", prepare_write_env->prepare_len, (char *)prepare_write_env->prepare_buf);
        ezlopi_ble_getts_parse_and_connect_wifi(prepare_write_env->prepare_buf, prepare_write_env->prepare_len);
    }
    else
    {
        TRACE_I("ESP_GATT_PREP_WRITE_CANCEL");
    }

    if (NULL != prepare_write_env->prepare_buf)
    {
        free(prepare_write_env->prepare_buf);
        prepare_write_env->prepare_buf = NULL;
    }

    prepare_write_env->prepare_len = 0;
}

static void __gatts_profile_wifi_info_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    TRACE_W("WIFI_INFO - BLE-GATT event: %d", event);

    switch (event)
    {
    case ESP_GATTS_REG_EVT:
    {
        TRACE_I("REGISTER_APP_EVT, status %d, app_id %d", param->reg.status, param->reg.app_id);
        gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].service_id.is_primary = true;
        gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].service_id.id.inst_id = 0x00;
        gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].service_id.id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID_WIFI_INFO;

        esp_err_t set_dev_name_ret = esp_ble_gap_set_device_name(TEST_DEVICE_NAME);
        if (set_dev_name_ret)
        {
            TRACE_E("set device name failed, error code = %x", set_dev_name_ret);
        }
#ifdef CONFIG_SET_RAW_ADV_DATA
        esp_err_t raw_adv_ret = esp_ble_gap_config_adv_data_raw(raw_adv_data, sizeof(raw_adv_data));
        if (raw_adv_ret)
        {
            TRACE_E("config raw adv data failed, error code = %x ", raw_adv_ret);
        }
        adv_config_done |= ADV_CONFIG_FLAG;
        esp_err_t raw_scan_ret = esp_ble_gap_config_scan_rsp_data_raw(raw_scan_rsp_data, sizeof(raw_scan_rsp_data));
        if (raw_scan_ret)
        {
            TRACE_E("config raw scan rsp data failed, error code = %x", raw_scan_ret);
        }
        adv_config_done |= SCAN_RSP_CONFIG_FLAG;
#else
        // config adv data
        esp_err_t ret = esp_ble_gap_config_adv_data(&adv_data);
        if (ret)
        {
            TRACE_E("config adv data failed, error code = %x", ret);
        }
        adv_config_done |= ADV_CONFIG_FLAG;
        // config scan response data
        ret = esp_ble_gap_config_adv_data(&scan_rsp_data);
        if (ret)
        {
            TRACE_E("config scan response data failed, error code = %x", ret);
        }
        adv_config_done |= SCAN_RSP_CONFIG_FLAG;

#endif
        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].service_id, GATTS_NUM_HANDLE_WIFI_INFO);
        break;
    }
    case ESP_GATTS_READ_EVT:
    {
#warning "Need to fix data transfer for 'character-description'"
        TRACE_I("GATT_READ_EVT, conn_id %d, trans_id %d, handle %d", param->read.conn_id, param->read.trans_id, param->read.handle);
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;

        char wifi_creds[64];
        memset(wifi_creds, 0, sizeof(wifi_creds));
        ezlopi_nvs_read_wifi(wifi_creds, sizeof(wifi_creds));

        cJSON *cjson_wifi_info = cJSON_CreateObject();
        if (cjson_wifi_info)
        {
            if (strlen(wifi_creds) >= 32)
            {
                wifi_creds[31] = '\0';
            }
            cJSON_AddStringToObject(cjson_wifi_info, "SSID", &wifi_creds[0]);
            cJSON_AddStringToObject(cjson_wifi_info, "PSD", "********");

            esp_netif_ip_info_t *wifi_ip_info = ezlopi_wifi_get_ip_infos();
            cJSON_AddStringToObject(cjson_wifi_info, "ip", ip4addr_ntoa((const ip4_addr_t *)&wifi_ip_info->ip));
            cJSON_AddStringToObject(cjson_wifi_info, "gw", ip4addr_ntoa((const ip4_addr_t *)&wifi_ip_info->gw));
            cJSON_AddStringToObject(cjson_wifi_info, "netmask", ip4addr_ntoa((const ip4_addr_t *)&wifi_ip_info->netmask));

            char *json_str_wifi_info = cJSON_Print(cjson_wifi_info);
            if (json_str_wifi_info)
            {
                cJSON_Minify(json_str_wifi_info);
                if (0 != strlen(json_str_wifi_info) && strlen(json_str_wifi_info) > param->read.offset)
                {
                    strncpy((char *)rsp.attr_value.value, json_str_wifi_info + param->read.offset, ESP_GATT_MAX_ATTR_LEN);
                    rsp.attr_value.len = strlen((const char *)rsp.attr_value.value);
                }
                else
                {
                    rsp.attr_value.len = 1;
                    rsp.attr_value.value[0] = 0; // Read 0 if the device not provisioned yet.
                }

                dump("rsp.attr_value.value", rsp.attr_value.value, 0, rsp.attr_value.len);
                esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
                free(json_str_wifi_info);
            }

            cJSON_Delete(cjson_wifi_info);
        }

        break;
    }
    case ESP_GATTS_WRITE_EVT:
    {
        TRACE_I("GATT_WRITE_EVT, conn_id %d, trans_id %d, handle %d", param->write.conn_id, param->write.trans_id, param->write.handle);
        if (0 == param->write.is_prep) // Data received in single packet
        {
            dump("GATT_WRITE_EVT value", param->write.value, 0, param->write.len);

            if (gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].descr_handle == param->write.handle && param->write.len == 2)
            {
                uint16_t descr_value = param->write.value[1] << 8 | param->write.value[0];
                if (descr_value == 0x0001)
                {
                }
                else if (descr_value == 0x0002)
                {
                }
                else if (descr_value == 0x0000)
                {
                    TRACE_I("notify/indicate disable ");
                }
                else
                {
                    TRACE_E("unknown descr value");
                }
            }

            ezlopi_ble_getts_parse_and_connect_wifi(param->write.value, param->write.len);
        }

        gatts_write_event_env(gatts_if, &a_prepare_write_env, param);
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT:
    {
        TRACE_I("ESP_GATTS_EXEC_WRITE_EVT");
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        gatts_exec_wifi_connect_event(&a_prepare_write_env, param);
        break;
    }
    case ESP_GATTS_MTU_EVT:
    {
        TRACE_I("ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        break;
    }
    case ESP_GATTS_CREATE_EVT:
    {
        TRACE_I("CREATE_SERVICE_EVT, status %d,  service_handle %d", param->create.status, param->create.service_handle);
        gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].service_handle = param->create.service_handle;
        gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].char_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].char_uuid.uuid.uuid16 = GATTS_CHAR_UUID_WIFI_INFO;

        esp_ble_gatts_start_service(gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].service_handle);
        esp_gatt_char_prop_t a_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
        esp_err_t add_char_ret = esp_ble_gatts_add_char(gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].service_handle, &gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].char_uuid,
                                                        ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, a_property, &gatts_demo_char1_val, NULL);
        if (add_char_ret)
        {
            TRACE_E("add char failed, error code =%x", add_char_ret);
        }
        break;
    }
    case ESP_GATTS_ADD_CHAR_EVT:
    {
        uint16_t length = 0;
        const uint8_t *prf_char;

        TRACE_I("ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d",
                param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);
        gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].char_handle = param->add_char.attr_handle;
        gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;
        esp_err_t get_attr_ret = esp_ble_gatts_get_attr_value(param->add_char.attr_handle, &length, &prf_char);
        if (get_attr_ret == ESP_FAIL)
        {
            TRACE_E("ILLEGAL HANDLE");
        }

        TRACE_I("the gatts demo char length = %x", length);
        for (int i = 0; i < length; i++)
        {
            TRACE_I("prf_char[%x] =%x", i, prf_char[i]);
        }
        esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].service_handle, &gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].descr_uuid,
                                                               ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, NULL, NULL);
        if (add_descr_ret)
        {
            TRACE_E("add char descr failed, error code =%x", add_descr_ret);
        }
        break;
    }
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
    {
        gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].descr_handle = param->add_char_descr.attr_handle;
        TRACE_I("ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d",
                param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);
        break;
    }
    case ESP_GATTS_START_EVT:
    {
        TRACE_I("SERVICE_START_EVT, status %d, service_handle %d",
                param->start.status, param->start.service_handle);
        break;
    }

    case ESP_GATTS_CONNECT_EVT:
    {
#if 0 // for insecure communication
        esp_ble_conn_update_params_t conn_params = {0};
        memcpy(conn_params.bda, param->connect.remote_bda, sizeof(esp_bd_addr_t));
        /* For the IOS system, please reference the apple official documents about the ble connection parameters restrictions. */
        conn_params.latency = 0;
        conn_params.max_int = 0x20; // max_int = 0x20*1.25ms = 40ms
        conn_params.min_int = 0x10; // min_int = 0x10*1.25ms = 20ms
        conn_params.timeout = 400;  // timeout = 400*10ms = 4000ms
        TRACE_I("ESP_GATTS_CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
                param->connect.conn_id,
                param->connect.remote_bda[0], param->connect.remote_bda[1], param->connect.remote_bda[2],
                param->connect.remote_bda[3], param->connect.remote_bda[4], param->connect.remote_bda[5]);
        gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].conn_id = param->connect.conn_id;
        // start sent the update connection parameters to the peer device.
        esp_ble_gap_update_conn_params(&conn_params);
#endif
        TRACE_I("ESP_GATTS_CONNECT_EVT");
        /* start security connect with peer device when receive the connect event sent by the master */
        esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);
        break;
    }
    case ESP_GATTS_DISCONNECT_EVT:
    {
        TRACE_I("ESP_GATTS_DISCONNECT_EVT, disconnect reason 0x%x", param->disconnect.reason);
        esp_ble_gap_start_advertising(&adv_params);
        break;
    }
    case ESP_GATTS_CONF_EVT:
    {
        TRACE_I("ESP_GATTS_CONF_EVT, status %d attr_handle %d", param->conf.status, param->conf.handle);
        if (param->conf.status != ESP_GATT_OK)
        {
            dump("param->conf.value", param->conf.value, 0, param->conf.len);
        }
        break;
    }
    case ESP_GATTS_STOP_EVT:
    case ESP_GATTS_DELETE_EVT:
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
    case ESP_GATTS_UNREG_EVT:
    case ESP_GATTS_OPEN_EVT:
    case ESP_GATTS_CANCEL_OPEN_EVT:
    case ESP_GATTS_CLOSE_EVT:
    case ESP_GATTS_LISTEN_EVT:
    case ESP_GATTS_CONGEST_EVT:
    case ESP_GATTS_RESPONSE_EVT:
    default:
        break;
    }
}

static void __gatts_profile_wifi_status_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    TRACE_W("WIFI_STATUS - BLE-GATT event: %d", event);

    switch (event)
    {
    case ESP_GATTS_REG_EVT: // 0
    {
        TRACE_I("REGISTER_APP_EVT, status %d, app_id %d", param->reg.status, param->reg.app_id);
        gl_profile_tab[PROFILE_WIFI_STATUS_APP_ID].service_id.is_primary = true;
        gl_profile_tab[PROFILE_WIFI_STATUS_APP_ID].service_id.id.inst_id = 0x00;
        gl_profile_tab[PROFILE_WIFI_STATUS_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_WIFI_STATUS_APP_ID].service_id.id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID_WIFI_STATUS;

        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[PROFILE_WIFI_STATUS_APP_ID].service_id, GATTS_NUM_HANDLE_WIFI_STATUS);
        break;
    }
    case ESP_GATTS_READ_EVT: // 1
    {
        TRACE_I("GATT_READ_EVT, conn_id %d, trans_id %d, handle %d", param->read.conn_id, param->read.trans_id, param->read.handle);
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 1;
        if (ezlopi_wifi_got_ip())
        {
            rsp.attr_value.value[0] = 1;
        }
        else
        {
            rsp.attr_value.value[0] = 0;
        }
        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
        break;
    }
    case ESP_GATTS_MTU_EVT: // 4
    {
        TRACE_I("ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        break;
    }
    case ESP_GATTS_CREATE_EVT: // 7
    {
        TRACE_I("CREATE_SERVICE_EVT, status %d,  service_handle %d", param->create.status, param->create.service_handle);
        gl_profile_tab[PROFILE_WIFI_STATUS_APP_ID].service_handle = param->create.service_handle;
        gl_profile_tab[PROFILE_WIFI_STATUS_APP_ID].char_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_WIFI_STATUS_APP_ID].char_uuid.uuid.uuid16 = GATTS_CHAR_UUID_WIFI_STATUS;

        esp_ble_gatts_start_service(gl_profile_tab[PROFILE_WIFI_STATUS_APP_ID].service_handle);

        {
            esp_bt_uuid_t tmp_uuid = {.uuid.uuid16 = GATTS_CHAR_UUID_WIFI_STATUS, .len = ESP_UUID_LEN_16};
            esp_err_t add_char_ret = esp_ble_gatts_add_char(param->create.service_handle, &tmp_uuid, ESP_GATT_PERM_READ, ESP_GATT_CHAR_PROP_BIT_READ, NULL, NULL);
            if (add_char_ret)
            {
                TRACE_E("WIFI_STATUS char failed, error code =%x", add_char_ret);
            }
        }
        break;
    }
    case ESP_GATTS_ADD_CHAR_EVT: // 9
    {
        TRACE_I("ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d",
                param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);

        gl_profile_tab[PROFILE_WIFI_STATUS_APP_ID].char_handle = param->add_char.attr_handle;
        gl_profile_tab[PROFILE_WIFI_STATUS_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_WIFI_STATUS_APP_ID].descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

        esp_bt_uuid_t tmp_uuid = {.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG, .len = ESP_UUID_LEN_16};
        esp_ble_gatts_add_char_descr(param->add_char.service_handle, &tmp_uuid, (ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE), NULL, NULL);

        break;
    }
    case ESP_GATTS_ADD_CHAR_DESCR_EVT: // 10
    {
        gl_profile_tab[PROFILE_WIFI_STATUS_APP_ID].descr_handle = param->add_char_descr.attr_handle;
        TRACE_I("ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d",
                param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);
        break;
    }
    case ESP_GATTS_START_EVT: // 12
    {
        TRACE_I("SERVICE_START_EVT, status %d, service_handle %d",
                param->start.status, param->start.service_handle);
        break;
    }
    case ESP_GATTS_CONNECT_EVT: // 14
    {
#if 0
        TRACE_I("CONNECT_EVT, conn_id %d, remote %02x:%02x:%02x:%02x:%02x:%02x:",
                param->connect.conn_id,
                param->connect.remote_bda[0], param->connect.remote_bda[1], param->connect.remote_bda[2],
                param->connect.remote_bda[3], param->connect.remote_bda[4], param->connect.remote_bda[5]);
        gl_profile_tab[PROFILE_WIFI_STATUS_APP_ID].conn_id = param->connect.conn_id;
#endif

        TRACE_I("ESP_GATTS_CONNECT_EVT");
        /* start security connect with peer device when receive the connect event sent by the master */
        esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);

        break;
    }
    case ESP_GATTS_CONF_EVT: // 5
    {
        TRACE_I("ESP_GATTS_CONF_EVT status %d attr_handle %d", param->conf.status, param->conf.handle);
        if (param->conf.status != ESP_GATT_OK)
        {
            dump("param->conf.value", param->conf.value, 0, param->conf.len);
        }
        break;
    }
    case ESP_GATTS_ADD_INCL_SRVC_EVT: // 8
    case ESP_GATTS_STOP_EVT:          // 13
    case ESP_GATTS_DELETE_EVT:        // 11
    case ESP_GATTS_DISCONNECT_EVT:    // 15
    case ESP_GATTS_OPEN_EVT:          // 16
    case ESP_GATTS_CANCEL_OPEN_EVT:   // 17
    case ESP_GATTS_CLOSE_EVT:         // 18
    case ESP_GATTS_LISTEN_EVT:        // 19
    case ESP_GATTS_CONGEST_EVT:       // 20
    default:
        break;
    }
}

static void __gatts_profile_wifi_error_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    TRACE_W("WIFI_ERROR - BLE-GATT event: %d", event);

    switch (event)
    {
    case ESP_GATTS_REG_EVT: // 0
    {
        TRACE_I("REGISTER_APP_EVT, status %d, app_id %d", param->reg.status, param->reg.app_id);
        gl_profile_tab[PROFILE_WIFI_ERROR_APP_ID].service_id.is_primary = true;
        gl_profile_tab[PROFILE_WIFI_ERROR_APP_ID].service_id.id.inst_id = 0x00;
        gl_profile_tab[PROFILE_WIFI_ERROR_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_WIFI_ERROR_APP_ID].service_id.id.uuid.uuid.uuid16 = GATTS_SERVICE_UUID_WIFI_ERROR;

        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[PROFILE_WIFI_ERROR_APP_ID].service_id, GATTS_NUM_HANDLE_WIFI_ERROR);
        break;
    }
    case ESP_GATTS_READ_EVT: // 1
    {
        TRACE_I("GATT_READ_EVT, conn_id %d, trans_id %d, handle %d", param->read.conn_id, param->read.trans_id, param->read.handle);
        esp_gatt_rsp_t rsp;
        memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
        rsp.attr_value.handle = param->read.handle;
        rsp.attr_value.len = 1;
        if (ezlopi_wifi_got_ip())
        {
            rsp.attr_value.value[0] = 1;
        }
        else
        {
            rsp.attr_value.value[0] = 0;
        }
        rsp.attr_value.len = strlen(ezlopi_wifi_get_last_disconnect_reason());
        snprintf((char *)rsp.attr_value.value, ESP_GATT_MAX_ATTR_LEN, "%s", ezlopi_wifi_get_last_disconnect_reason());
        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
        break;
    }
    case ESP_GATTS_MTU_EVT: // 4
    {
        TRACE_I("ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        break;
    }
    case ESP_GATTS_CREATE_EVT: // 7
    {
        TRACE_I("CREATE_SERVICE_EVT, status %d,  service_handle %d", param->create.status, param->create.service_handle);
        gl_profile_tab[PROFILE_WIFI_ERROR_APP_ID].service_handle = param->create.service_handle;
        gl_profile_tab[PROFILE_WIFI_ERROR_APP_ID].char_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_WIFI_ERROR_APP_ID].char_uuid.uuid.uuid16 = GATTS_CHAR_UUID_WIFI_ERROR;

        esp_ble_gatts_start_service(gl_profile_tab[PROFILE_WIFI_ERROR_APP_ID].service_handle);

        {
            esp_bt_uuid_t tmp_uuid = {.uuid.uuid16 = GATTS_CHAR_UUID_WIFI_ERROR, .len = ESP_UUID_LEN_16};
            esp_err_t add_char_ret = esp_ble_gatts_add_char(param->create.service_handle, &tmp_uuid, ESP_GATT_PERM_READ, ESP_GATT_CHAR_PROP_BIT_READ, NULL, NULL);
            if (add_char_ret)
            {
                TRACE_E("WIFI_STATUS char failed, error code =%x", add_char_ret);
            }
        }
        break;
    }
    case ESP_GATTS_ADD_CHAR_EVT: // 9
    {
        TRACE_I("ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d",
                param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);

        gl_profile_tab[PROFILE_WIFI_ERROR_APP_ID].char_handle = param->add_char.attr_handle;
        gl_profile_tab[PROFILE_WIFI_ERROR_APP_ID].descr_uuid.len = ESP_UUID_LEN_16;
        gl_profile_tab[PROFILE_WIFI_ERROR_APP_ID].descr_uuid.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

        esp_bt_uuid_t tmp_uuid = {.uuid.uuid16 = ESP_GATT_UUID_CHAR_CLIENT_CONFIG, .len = ESP_UUID_LEN_16};
        esp_ble_gatts_add_char_descr(param->add_char.service_handle, &tmp_uuid, (ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE), NULL, NULL);

        break;
    }
    case ESP_GATTS_ADD_CHAR_DESCR_EVT: // 10
    {
        gl_profile_tab[PROFILE_WIFI_ERROR_APP_ID].descr_handle = param->add_char_descr.attr_handle;
        TRACE_I("ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d",
                param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);
        break;
    }
    case ESP_GATTS_START_EVT: // 12
    {
        TRACE_I("SERVICE_START_EVT, status %d, service_handle %d",
                param->start.status, param->start.service_handle);
        break;
    }
    case ESP_GATTS_CONNECT_EVT: // 14
    {
        TRACE_I("ESP_GATTS_CONNECT_EVT");
        /* start security connect with peer device when receive the connect event sent by the master */
        esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);

        break;
    }
    case ESP_GATTS_CONF_EVT: // 5
    {
        TRACE_I("ESP_GATTS_CONF_EVT status %d attr_handle %d", param->conf.status, param->conf.handle);
        if (param->conf.status != ESP_GATT_OK)
        {
            dump("param->conf.value", param->conf.value, 0, param->conf.len);
        }
        break;
    }
    case ESP_GATTS_ADD_INCL_SRVC_EVT: // 8
    case ESP_GATTS_DELETE_EVT:        // 11
    case ESP_GATTS_STOP_EVT:          // 13
    case ESP_GATTS_DISCONNECT_EVT:    // 15
    case ESP_GATTS_OPEN_EVT:          // 16
    case ESP_GATTS_CANCEL_OPEN_EVT:   // 17
    case ESP_GATTS_CLOSE_EVT:         // 18
    case ESP_GATTS_LISTEN_EVT:        // 19
    case ESP_GATTS_CONGEST_EVT:       // 20
    default:
        break;
    }
}

static void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    /* If event is register event, store the gatts_if for each profile */
    if (event == ESP_GATTS_REG_EVT)
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

    /* If the gatts_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
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

#define CHECK_PRINT_ERROR(x, msg)                                    \
    {                                                                \
        if (x)                                                       \
        {                                                            \
            TRACE_E("%s %s: %s", __func__, msg, esp_err_to_name(x)); \
            return;                                                  \
        }                                                            \
    }

static esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
void GATT_SERVER_MAIN(void)
{
    s_ezlopi_factory_info_t *factory = ezlopi_factory_info_get_info();
    snprintf(TEST_DEVICE_NAME, sizeof(TEST_DEVICE_NAME), "ezlopi_test_%llu", factory->id);

    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    CHECK_PRINT_ERROR(esp_bt_controller_init(&bt_cfg), "initialize controller failed");
    CHECK_PRINT_ERROR(esp_bt_controller_enable(ESP_BT_MODE_BLE), "enable controller failed");
    CHECK_PRINT_ERROR(esp_bluedroid_init(), "init bluetooth failed");
    CHECK_PRINT_ERROR(esp_bluedroid_enable(), "enable bluetooth failed");
    CHECK_PRINT_ERROR(esp_ble_gatts_register_callback(gatts_event_handler), "gatts register error, error code");
    CHECK_PRINT_ERROR(esp_ble_gap_register_callback(gap_event_handler), "gap register error");
    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(PROFILE_WIFI_INFO_APP_ID), "gatts app register error");
    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(PROFILE_WIFI_STATUS_APP_ID), "gatts app register error");
    CHECK_PRINT_ERROR(esp_ble_gatts_app_register(PROFILE_WIFI_ERROR_APP_ID), "gatts app register error");
    CHECK_PRINT_ERROR(esp_ble_gatt_set_local_mtu(517), "set local  MTU failed");
    ezlopi_ble_start_secure_gatt_server();
}

static void ezlopi_ble_start_secure_gatt_server(void)
{
    uint32_t passkey = 123456;
    esp_ble_auth_req_t auth_req = ESP_LE_AUTH_REQ_BOND_MITM;
    esp_ble_io_cap_t iocap = ESP_IO_CAP_OUT;
    uint8_t key_size = 16;
    uint8_t auth_option = ESP_BLE_ONLY_ACCEPT_SPECIFIED_AUTH_DISABLE;
    uint8_t oob_support = ESP_BLE_OOB_DISABLE;
    uint8_t init_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;
    uint8_t rsp_key = ESP_BLE_ENC_KEY_MASK | ESP_BLE_ID_KEY_MASK;

    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_STATIC_PASSKEY, &passkey, sizeof(uint32_t)), "failed -set - ESP_BLE_SM_SET_STATIC_PASSKEY");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &auth_req, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_AUTHEN_REQ_MODE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &iocap, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_IOCAP_MODE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_MAX_KEY_SIZE, &key_size, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_MAX_KEY_SIZE");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH, &auth_option, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_ONLY_ACCEPT_SPECIFIED_SEC_AUTH");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_OOB_SUPPORT, &oob_support, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_OOB_SUPPORT");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_INIT_KEY, &init_key, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_SET_INIT_KEY");
    CHECK_PRINT_ERROR(esp_ble_gap_set_security_param(ESP_BLE_SM_SET_RSP_KEY, &rsp_key, sizeof(uint8_t)), "failed -set - ESP_BLE_SM_SET_RSP_KEY");
}

static void show_bonded_devices(void)
{
    int dev_num = esp_ble_get_bond_device_num();

    esp_ble_bond_dev_t *dev_list = (esp_ble_bond_dev_t *)malloc(sizeof(esp_ble_bond_dev_t) * dev_num);
    esp_ble_get_bond_device_list(&dev_num, dev_list);
    TRACE_I("Bonded devices number : %d\n", dev_num);

    TRACE_I("Bonded devices list : %d\n", dev_num);
    for (int i = 0; i < dev_num; i++)
    {
        dump("dev_list[i].bd_addr", dev_list[i].bd_addr, 0, sizeof(esp_bd_addr_t));
    }

    free(dev_list);
}

static char *esp_auth_req_to_str(esp_ble_auth_req_t auth_req)
{
    char *auth_str = NULL;
    switch (auth_req)
    {
    case ESP_LE_AUTH_NO_BOND:
        auth_str = "ESP_LE_AUTH_NO_BOND";
        break;
    case ESP_LE_AUTH_BOND:
        auth_str = "ESP_LE_AUTH_BOND";
        break;
    case ESP_LE_AUTH_REQ_MITM:
        auth_str = "ESP_LE_AUTH_REQ_MITM";
        break;
    case ESP_LE_AUTH_REQ_BOND_MITM:
        auth_str = "ESP_LE_AUTH_REQ_BOND_MITM";
        break;
    case ESP_LE_AUTH_REQ_SC_ONLY:
        auth_str = "ESP_LE_AUTH_REQ_SC_ONLY";
        break;
    case ESP_LE_AUTH_REQ_SC_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_BOND";
        break;
    case ESP_LE_AUTH_REQ_SC_MITM:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM";
        break;
    case ESP_LE_AUTH_REQ_SC_MITM_BOND:
        auth_str = "ESP_LE_AUTH_REQ_SC_MITM_BOND";
        break;
    default:
        auth_str = "INVALID BLE AUTH REQ";
        break;
    }

    return auth_str;
}

static char *esp_key_type_to_str(esp_ble_key_type_t key_type)
{
    char *key_str = NULL;
    switch (key_type)
    {
    case ESP_LE_KEY_NONE:
        key_str = "ESP_LE_KEY_NONE";
        break;
    case ESP_LE_KEY_PENC:
        key_str = "ESP_LE_KEY_PENC";
        break;
    case ESP_LE_KEY_PID:
        key_str = "ESP_LE_KEY_PID";
        break;
    case ESP_LE_KEY_PCSRK:
        key_str = "ESP_LE_KEY_PCSRK";
        break;
    case ESP_LE_KEY_PLK:
        key_str = "ESP_LE_KEY_PLK";
        break;
    case ESP_LE_KEY_LLK:
        key_str = "ESP_LE_KEY_LLK";
        break;
    case ESP_LE_KEY_LENC:
        key_str = "ESP_LE_KEY_LENC";
        break;
    case ESP_LE_KEY_LID:
        key_str = "ESP_LE_KEY_LID";
        break;
    case ESP_LE_KEY_LCSRK:
        key_str = "ESP_LE_KEY_LCSRK";
        break;
    default:
        key_str = "INVALID BLE KEY TYPE";
        break;
    }

    return key_str;
}