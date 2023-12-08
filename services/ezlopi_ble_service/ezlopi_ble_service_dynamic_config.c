#include "string.h"
#include "time.h"

#include "cJSON.h"
#include "lwip/ip_addr.h"
#include "esp_event_base.h"
#include "mbedtls/base64.h"

#include "trace.h"

#include "ezlopi_nvs.h"
#include "ezlopi_wifi.h"
#include "ezlopi_devices.h"
#include "ezlopi_ble_auth.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_buffer.h"
#include "ezlopi_ble_profile.h"
#include "ezlopi_ble_service.h"
#include "ezlopi_factory_info.h"
#include "esp_gatt_common_api.h"

#define CJ_GET_STRING(name) cJSON_GetStringValue(cJSON_GetObjectItem(root, name))
#define CJ_GET_NUMBER(name) cJSON_GetNumberValue(cJSON_GetObjectItem(root, name))

static s_gatt_service_t *g_dynamic_config_service = NULL;
static s_linked_buffer_t *g_dynamic_config_linked_buffer = NULL;

static char *__dynamic_config_jsonify(void);
static char *__dynamic_config_base64(void);
static char *__base64_decode_dynamic_config(uint32_t total_size);

static void __dynamic_config_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
static void __dynamic_config_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);

#if 0
static void __process_dynamic_config(uint8_t *value, uint32_t len);
static void __dynamic_config_write_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
#endif

static void __provisioning_status_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);

void ezlopi_ble_service_dynamic_config_init(void)
{
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t permission;
    esp_gatt_char_prop_t properties;

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = BLE_DYNAMIC_CONFIG_SERVICE_UUID;
    g_dynamic_config_service = ezlopi_ble_gatt_create_service(BLE_DYNAMIC_CONFIG_HANDLE, &uuid);

    uuid.uuid.uuid16 = BLE_DYNAMIC_CONFIG_CHAR_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_WRITE | ESP_GATT_PERM_READ;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_NOTIFY | ESP_GATT_CHAR_PROP_BIT_INDICATE;
    ezlopi_ble_gatt_add_characteristic(g_dynamic_config_service, &uuid, permission, properties, __dynamic_config_read_func, __dynamic_config_write_func, NULL); // reliable-write is not implemented for now
}

static char *__provisioning_status_jsonify(void)
{
    char *prov_status_jstr = NULL;
    cJSON *root = cJSON_CreateObject();
    if (root)
    {
        uint32_t prov_stat = ezlopi_nvs_get_provisioning_status();
        if (1 == prov_stat)
        {
            cJSON_AddNumberToObject(root, "version", ezlopi_factory_info_v2_get_version());
            cJSON_AddNumberToObject(root, "status", prov_stat);
        }
        else
        {
            cJSON_AddNumberToObject(root, "version", 0);
            cJSON_AddNumberToObject(root, "status", 0);
        }

        char tmp_buffer[32];
        snprintf(tmp_buffer, sizeof(tmp_buffer), "%08x", ezlopi_nvs_config_info_version_number_get());
        cJSON_AddStringToObject(root, "config_id", tmp_buffer);
        cJSON_AddNumberToObject(root, "config_time", ezlopi_nvs_config_info_update_time_get());

        prov_status_jstr = cJSON_Print(root);
        if (prov_status_jstr)
        {
            cJSON_Minify(prov_status_jstr);
        }

        cJSON_Delete(root);
    }

    return prov_status_jstr;
}

static void __provisioning_status_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (value)
    {
        static char *prov_status_jstr;
        if (NULL == prov_status_jstr)
        {
            prov_status_jstr = __provisioning_status_jsonify();
        }

        if (NULL != prov_status_jstr)
        {
            uint32_t total_data_len = strlen(prov_status_jstr);
            uint32_t max_data_buffer_size = ezlopi_ble_gatt_get_max_data_size();
            uint32_t copy_size = ((total_data_len - param->read.offset) < max_data_buffer_size) ? (total_data_len - param->read.offset) : max_data_buffer_size;

            if ((0 != total_data_len) && (total_data_len > param->read.offset))
            {
                strncpy((char *)value->value, prov_status_jstr + param->read.offset, copy_size);
                value->len = copy_size;
            }
            else
            {
                value->len = 1;
                value->value[0] = 0; // Read 0 if the device not provisioned yet.
            }

            if ((param->read.offset + copy_size) >= total_data_len)
            {
                free(prov_status_jstr);
                prov_status_jstr = NULL;
            }
        }
        else
        {
            TRACE_E("Unable to create json string");
            value->len = 1;
            value->value[0] = 0; // Read 0 if the device not provisioned yet.
        }
    }
    else
    {
        TRACE_E("VALUE IS NULL");
    }
}

