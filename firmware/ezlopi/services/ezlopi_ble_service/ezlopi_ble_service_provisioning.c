#include "string.h"

#include "cJSON.h"
#include "lwip/ip_addr.h"
#include "esp_event_base.h"

#include "trace.h"
#include "ezlopi_wifi.h"

#include "ezlopi_nvs.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_profile.h"

#include "ezlopi_ble_service.h"
#include "ezlopi_ble_buffer.h"
#include "mbedtls/base64.h"

static s_gatt_service_t *g_provisioning_service;
static s_linked_buffer_t *g_provisioning_linked_buffer = NULL;

static void ezlopi_process_provisioning_info(uint8_t *value, uint32_t len);

static void provisioning_info_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
static void provisioning_info_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
static void provisioning_info_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);

void ezlopi_ble_service_provisioning_init(void)
{
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t permission;
    esp_gatt_char_prop_t properties;

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = 0xFF00;
    g_provisioning_service = ezlopi_ble_gatt_create_service(BLE_PROVISIONING_ID_HANDLE, &uuid);
    TRACE_W("'provisioning_service' service added to list");

    uuid.uuid.uuid16 = 0xFF01;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;
    ezlopi_ble_gatt_add_characteristic(g_provisioning_service, &uuid, permission, properties, provisioning_info_read_func, provisioning_info_write_func, provisioning_info_exec_func);
    TRACE_W("'provisioning_service' service added to list");
}

static void provisioning_info_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Write function called!");
    dump("GATT_WRITE_EVT value", param->write.value, 0, param->write.len);

    if (0 == param->write.is_prep) // Data received in single packet
    {
        if ((NULL != param->write.value) && (param->write.len > 0))
        {
            ezlopi_process_provisioning_info(param->write.value, param->write.len);
        }
    }
    else
    {
        if (NULL == g_provisioning_linked_buffer)
        {
            g_provisioning_linked_buffer = ezlopi_ble_buffer_create(param);
        }
        else
        {
            ezlopi_ble_buffer_add_to_buffer(g_provisioning_linked_buffer, param);
        }
    }
}

static void provisioning_info_read_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Read function called!");
}

static void provisioning_info_exec_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    TRACE_D("Write execute function called.");
    ezlopi_ble_buffer_accumulate_to_start(g_provisioning_linked_buffer);
    ezlopi_process_provisioning_info(g_provisioning_linked_buffer->buffer, g_provisioning_linked_buffer->len);
    ezlopi_ble_buffer_free_buffer(g_provisioning_linked_buffer);
    g_provisioning_linked_buffer = NULL;
}

static void ezlopi_process_provisioning_info(uint8_t *value, uint32_t len)
{
    if ((NULL != value) && (len > 0))
    {
        cJSON *root = cJSON_Parse((const char *)value);
        if (root)
        {
            TRACE_D("value = %s", value);
            char *user_id = cJSON_GetObjectItemCaseSensitive(root, "user_id")->valuestring;
            char *device_name = cJSON_GetObjectItemCaseSensitive(root, "device_name")->valuestring;
            char *brand = cJSON_GetObjectItemCaseSensitive(root, "brand")->valuestring;
            char *manufacturer_name = cJSON_GetObjectItemCaseSensitive(root, "manufacturer_name")->valuestring;
            char *model_number = cJSON_GetObjectItemCaseSensitive(root, "model_number")->valuestring;
            char *uuid = cJSON_GetObjectItemCaseSensitive(root, "uuid")->valuestring;
            char *uuid_provisioning = cJSON_GetObjectItemCaseSensitive(root, "uuid_provisioning")->valuestring;
            double serial = cJSON_GetObjectItemCaseSensitive(root, "serial")->valuedouble;
            char *cloud_server = cJSON_GetObjectItemCaseSensitive(root, "cloud_server")->valuestring;
            char *ssl_private_key = cJSON_GetObjectItemCaseSensitive(root, "ssl_private_key")->valuestring;
            char *ssl_public_key = cJSON_GetObjectItemCaseSensitive(root, "ssl_public_key")->valuestring;
            char *ca_cert = cJSON_GetObjectItemCaseSensitive(root, "ca_cert")->valuestring;
            char *device_type_ezlopi = cJSON_GetObjectItemCaseSensitive(root, "device_type_ezlopi")->valuestring;

            TRACE_D("************************* BLE-PROVISIONING *************************");
            TRACE_D("user_id:               %s", user_id ? user_id : "");
            TRACE_D("device_name:           %s", device_name ? device_name : "");
            TRACE_D("brand:                 %s", brand ? brand : "");
            TRACE_D("manufacturer_name:     %s", manufacturer_name ? manufacturer_name : "");
            TRACE_D("model_number:          %s", model_number ? model_number : "");
            TRACE_D("uuid:                  %s", uuid ? uuid : "");
            TRACE_D("uuid_provisioning:     %s", uuid_provisioning ? uuid_provisioning : "");
            TRACE_D("serial:                %f", serial);
            TRACE_D("cloud_server:          %s", cloud_server ? cloud_server : "");
            TRACE_D("ssl_private_key:       %s", ssl_private_key ? ssl_private_key : "");
            TRACE_D("ssl_public_key:        %s", ssl_public_key ? ssl_public_key : "");
            TRACE_D("ca_cert:               %s", ca_cert ? ca_cert : "");
            TRACE_D("device_type_ezlopi:    %s", device_type_ezlopi ? device_type_ezlopi : "");
            TRACE_D("********************************************************************");

            cJSON_Delete(root);
        }
        else
        {
            TRACE_E("Invalid json packet received!");
        }
    }
}

