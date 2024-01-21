#include <string.h>

#include "trace.h"
#include "ezlopi_ble_gap.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_config.h"
#include "ezlopi_ble_profile.h"
#include "esp_gatt_common_api.h"

static uint16_t g_mtu_size = ESP_GATT_DEF_BLE_MTU_SIZE;

static char *ezlopi_ble_gatt_event_to_string(esp_gatts_cb_event_t event);
static void ezlopi_ble_gatt_call_read_by_handle(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
static void ezlopi_ble_gatt_call_write_by_handle(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
static void ezlopi_ble_gatt_call_write_exec_by_handle(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

uint16_t ezlopi_ble_gatt_get_max_data_size(void)
{
    return g_mtu_size;
}

void ezlopi_ble_gatts_characteristic_notify(s_gatt_service_t *service, s_gatt_char_t *characteristics, esp_gatt_value_t *value)
{
    esp_ble_gatts_send_indicate(service->gatts_if, service->conn_id, characteristics->handle, value->len, value->value, false);
}

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
            ezlopi_ble_gap_config_adv_data();
            // ezlopi_ble_gap_config_scan_rsp_data();

            service->gatts_if = gatts_if;
            service->status = GATT_STATUS_PROCESSING;
            esp_ble_gatts_create_service(gatts_if, &service->service_id, service->num_handles);
            TRACE_D("service->num_handles: %d", service->num_handles);
        }
        else
        {
            TRACE_D("service is null");
        }
        break;
    }
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
            esp_err_t err = esp_ble_gatts_add_char(service->service_handle, &char_to_add->uuid, char_to_add->permission, char_to_add->property, NULL, NULL);
            // ezlopi_ble_gatt_print_characteristic(char_to_add);
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

        TRACE_I("ADD_CHAR_EVT, status %d,  attr_handle %d, service_handle %d",
                param->add_char.status, param->add_char.attr_handle, param->add_char.service_handle);

        s_gatt_service_t *service = ezlopi_ble_profile_get_service_by_gatts_if(gatts_if);
        s_gatt_char_t *char_initiating = ezlopi_ble_profile_get_initiating_characterstics(service);

        if (char_initiating)
        {
            char_initiating->handle = param->add_char.attr_handle;
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
                                                           char_to_add->property, NULL, NULL);
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
            // ezlopi_ble_gatt_print_descriptor(desc_to_init);

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
                                                       char_to_add->property, NULL, NULL);
                // ezlopi_ble_gatt_print_characteristic(char_to_add);
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
        // esp_gatt_rsp_t rsp = {.attr_value = {.len = 0, .value[0] = 0}, .handle = param->read.handle};
        // esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
        break;
    }
    case ESP_GATTS_WRITE_EVT:
    {
        ezlopi_ble_gatt_call_write_by_handle(gatts_if, param);
        break;
    }
    case ESP_GATTS_EXEC_WRITE_EVT:
    {
        ezlopi_ble_gatt_call_write_exec_by_handle(gatts_if, param);
        break;
    }
#if (1 == EZLOPI_BLE_ENALBE_PAIRING)
    case ESP_GATTS_CONNECT_EVT:
    {
        TRACE_I("ESP_GATTS_CONNECT_EVT");
        /* start security connect with peer device when receive the connect event sent by the master */
        esp_ble_set_encryption(param->connect.remote_bda, ESP_BLE_SEC_ENCRYPT_MITM);
        break;
    }
#endif
    case ESP_GATTS_DISCONNECT_EVT:
    {
        TRACE_I("ESP_GATTS_DISCONNECT_EVT, disconnect reason 0x%x", param->disconnect.reason);
        ezlopi_ble_gap_start_advertising();
        break;
    }
    case ESP_GATTS_MTU_EVT: // 4
    {
        ezlopi_ble_setup_adv_config();
        TRACE_I("ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
        g_mtu_size = param->mtu.mtu;
        break;
    }
    case ESP_GATTS_RESPONSE_EVT:
    default:
    {
        TRACE_W("BLE GATT Event: %s Not Implemented!", ezlopi_ble_gatt_event_to_string(event));
        break;
    }
    }
}