static void __dynamic_config_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Write function called!");
    TRACE_D("GATT_WRITE_EVT value: %.*s", param->write.len, param->write.value);

    ezlopi_nvs_set_provisioning_status();

    if (NULL == g_dynamic_config_linked_buffer)
    {
        g_dynamic_config_linked_buffer = ezlopi_ble_buffer_create(param);
    }
    else
    {
        ezlopi_ble_buffer_add_to_buffer(g_dynamic_config_linked_buffer, param);
    }

    if (g_dynamic_config_linked_buffer)
    {
        if ((NULL != param->write.value) && (param->write.len > 0))
        {
            cJSON *root = cJSON_ParseWithLength((const char *)param->write.value, param->write.len);
            if (root)
            {

                uint32_t len = CJ_GET_NUMBER("len");
                uint32_t tot_len = CJ_GET_NUMBER("total_len");
                uint32_t sequence = CJ_GET_NUMBER("sequence");

                TRACE_D("Len: %d", len);
                TRACE_D("tot_len: %d", tot_len);
                TRACE_D("sequence: %d", sequence);

                if (sequence && len && tot_len)
                {
                    if (((sequence - 1) * 400 + len) >= tot_len)
                    {
                        char *decoded_data = __base64_decode_dynamic_config(tot_len); // uncommente f
                        if (decoded_data)
                        {

                            ezlopi_factory_info_v2_set_ezlopi_config(decoded_data);

#if 0
                            cJSON *cj_config = cJSON_Parse(decoded_data);
                            if (cj_config)
                            {
                                char *user_id = NULL;
                                CJSON_GET_VALUE_STRING(cj_config, "user_id", user_id);

                                if (user_id && (BLE_AUTH_SUCCESS == ezlopi_ble_auth_check_user_id(user_id)))
                                {
                                    cJSON_DeleteItemFromObject(cj_config, "user_id");
                                    char *save_config_str = cJSON_Print(cj_config);
                                    if (save_config_str)
                                    {
                                        TRACE_D("here");
                                        ezlopi_factory_info_v2_set_ezlopi_config(save_config_str);
                                        free(save_config_str);
                                    }
                                }
                                else
                                {
                                    TRACE_E("User varification failed!");

                                    char *curr_user_id = ezlopi_nvs_read_user_id_str();
                                    if (curr_user_id)
                                    {
                                        TRACE_D("current user: %s", curr_user_id);
                                        free(curr_user_id);
                                    }
                                }

                                cJSON_Delete(cj_config);
                            }
#endif
                            free(decoded_data);
                        }

                        ezlopi_ble_buffer_free_buffer(g_dynamic_config_linked_buffer);
                        g_dynamic_config_linked_buffer = NULL;
                    }
                }
            }
        }
    }
}