// ewogICAgInVzZXJfaWQiOiAibG9tYXNzdWJlZGkiLAogICAgImRldmljZV9uYW1lIjogIk15IERldmljZSIsCiAgICAiYnJhbmQiOiAiTkRTIFRoZXJtb3N0YXQiLAogICAgIm1hbnVmYWN0dXJlcl9uYW1lIjogIk5lcGFsIERpZ2l0YWwgU3lzdGVtcyIsCiAgICAibW9kZWxfbnVtYmVyIjogIjA2M0RFWDUyNCIsCiAgICAidXVpZCI6ICI2NTI2MWQ3Ni1lNTg0LTRkMzUtYWZmMS1kODRiZDA0MyIsCiAgICAidXVpZF9wcm92aXNpb25pbmciOiAiNWZlNmI0OTgtOTdiNi00NjdhLTk1OTgtYWJmMmViM2IxOTVmIiwKICAgICJzZXJpYWwiOiAxMDAwMDQwMzIsCiAgICAiY2xvdWRfc2VydmVyIjogImh0dHBzOi8vY2xvdWQuZXpsby5jb206NzAwMCIsCiAgICAic3NsX3ByaXZhdGVfa2V5IjogIi0tLS0tQkVHSU4gUFJJVkFURSBLRVktLS0tLVxuTUlHRUFnRUFNQkFHQnlxR1NNNDlBZ0VHQlN1QkJBQUtCRzB3YXdJQkFRUWdsc2RGM0srU0tUdGExSEhIMERueVxuNitoT3kxT29ab1J3c1pZY2RjeGRRYWloUkFOQ0FBUnUzRExuWnZRMXQ0aG1oZVVyUThLSm5abWRKWEUzdGw2RVxuWGk0eXpxMW9kYjI5ZFNLaU5DQmovTUo2bXVtL2RxVEhVTjY1OHZkSE5xanJXbnlXenZPNFxuLS0tLS1FTkQgUFJJVkFURSBLRVktLS0tLVxuIiwKICAgICJzc2xfcHVibGljX2tleSI6ICItLS0tLUJFR0lOIFBVQkxJQyBLRVktLS0tLVxuTUZZd0VBWUhLb1pJemowQ0FRWUZLNEVFQUFvRFFnQUVidHd5NTJiME5iZUlab1hsSzBQQ2laMlpuU1Z4TjdaZVxuaEY0dU1zNnRhSFc5dlhVaW9qUWdZL3pDZXBycHYzYWt4MURldWZMM1J6YW82MXA4bHM3enVBPT1cbi0tLS0tRU5EIFBVQkxJQyBLRVktLS0tLSIsCiAgICAiY2FfY2VydCI6ICItLS0tLUJFR0lOIHhhQ0VSVElGSUNBVEUtLS0tLVxyXG5NSUlDYkRDQ0FoR2dBd0lCQWdJSkFPQnl6YUk3YUhZOU1Bb0dDQ3FHU000OUJBTURNSUdRTVFzd0NRWURWUVFHXHJcbkV3SlZVekVVTUJJR0ExVUVDQXdMSUU1bGR5QktaWEp6WlhreEVEQU9CZ05WQkFjTUIwTnNhV1owYjI0eER6QU5cclxuQmdOVkJBb01Ca2xVSUU5d2N6RVBNQTBHQTFVRUN3d0dTVlFnVDNCek1SUXdFZ1lEVlFRRERBdGxXa3hQSUV4VVxyXG5SQ0JEUVRFaE1COEdDU3FHU0liM0RRRUpBUllTYzNsellXUnRhVzV6UUdWNmJHOHVZMjl0TUNBWERURTVNRFV6XHJcbk1URTNNREUwTjFvWUR6SXhNVGt3TlRBM01UY3dNVFEzV2pDQmtERUxNQWtHQTFVRUJoTUNWVk14RkRBU0JnTlZcclxuQkFnTUN5Qk9aWGNnU21WeWMyVjVNUkF3RGdZRFZRUUhEQWREYkdsbWRHOXVNUTh3RFFZRFZRUUtEQVpKVkNCUFxyXG5jSE14RHpBTkJnTlZCQXNNQmtsVUlFOXdjekVVTUJJR0ExVUVBd3dMWlZwTVR5Qk1WRVFnUTBFeElUQWZCZ2txXHJcbmhraUc5dzBCQ1FFV0VuTjVjMkZrYldsdWMwQmxlbXh2TG1OdmJUQldNQkFHQnlxR1NNNDlBZ0VHQlN1QkJBQUtcclxuQTBJQUJITFFkaExEWXNhZklGWThwWmg5NmFER3FWbTZFNHI4blc5czRDZmRwWGFhL1I0Q25qYVZwRFFJN1VtUVxyXG45dlZER1puOG1jbW03VmpLeCtUU0NTME1JS09qVXpCUk1CMEdBMVVkRGdRV0JCUmlUbDhFejFsOTRqYXFjeGJpXHJcbnl4a1ZDMEZrQlRBZkJnTlZIU01FR0RBV2dCUmlUbDhFejFsOTRqYXFjeGJpeXhrVkMwRmtCVEFQQmdOVkhSTUJcclxuQWY4RUJUQURBUUgvTUFvR0NDcUdTTTQ5QkFNREEwa0FNRVlDSVFEN0VVczhqNTBqS0ZkLzQ2Wm85NU5iclBZUVxyXG5QdExUSEg5WWpVa01Fa1lENWdJaEFNUDR5N0UxYUI3OG5Rcm1kM0lYOE1NMzJrOWRNOHhUME16dFIxNk90c3VWXHJcbi0tLS0tRU5EIENFUlRJRklDQVRFLS0tLS0iLAogICAgImRldmljZV90eXBlX2V6bG9waSI6ICJnZW5lcmljIgp9