static f_upcall_t ezlopi_ble_gatt_call_by_handle(esp_gatt_if_t gatts_if, uint16_t handle, esp_gatts_cb_event_t event)
{
    s_gatt_service_t *service = ezlopi_ble_profile_get_service_by_gatts_if(gatts_if);
    if (service)
    {
        s_gatt_char_t *characteristic = service->characteristics;
        while (characteristic)
        {
            if (ESP_GATTS_EXEC_WRITE_EVT == event)
            {
                return characteristic->write_exce_upcall;
            }

            if (handle == characteristic->handle)
            {
                switch (event)
                {
                case ESP_GATTS_READ_EVT:
                {
                    TRACE_I("Is a characteristic 'read'.");
                    return characteristic->read_upcall;
                }
                case ESP_GATTS_WRITE_EVT:
                {
                    TRACE_I("Is a characteristic 'write'.");
                    return characteristic->write_upcall;
                }
                case ESP_GATTS_EXEC_WRITE_EVT:
                {
                    TRACE_I("Is a characteristic 'write_exce'.");
                    return characteristic->write_exce_upcall;
                }
                default:
                {
                    TRACE_I("Characteristic upcall not found!");
                    return NULL;
                }
                }
            }

            s_gatt_descr_t *descriptor = characteristic->descriptor;
            while (descriptor)
            {
                if (handle == descriptor->handle)
                {
                    TRACE_I("Is a descriptor %s.", event ? "read" : "write");
                    // ezlopi_ble_gatt_print_descriptor(descriptor);
                    switch (event)
                    {
                    case ESP_GATTS_READ_EVT:
                    {
                        TRACE_I("Is a descriptor 'read'.");
                        return descriptor->read_upcall;
                    }
                    case ESP_GATTS_WRITE_EVT:
                    {
                        TRACE_I("Is a descriptor 'write'.");
                        return descriptor->write_upcall;
                    }
                    case ESP_GATTS_EXEC_WRITE_EVT:
                    {
                        TRACE_I("Is a descriptor 'write_exce'.");
                        return descriptor->write_exce_upcall;
                    }
                    default:
                    {
                        TRACE_I("Descriptor upcall not found!");
                        return NULL;
                    }
                    }
                }
                descriptor = descriptor->next;
            }

            characteristic = characteristic->next;
        }
    }
    else
    {
        TRACE_E("Service not found!");
    }

    return NULL;
}

static void ezlopi_ble_gatt_call_write_by_handle(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    f_upcall_t write_upcall = ezlopi_ble_gatt_call_by_handle(gatts_if, param->write.handle, ESP_GATTS_WRITE_EVT);
    if (write_upcall)
    {
        esp_gatt_rsp_t gatt_rsp;
        write_upcall(&gatt_rsp.attr_value, param);

        if (param->write.need_rsp)
        {
            if (param->write.is_prep)
            {
                gatt_rsp.attr_value.len = param->write.len;
                gatt_rsp.attr_value.handle = param->write.handle;
                gatt_rsp.attr_value.offset = param->write.offset;
                gatt_rsp.attr_value.auth_req = ESP_GATT_AUTH_REQ_NONE;
                memcpy(gatt_rsp.attr_value.value, param->write.value, param->write.len);
                esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, &gatt_rsp);
            }
            else
            {
                esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
            }
        }
    }
}

static void ezlopi_ble_gatt_call_write_exec_by_handle(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    f_upcall_t write_exec_upcall = ezlopi_ble_gatt_call_by_handle(gatts_if, param->write.handle, ESP_GATTS_EXEC_WRITE_EVT);
    if (write_exec_upcall)
    {
        esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
        if (ESP_GATT_PREP_WRITE_EXEC == param->exec_write.exec_write_flag)
        {
            write_exec_upcall(NULL, param);
        }
    }
    else
    {
        TRACE_E("'write_exec_upcall' not found!");
    }
}

static void ezlopi_ble_gatt_call_read_by_handle(esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    f_upcall_t read_upcall = ezlopi_ble_gatt_call_by_handle(gatts_if, param->read.handle, ESP_GATTS_READ_EVT);

    if (read_upcall)
    {
        esp_gatt_rsp_t rsp;
        rsp.attr_value.handle = param->read.handle;
        read_upcall(&rsp.attr_value, param);
        esp_ble_gatts_send_response(gatts_if, param->read.conn_id, param->read.trans_id, ESP_GATT_OK, &rsp);
    }
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