static void __dynamic_config_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Read function called!");

    static char *g_dynamic_config_base64;
    static uint32_t g_dynamic_config_sequence_no;
    static time_t g_provisioning_last_read_time;
    static uint32_t g_dynamic_config_number_of_sequence;
    static const time_t gc_provisioning_read_timeout_s = 2;
    static const uint32_t g_required_ble_prov_buffer_size = 517;

    // timeout logic
    int status = -1; // success for non negative, failed for negative
    time_t time_now = 0;

    time(&time_now);
    if ((time_now - g_provisioning_last_read_time) >= gc_provisioning_read_timeout_s)
    {
        g_dynamic_config_sequence_no = 0;
    }

    time(&g_provisioning_last_read_time);

    if (value)
    {
        if (NULL == g_dynamic_config_base64)
        {
            g_dynamic_config_base64 = __dynamic_config_base64();

            g_dynamic_config_sequence_no = 0;
            g_dynamic_config_number_of_sequence = strlen(g_dynamic_config_base64) / ezlopi_ble_gatt_get_max_data_size();
            g_dynamic_config_number_of_sequence = (strlen(g_dynamic_config_base64) % ezlopi_ble_gatt_get_max_data_size()) ? (g_dynamic_config_number_of_sequence + 1) : g_dynamic_config_number_of_sequence;
        }

        if (NULL != g_dynamic_config_base64)
        {
            if (ezlopi_ble_gatt_get_max_data_size() >= g_required_ble_prov_buffer_size)
            {
                uint32_t total_data_len = strlen(g_dynamic_config_base64);
                uint32_t copy_size = total_data_len - (g_dynamic_config_sequence_no * 400);
                copy_size = (copy_size > 400) ? 400 : copy_size;

                TRACE_B("copy_size: %d", copy_size);
                TRACE_B("total_data_len: %d", total_data_len);

                cJSON *cj_response = cJSON_CreateObject();
                if (cj_response)
                {
                    static char data_buffer[400 + 1];
                    snprintf(data_buffer, sizeof(data_buffer), "%.*s", copy_size, g_dynamic_config_base64 + (g_dynamic_config_sequence_no * 400));

                    cJSON_AddNumberToObject(cj_response, "len", copy_size);
                    cJSON_AddNumberToObject(cj_response, "total_len", total_data_len);
                    cJSON_AddNumberToObject(cj_response, "sequence", g_dynamic_config_sequence_no);
                    cJSON_AddStringToObject(cj_response, "data", data_buffer);

                    char *send_data = cJSON_Print(cj_response);
                    if (send_data)
                    {
                        TRACE_D("data: %s", send_data);
                        cJSON_Minify(send_data);

                        if ((0 != total_data_len) && (total_data_len >= ((g_dynamic_config_sequence_no * 400) + copy_size)))
                        {
                            value->len = strlen(send_data);
                            strncpy((char *)value->value, send_data, value->len + 1);

                            TRACE_B("data: %s", (char *)value->value);

                            g_dynamic_config_sequence_no += 1;
                            status = 0;

                            if (copy_size < 400) // Done reading
                            {
                                status = 1; // non negative for done reading
                                free(g_dynamic_config_base64);
                                g_dynamic_config_base64 = NULL;
                            }
                        }
                        else
                        {
                            TRACE_W("Check value: %d", ((g_dynamic_config_sequence_no * 400) + copy_size));
                            TRACE_W("total_data_len: %d", total_data_len);
                            status = -4;
                        }

                        free(send_data);
                    }
                    else
                    {
                        status = -3;
                    }

                    cJSON_Delete(cj_response);
                }
                else
                {
                    status = -2;
                }
            }
            else
            {
                TRACE_E("MTU size must be greater than or equal to %d!", g_required_ble_prov_buffer_size);
                TRACE_W("call SET-MTU API from client stack!");

                CHECK_PRINT_ERROR(esp_ble_gatt_set_local_mtu(517), "set local  MTU failed");
                status = -2;
            }
        }

        if (status)
        {
            if (status < 0)
            {
                TRACE_E("Error found: %d", status);
                value->len = 1;
                value->value[0] = 0; // Read 0 if the device not provisioned yet.
            }
            else
            {
                TRACE_I("Done Transmitting.");
            }

            g_dynamic_config_sequence_no = 0;
            g_dynamic_config_number_of_sequence = 0;

            if (g_dynamic_config_base64)
            {
                free(g_dynamic_config_base64);
                g_dynamic_config_base64 = NULL;
            }
        }
    }
    else
    {
        TRACE_E("VALUE IS NULL");
    }
}

#if 0
static void __dynamic_config_write_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (g_dynamic_config_linked_buffer)
    {
        TRACE_D("Write execute function called.");
        ezlopi_ble_buffer_accumulate_to_start(g_dynamic_config_linked_buffer);
        __process_dynamic_config(g_dynamic_config_linked_buffer->buffer, g_dynamic_config_linked_buffer->len);
        ezlopi_ble_buffer_free_buffer(g_dynamic_config_linked_buffer);
        g_dynamic_config_linked_buffer = NULL;
    }
}

static void __process_dynamic_config(uint8_t *value, uint32_t len)
{
    if ((NULL != value) && (len > 0))
    {
        cJSON *cj_config = cJSON_Parse((const char *)value);
        if (cj_config)
        {
            char *user_id = NULL;
            CJSON_GET_VALUE_STRING(cj_config, "user_id", user_id);

            if (user_id && (BLE_AUTH_SUCCESS == ezlopi_ble_auth_check_user_id(user_id)))
            {
                cJSON *cj_device_config = cJSON_GetObjectItem(cj_config, "device_config");
                if (cj_device_config)
                {
                    cJSON_DeleteItemFromObject(cj_config, "cmd");
                    cJSON_AddNumberToObject(cj_device_config, "cmd", 3);
                    char *tmp_str = cJSON_Print(cj_device_config);
                    if (tmp_str)
                    {
                        TRACE_D("device_config: %s", tmp_str);
                        ezlopi_factory_info_v2_set_ezlopi_config(tmp_str);
                        free(tmp_str);
                    }
                }
            }
            else
            {
                TRACE_E("User varification failed!");

                char *curr_user_id = ezlopi_nvs_read_user_id_str();
                if (curr_user_id)
                {
                    TRACE_D("current user: %s", curr_user_id);
                    free(curr_user_id);
                }
            }

            cJSON_Delete(cj_config);
        }
        else
        {
            TRACE_E("Invalid json packet received!");
        }
    }
}
#endif

