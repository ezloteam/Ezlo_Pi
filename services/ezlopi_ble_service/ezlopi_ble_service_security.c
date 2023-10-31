#include "string.h"

#include "lwip/ip_addr.h"
#include "cJSON.h"

#include "trace.h"
#include "ezlopi_wifi.h"

#include "ezlopi_nvs.h"
#include "ezlopi_ble_gap.h"
#include "ezlopi_ble_gatt.h"
#include "ezlopi_ble_profile.h"
#include "ezlopi_factory_info.h"
#include "ezlopi_nvs.h"

#include "ezlopi_ble_service.h"
#include "ezlopi_ble_buffer.h"

s_gatt_service_t *security_service = NULL;

#if (1 == EZLOPI_BLE_ENALBE_PASSKEY)
s_gatt_char_t *passkey_characterstic = NULL;
static void passkey_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);
#endif

s_gatt_char_t *factory_reset_characterstic = NULL;
static void factory_reset_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param);

#define CJ_GET_NUMBER(name) cJSON_GetNumberValue(cJSON_GetObjectItem(root, name))

void ezlopi_ble_service_security_init(void)
{
    esp_bt_uuid_t uuid;
    esp_gatt_perm_t permission;
    esp_gatt_char_prop_t properties;

    uuid.len = ESP_UUID_LEN_16;
    uuid.uuid.uuid16 = BLE_SECURITY_SERVICE_UUID;
    security_service = ezlopi_ble_gatt_create_service(BLE_SECURITY_SERVICE_HANDLE, &uuid);

#if (1 == EZLOPI_BLE_ENALBE_PASSKEY)
    uuid.uuid.uuid16 = BLE_SECURITY_CHAR_PASSKEY_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE;
    passkey_characterstic = ezlopi_ble_gatt_add_characteristic(security_service, &uuid, permission, properties, NULL, passkey_write_func, NULL);
#endif

    uuid.uuid.uuid16 = BLE_SECURITY_FACTORY_RESET_CHAR_UUID;
    uuid.len = ESP_UUID_LEN_16;
    permission = ESP_GATT_PERM_WRITE;
    properties = ESP_GATT_CHAR_PROP_BIT_WRITE;
    factory_reset_characterstic = ezlopi_ble_gatt_add_characteristic(security_service, &uuid, permission, properties, NULL, factory_reset_write_func, NULL);
}

#if (1 == EZLOPI_BLE_ENALBE_PASSKEY)
static void passkey_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{
    if (param->write.len == 4)
    {
        uint32_t passkey = *((uint32_t *)param->write.value);
        if (passkey < 1000000)
        {
            TRACE_D("New passkey: %d", passkey);
            ezlopi_ble_gap_set_passkey(passkey);
            ezlopi_nvs_write_ble_passkey(passkey);
            ezlopi_ble_gap_dissociate_bonded_devices();
        }
    }
}
#endif

static void factory_reset_write_func(esp_gatt_value_t *value, esp_ble_gatts_cb_param_t *param)
{

    if (param && param->write.len && param->write.value)
    {

        cJSON *root = cJSON_ParseWithLength((const char *)param->write.value, param->write.len);
        if (root)
        {

#if (1 == EZLOPI_BLE_ENALBE_PASSKEY)
            static uint32_t authenticated_flag;
            static uint32_t start_tick;
#endif

            uint32_t cmd = CJ_GET_NUMBER("cmd");

            TRACE_D("cmd: %d", cmd);
            if (2 == cmd) // factory reset command
            {
#if (1 == EZLOPI_BLE_ENALBE_PASSKEY)
                uint32_t current_tick = xTaskGetTickCount();
                if ((current_tick - start_tick) < (30 * 1000 / portTICK_RATE_MS) && (1 == authenticated_flag)) // once authenticated, valid for 30 seconds only
                {
#endif
                    int ret = ezlopi_factory_info_v2_factory_reset();
                    if (ret)
                    {
                        TRACE_I("FLASH RESET WAS DONE SUCCESSFULLY");
                    }

                    ret = ezlopi_nvs_factory_reset();
                    if (ret)
                    {
                        TRACE_I("NVS-RESET WAS DONE SUCCESSFULLY");
                    }

                    TRACE_B("factory reset done, rebooting now .............................................");
                    vTaskDelay(2000 / portTICK_RATE_MS);
                    esp_restart();
#if (1 == EZLOPI_BLE_ENALBE_PASSKEY)
                }
                else
                {
                    authenticated_flag = 0;
                    TRACE_W("Not authenticated for factory-reset!");
                }
#endif
            }
#if (1 == EZLOPI_BLE_ENALBE_PASSKEY)
            else if (1 == cmd) // authentication request for soft-factory-reset
            {

                uint32_t passkey = CJ_GET_NUMBER("passkey");
                uint32_t original_passkey = 0;
                ezlopi_nvs_read_ble_passkey(&original_passkey);

                TRACE_D("Old passkey: %u, current_passkey: %u", original_passkey, passkey);

                if (passkey == original_passkey)
                {
                    authenticated_flag = 1;
                    TRACE_W("Authenticated!");
                    start_tick = xTaskGetTickCount();
                }
                else
                {
                    authenticated_flag = 0;
                    TRACE_W("Not authenticated!");
                }
            }
#endif
            else
            {
                TRACE_W("Command not valid: [cmd: %u].", cmd);
            }

            cJSON_free(root);
        }
    }
}