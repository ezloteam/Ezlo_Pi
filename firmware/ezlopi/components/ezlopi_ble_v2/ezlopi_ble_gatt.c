#include <string.h>

#include "trace.h"
#include "ezlopi_ble_gap.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_config.h"
#include "ezlopi_ble_profile.h"

#define GATTS_DEMO_CHAR_VAL_LEN_MAX 0x40
static uint8_t char1_str[GATTS_DEMO_CHAR_VAL_LEN_MAX] = {0x11, 0x22, 0x33};

static esp_attr_value_t gatts_demo_char1_val = {
    .attr_max_len = GATTS_DEMO_CHAR_VAL_LEN_MAX,
    .attr_len = sizeof(char1_str),
    .attr_value = char1_str,
};

static char *ezlopi_ble_gatt_event_to_string(esp_gatts_cb_event_t event);
static void ezlopi_ble_gatt_call_read_by_handle(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
static void ezlopi_ble_gatt_call_write_by_handle(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

void ezlopi_ble_gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    TRACE_B("BLE GATT Event: %s, gatts_if: %d", ezlopi_ble_gatt_event_to_string(event), gatts_if);

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
            TRACE_D("service->num_handles: %d", service->num_handles);
        }
        break;
    }
#if 0
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
#endif
    case ESP_GATTS_CREATE_EVT:
    {
        s_gatt_service_t *service = ezlopi_ble_profile_get_service_by_gatts_if(gatts_if);
        service->service_handle = param->create.service_handle;
        service->status = GATT_STATUS_DONE;
        esp_ble_gatts_start_service(service->service_handle);
        TRACE_D("service->service_handle: %d", service->service_handle);
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
            // esp_err_t err = esp_ble_gatts_add_char(service->service_handle, &char_to_add->uuid, char_to_add->permission,
            //                                        char_to_add->property, &gatts_demo_char1_val, &char_to_add->control);
            esp_gatt_char_prop_t a_property = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
            esp_err_t err = esp_ble_gatts_add_char(service->service_handle, &char_to_add->uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
                                                   a_property, NULL, NULL);
            TRACE_W("Add characterstics:: permission: %02x, properties: %d", char_to_add->permission, char_to_add->property);
            TRACE_W("Add characterstics:: permission: %02x, properties: %d", ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE, a_property);
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
                esp_err_t add_descr_ret = esp_ble_gatts_add_char_descr(service->service_handle, &desc_to_init->uuid, desc_to_init->permission, &gatts_demo_char1_val, NULL);
                TRACE_W("Descriptor:: permission: %02x", desc_to_init->permission);

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
        s_gatt_char_t *char_initiating = ezlopi_ble_profile_get_initiating_characterstics(service);
        s_gatt_descr_t *desc_initiating = ezlopi_ble_profile_get_initiating_descriptor(char_initiating);
        if (desc_initiating)
        {
            desc_initiating->status = GATT_STATUS_DONE;
            desc_initiating->handle = param->add_char_descr.attr_handle;
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
        else
        {
            char_initiating->status = GATT_STATUS_DONE;

            s_gatt_char_t *char_to_add = ezlopi_ble_profile_get_characterstics_to_init(service);
            if (char_to_add)
            {
                char_to_add->status = GATT_STATUS_PROCESSING;
                esp_err_t err = esp_ble_gatts_add_char(service->service_handle, &char_to_add->uuid, char_to_add->permission,
                                                       char_to_add->property, &gatts_demo_char1_val, &char_to_add->control);
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
    case ESP_GATTS_READ_EVT:
    {
        ezlopi_ble_gatt_call_read_by_handle(gatts_if, param);
        break;
        // esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
#if 0
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
#endif
    }
    case ESP_GATTS_WRITE_EVT:
    {
        ezlopi_ble_gatt_call_write_by_handle(gatts_if, param);
        break;
    }
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
        ezlopi_ble_gap_start_advertising();
        break;
    }
    case ESP_GATTS_MTU_EVT: // 4
    {
        TRACE_I("ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        break;
    }
    default:
    {
        TRACE_W("BLE GATT Event: %s Not Implemented!", ezlopi_ble_gatt_event_to_string(event));
        break;
    }
    }
}

static f_upcall_t ezlopi_ble_gatt_call_by_handle(esp_gatt_if_t gatts_if, uint16_t handle, bool is_read)
{
    s_gatt_service_t *service = ezlopi_ble_profile_get_service_by_gatts_if(gatts_if);
    if (service)
    {
        s_gatt_char_t *characteristic = service->characteristics;
        while (characteristic)
        {
            if (handle == characteristic->handle)
            {
                TRACE_I("Is a characteristic %s.", is_read ? "read" : "write");
                ezlopi_ble_gatt_print_characteristic(characteristic);
                if (is_read)
                {
                    return characteristic->read_upcall;
                }
                else
                {
                    return characteristic->write_upcall;
                }
            }

            s_gatt_descr_t *descriptor = characteristic->descriptor;
            while (descriptor)
            {
                if (handle == descriptor->handle)
                {
                    TRACE_I("Is a descriptor %s.", is_read ? "read" : "write");
                    ezlopi_ble_gatt_print_descriptor(descriptor);
                    if (is_read)
                    {
                        return descriptor->read_upcall;
                    }
                    else
                    {
                        return descriptor->write_upcall;
                    }
                }
                descriptor = descriptor->next;
            }

            characteristic = characteristic->next;
        }
    }

    return NULL;
}

static void ezlopi_ble_gatt_call_write_by_handle(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    f_upcall_t write_upcall = ezlopi_ble_gatt_call_by_handle(gatts_if, param->write.handle, 0);
    if (write_upcall)
    {
        write_upcall(NULL);
    }
}

static void ezlopi_ble_gatt_call_read_by_handle(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    f_upcall_t read_upcall = ezlopi_ble_gatt_call_by_handle(gatts_if, param->read.handle, 1);

    if (read_upcall)
    {
        read_upcall(NULL);
    }

    // esp_gatt_rsp_t rsp;
    // memset(&rsp, 0, sizeof(esp_gatt_rsp_t));
    // rsp.attr_value.handle = param->read.handle;
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