static char *__base64_decode_dynamic_config(uint32_t total_size)
{
    char *decoded_config_json = NULL;
    char *base64_buffer = malloc(total_size + 1);

    if (base64_buffer)
    {
        uint32_t pos = 0;
        s_linked_buffer_t *tmp_prov_buffer = g_dynamic_config_linked_buffer;

        while (tmp_prov_buffer)
        {
            // TRACE_W("tmp_prov_buffer->buffer[%d]: %.*s", tmp_prov_buffer->len, tmp_prov_buffer->len, (char *)tmp_prov_buffer->buffer);
            cJSON *root = cJSON_ParseWithLength((const char *)tmp_prov_buffer->buffer, tmp_prov_buffer->len);
            if (root)
            {
                uint32_t len = CJ_GET_NUMBER("len");
                // uint32_t tot_len = CJ_GET_NUMBER("total_len");
                // uint32_t sequence = CJ_GET_NUMBER("sequence");
                char *data = CJ_GET_STRING("data");
                if (data)
                {
                    memcpy(base64_buffer + pos, data, len);
                    pos += len;
                    base64_buffer[pos] = '\0';
                }
                else
                {
                    TRACE_E("DATA IS NULL");
                }
            }
            else
            {
                TRACE_E("Failed to parse");
            }

            tmp_prov_buffer = tmp_prov_buffer->next;
        }

        TRACE_D("base64_buffer: %s", base64_buffer);

        decoded_config_json = malloc(total_size);
        if (decoded_config_json)
        {
            size_t o_len = 0;
            bzero(decoded_config_json, total_size);
            mbedtls_base64_decode((uint8_t *)decoded_config_json, (size_t)total_size, &o_len, (uint8_t *)base64_buffer, strlen(base64_buffer));
            TRACE_D("Decoded data: %s", decoded_config_json);
        }
        else
        {
            TRACE_E("mALLOC FAILED");
        }

        free(base64_buffer);
    }

    return decoded_config_json;
}

#if 0
static char *__dynamic_config_jsonify(void)
{
    char *str_json_prov_info = NULL;

    char *device_config = ezlopi_factory_info_v2_get_ezlopi_config(); // don't free this, it is being used by other modules as well

    if (device_config)
    {
        TRACE_D("device-config: %s", device_config);
        cJSON *cj_device_config = cJSON_Parse(device_config);

        if (cj_device_config)
        {
            cJSON_DeleteItemFromObject(cj_device_config, "cmd");
            cJSON_AddNumberToObject(cj_device_config, "config_time", ezlopi_nvs_config_info_update_time_get());
            cJSON_AddNumberToObject(cj_device_config, "_")
            str_json_prov_info = cJSON_Print(cj_device_config);
            if (str_json_prov_info)
            {
                cJSON_Minify(str_json_prov_info);
            }
        }
    }

    return str_json_prov_info;
}
#endif

static char *__dynamic_config_base64(void)
{
    const uint32_t base64_data_len = 4096;
    char *base64_data = malloc(base64_data_len);
    if (base64_data)
    {
        uint32_t out_put_len = 0;
        char *str_provisioning_data = ezlopi_factory_info_v2_get_ezlopi_config(); // do not free 'str_provisioning_data', it is used by other modules
        if (str_provisioning_data)
        {
            TRACE_D("str_provisioning_data[len: %d]: %s", strlen(str_provisioning_data), str_provisioning_data);

            int ret = mbedtls_base64_encode((unsigned char *)base64_data, base64_data_len, &out_put_len, (const unsigned char *)str_provisioning_data, strlen(str_provisioning_data));

            TRACE_D("'mbedtls_base64_encode' returned: %04x", ret);
        }

        TRACE_D("out-put-len: %d", out_put_len);
        TRACE_D("base64_data[len: %d]: %s", strlen(base64_data), base64_data);

        if (0 == out_put_len)
        {
            free(base64_data);
            base64_data = NULL;
        }
    }

    return base64_data;
}
