#include "string.h"

#include "lwip/ip_addr.h"
#include "cJSON.h"

#include "trace.h"
#include "ezlopi_wifi.h"

#include "ezlopi_nvs.h"
#include "ezlopi_ble_gap.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_profile.h"
#include "ezlopi_nvs.h"

#include "ezlopi_ble_service.h"
#include "ezlopi_ble_buffer.h"

static s_linked_buffer_t *user_id_linked_buffer = NULL;

static void user_id_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
static void user_id_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
static void user_id_write_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);

void ezlopi_ble_service_user_id_init(void)
{
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t permission;
    esp_gatt_char_prop_t properties;
    s_gatt_service_t *service;

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = 0x00E3;
    service = ezlopi_ble_gatt_create_service(BLE_USER_ID_SERVICE_HANDLE, &uuid);
    uuid.uuid.uuid16 = 0xE301;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_WRITE | ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ;
    ezlopi_ble_gatt_add_characteristic(service, &uuid, permission, properties, user_id_read_func, user_id_write_func, user_id_write_exec_func);
}

static void user_id_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (value)
    {
        static char *user_id;
        if (NULL == user_id)
        {
            ezlopi_nvs_read_user_id_str(&user_id);
        }

        if (NULL != user_id)
        {
            uint32_t total_data_len = strlen(user_id);
            uint32_t max_data_buffer_size = ezlopi_ble_gatt_get_max_data_size();
            uint32_t copy_size = ((total_data_len - param->read.offset) < max_data_buffer_size) ? (total_data_len - param->read.offset) : max_data_buffer_size;
            if ((0 != total_data_len) && (total_data_len > param->read.offset))
            {
                strncpy((char *)value->value, user_id + param->read.offset, copy_size);
                value->len = copy_size;
            }
            else
            {
                value->len = 1;
                value->value[0] = 0; // Read 0 if the device not provisioned yet.
            }

            if ((param->read.offset + copy_size) >= total_data_len)
            {
                free(user_id);
                user_id = NULL;
            }
        }
        else
        {
            value->len = 1;
            value->value[0] = 0; // Read 0 if the device not provisioned yet.
        }
    }
}

static void user_id_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Write function called!");
    if (0 == param->write.is_prep) // Data received in single packet
    {
        dump("GATT_WRITE_EVT value", param->write.value, 0, param->write.len);

        if (param->write.len == 2)
        {
            uint16_t descr_value = (param->write.value[1] << 8) | param->write.value[0];
            if (descr_value == 0x0001)
            {
            }
            else if (descr_value == 0x0002)
            {
            }
            else if (descr_value == 0x0000)
            {
                TRACE_I("notify/indicate disable");
            }
            else
            {
                TRACE_E("unknown descr value");
            }
        }
        else if ((NULL != param->write.value) && (param->write.len > 2))
        {
            char tmp_buffer[37];
            strncpy(tmp_buffer, (char *)param->write.value, (36 < param->write.len) ? 36 : param->write.len);
            ezlopi_nvs_write_user_id_str(tmp_buffer);
        }
    }
    else
    {
        if (NULL == user_id_linked_buffer)
        {
            user_id_linked_buffer = ezlopi_ble_buffer_create(param);
        }
        else
        {
            ezlopi_ble_buffer_add_to_buffer(user_id_linked_buffer, param);
        }
    }
}

static void user_id_write_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Write execute function called.");
    ezlopi_ble_buffer_accumulate_to_start(user_id_linked_buffer);
    char tmp_buffer[37];
    strncpy(tmp_buffer, (char *)user_id_linked_buffer->buffer, 36);
    ezlopi_nvs_write_user_id_str(tmp_buffer);
    ezlopi_ble_buffer_free_buffer(user_id_linked_buffer);
    user_id_linked_buffer = NULL;
}
