#include <string.h>

#include "trace.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_config.h"
#include "ezlopi_ble_profile.h"

#define GATTS_DEMO_CHAR_VAL_LEN_MAX 0x40
static uint8_t char1_str[] = {0x11, 0x22, 0x33};

static esp_attr_value_t gatts_demo_char1_val = {
    .attr_max_len = GATTS_DEMO_CHAR_VAL_LEN_MAX,
    .attr_len = sizeof(char1_str),
    .attr_value = char1_str,
};

static char *ezlopi_ble_gatt_event_to_string(esp_gatts_cb_event_t event);

void ezlopi_ble_gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    TRACE_I("BLE GATT Event: %s, gatts_if: %d", ezlopi_ble_gatt_event_to_string(event), gatts_if);

    switch (event)
    {
    case ESP_GATTS_REG_EVT:
    {
        s_gatt_service_t *service = ezlopi_ble_profile_get_by_app_id(param->reg.app_id);
        if (service)
        {
            TRACE_D("Found app-id: %d", service->app_id);
            service->gatts_if = gatts_if;
            service->status = GATT_STATUS_PROCESSING;
            esp_ble_gatts_create_service(gatts_if, &service->service_id, service->num_handles);
        }
        break;
#if 0
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
        esp_ble_gatts_create_service(gatts_if, &gl_profile_tab[PROFILE_WIFI_INFO_APP_ID].service_id, GATTS_NUM_HANDLE_WIFI_INFO);
        break;
#endif
    }
#if 0
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
#endif
    case ESP_GATTS_CREATE_EVT:
    {
        s_gatt_service_t *service = ezlopi_ble_profile_get_service_by_gatts_if(gatts_if);
        service->service_handle = param->create.service_handle;
        service->status = GATT_STATUS_DONE;
        esp_ble_gatts_start_service(service->service_handle);
        break;
    }
    case ESP_GATTS_START_EVT:
    {
        TRACE_I("SERVICE_START_EVT, status %d, service_handle %d", param->start.status, param->start.service_handle);
        s_gatt_service_t *service = ezlopi_ble_profile_get_service_by_gatts_if(gatts_if);
        s_gatt_char_t *char_to_add = ezlopi_ble_profile_get_characterstics_to_init(service);
        if (char_to_add)
        {
            char_to_add->status = GATT_STATUS_PROCESSING;
            esp_err_t err = esp_ble_gatts_add_char(service->service_handle, &char_to_add->uuid, char_to_add->permission,
                                                   char_to_add->property, &gatts_demo_char1_val, char_to_add->control);
            if (err)
            {
                TRACE_E("esp_ble_gatts_add_char: %s", esp_err_to_name(err));
            }
        }

        break;
    }
    case ESP_GATTS_ADD_CHAR_EVT:
    {
        uint16_t length = 0;
        const uint8_t *prf_char;

        TRACE_I("ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d",
                param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);

        s_gatt_service_t *service = ezlopi_ble_profile_get_service_by_gatts_if(gatts_if);
        s_gatt_char_t *char_initiating = ezlopi_ble_profile_get_initiating_characterstics(service);

        if (char_initiating)
        {
            // char_initiating->status = GATT_STATUS_DONE;
            char_initiating->handle = param->add_char.attr_handle;
            esp_err_t get_attr_ret = esp_ble_gatts_get_attr_value(param->add_char.attr_handle, &length, &prf_char);
            if (get_attr_ret == ESP_FAIL)
            {
                TRACE_E("ILLEGAL HANDLE");
            }

            s_gatt_descr_t *desc_to_init = ezlopi_ble_profile_get_descriptor_to_init(char_initiating);
            if (desc_to_init)
            {
                desc_to_init->status = GATT_STATUS_PROCESSING;
                esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(service->service_handle, &desc_to_init->uuid, desc_to_init->permission, NULL, NULL);
                if (add_descr_ret)
                {
                    TRACE_E("add char descr failed, error code =%x", add_descr_ret);
                }
            }
        }

        break;
    }
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
    {
        TRACE_I("ADD_DESCR_EVT, status %d, attr_handle %d, service_handle %d",
                param->add_char_descr.status, param->add_char_descr.attr_handle, param->add_char_descr.service_handle);

        s_gatt_service_t *service = ezlopi_ble_profile_get_service_by_gatts_if(gatts_if);
        TRACE_W("service: %d", (uint32_t)service);
        s_gatt_char_t *char_initiating = ezlopi_ble_profile_get_initiating_characterstics(service);
        TRACE_W("char_initiating: %d", (uint32_t)char_initiating);
        s_gatt_descr_t *desc_initiating = ezlopi_ble_profile_get_initiating_descriptor(char_initiating);
        TRACE_W("descriptor: %d", (uint32_t)desc_initiating);
        if (desc_initiating)
        {
            desc_initiating->status = GATT_STATUS_DONE;
            desc_initiating->handle = param->add_char_descr.attr_handle;
        }

        TRACE_W("Here 0");

        s_gatt_descr_t *desc_to_init = ezlopi_ble_profile_get_descriptor_to_init(char_initiating);
        TRACE_W("desc_to_init: %d", (uint32_t)desc_to_init);
        if (desc_to_init)
        {
            desc_to_init->status = GATT_STATUS_PROCESSING;
            esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(service->service_handle, &desc_to_init->uuid, desc_to_init->permission, NULL, NULL);
            if (add_descr_ret)
            {
                TRACE_E("add char descr failed, error code =%x", add_descr_ret);
            }
        }
        else
        {
            char_initiating->status = GATT_STATUS_DONE;

            s_gatt_char_t *char_to_add = ezlopi_ble_profile_get_characterstics_to_init(service);
            if (char_to_add)
            {
                char_to_add->status = GATT_STATUS_PROCESSING;
                esp_err_t err = esp_ble_gatts_add_char(service->service_handle, &char_to_add->uuid, char_to_add->permission,
                                                       char_to_add->property, &gatts_demo_char1_val, char_to_add->control);
                if (err)
                {
                    TRACE_E("esp_ble_gatts_add_char: %s", esp_err_to_name(err));
                }
            }
            else
            {
                service->status = GATT_STATUS_DONE;
            }
        }

        break;
    }

#if 0
    case ESP_GATTS_CONNECT_EVT:
    {
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
#endif

    default:
        break;
    }

#if 0
    /* If event is register event, store the gatts_if for each profile */

    if (ESP_GATTS_REG_EVT == event)
    {
        if (param->reg.status == ESP_GATT_OK)
        {
            // gl_profile_tab[param->reg.app_id].gatts_if = gatts_if;
        }
        else
        {
            TRACE_W("Reg app failed, app_id %04x, status %d", param->reg.app_id, param->reg.status);
            return;
        }
    }

    /* If the gatts_if equal to profile A, call profile A cb handler,
     * so here call each profile's callback */
    // do
    // {
    //     int idx;
    //     for (idx = 0; idx < sizeof(gl_profile_tab) / sizeof(gatts_profile_inst_t); idx++)
    //     {
    //         if (gatts_if == ESP_GATT_IF_NONE || /* ESP_GATT_IF_NONE, not specify a certain gatt_if, need to call every profile cb function */
    //             gatts_if == gl_profile_tab[idx].gatts_if)
    //         {
    //             if (gl_profile_tab[idx].gatts_cb)
    //             {
    //                 gl_profile_tab[idx].gatts_cb(event, gatts_if, param);
    //             }
    //         }
    //     }
    // } while (0);
#endif
}

static char *ezlopi_ble_gatt_event_to_string(esp_gatts_cb_event_t event)
{
    char *ret = "GATT event not defined!";

    switch (event)
    {
    case ESP_GATTS_REG_EVT:
    {
        ret = "ESP_GATTS_REG_EVT";
        break;
    }
    case ESP_GATTS_READ_EVT:
    {
        ret = "ESP_GATTS_READ_EVT";
        break;
    }
    case ESP_GATTS_WRITE_EVT:
    {
        ret = "ESP_GATTS_WRITE_EVT";
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT:
    {
        ret = "ESP_GATTS_EXEC_WRITE_EVT";
        break;
    }
    case ESP_GATTS_MTU_EVT:
    {
        ret = "ESP_GATTS_MTU_EVT";
        break;
    }
    case ESP_GATTS_CONF_EVT:
    {
        ret = "ESP_GATTS_CONF_EVT";
        break;
    }
    case ESP_GATTS_UNREG_EVT:
    {
        ret = "ESP_GATTS_UNREG_EVT";
        break;
    }
    case ESP_GATTS_CREATE_EVT:
    {
        ret = "ESP_GATTS_CREATE_EVT";
        break;
    }
    case ESP_GATTS_ADD_INCL_SRVC_EVT:
    {
        ret = "ESP_GATTS_ADD_INCL_SRVC_EVT";
        break;
    }
    case ESP_GATTS_ADD_CHAR_EVT:
    {
        ret = "ESP_GATTS_ADD_CHAR_EVT";
        break;
    }
    case ESP_GATTS_ADD_CHAR_DESCR_EVT:
    {
        ret = "ESP_GATTS_ADD_CHAR_DESCR_EVT";
        break;
    }
    case ESP_GATTS_DELETE_EVT:
    {
        ret = "ESP_GATTS_DELETE_EVT";
        break;
    }
    case ESP_GATTS_START_EVT:
    {
        ret = "ESP_GATTS_START_EVT";
        break;
    }
    case ESP_GATTS_STOP_EVT:
    {
        ret = "ESP_GATTS_STOP_EVT";
        break;
    }
    case ESP_GATTS_CONNECT_EVT:
    {
        ret = "ESP_GATTS_CONNECT_EVT";
        break;
    }
    case ESP_GATTS_DISCONNECT_EVT:
    {
        ret = "ESP_GATTS_DISCONNECT_EVT";
        break;
    }
    case ESP_GATTS_OPEN_EVT:
    {
        ret = "ESP_GATTS_OPEN_EVT";
        break;
    }
    case ESP_GATTS_CANCEL_OPEN_EVT:
    {
        ret = "ESP_GATTS_CANCEL_OPEN_EVT";
        break;
    }
    case ESP_GATTS_CLOSE_EVT:
    {
        ret = "ESP_GATTS_CLOSE_EVT";
        break;
    }
    case ESP_GATTS_LISTEN_EVT:
    {
        ret = "ESP_GATTS_LISTEN_EVT";
        break;
    }
    case ESP_GATTS_CONGEST_EVT:
    {
        ret = "ESP_GATTS_CONGEST_EVT";
        break;
    }
    case ESP_GATTS_RESPONSE_EVT:
    {
        ret = "ESP_GATTS_RESPONSE_EVT";
        break;
    }
    case ESP_GATTS_CREAT_ATTR_TAB_EVT:
    {
        ret = "ESP_GATTS_CREAT_ATTR_TAB_EVT";
        break;
    }
    case ESP_GATTS_SET_ATTR_VAL_EVT:
    {
        ret = "ESP_GATTS_SET_ATTR_VAL_EVT";
        break;
    }
    case ESP_GATTS_SEND_SERVICE_CHANGE_EVT:
    {
        ret = "ESP_GATTS_SEND_SERVICE_CHANGE_EVTbreak";
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}
