#include "string.h"
#include "time.h"

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "trace.h"
#include "ezlopi_nvs.h"
// #include "interface_common.h"

static nvs_handle_t ezlopi_nvs_handle = 0;
static const char *storage_name = "storage";
static const char *config_nvs_name = "config_data";
static const char *passkey_nvs_name = "passkey";
static const char *user_id_nvs_name = "user_id";
static const char *wifi_info_nvs_name = "wifi_info";
static const char *boot_count_nvs_name = "boot_count";
static const char *provisioning_status_nvs_name = "prov_stat";
static const char *ezlopi_scenes_nvs_name = "ezlopi_scenes";
static const char *ezlopi_scenes_v2_nvs_name = "ez_scenes_v2";
static const char *ezlopi_scripts_nvs_ids = "ezlopi_scripts";
static const char *settings_initialized_status_name = "settings_magic";

int ezlopi_nvs_init(void)
{
    int ret = 0;
    if (0 == ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_flash_init();

        if (ESP_ERR_NVS_NO_FREE_PAGES == err || ESP_ERR_NVS_NEW_VERSION_FOUND == err)
        {
            TRACE_D("NVS Init Failed once!, Error: %s", esp_err_to_name(err));
            ESP_ERROR_CHECK(nvs_flash_erase());
            err = nvs_flash_init();
        }

        err = nvs_open(storage_name, NVS_READWRITE, &ezlopi_nvs_handle);
        if (ESP_OK != err)
        {
            TRACE_E("NVS Open Error!");
            // vTaskDelay(2000 / portTICK_RATE_MS);
        }
        else
        {
            ret = 1;
            TRACE_D("NVS Open success");
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

int ezlopi_nvs_scene_set_v2(char *scene)
{
    int ret = 0;
    if (scene)
    {
        ret = ezlopi_nvs_write_str(scene, strlen(scene) + 1, ezlopi_scenes_v2_nvs_name);
    }
    return ret;
}

char *ezlopi_nvs_scene_get_v2(void)
{
    return ezlopi_nvs_read_str(ezlopi_scenes_v2_nvs_name);
}

int ezlopi_nvs_scene_set(char *scene)
{
    int ret = 0;
    if (scene)
    {
        ret = ezlopi_nvs_write_str(scene, strlen(scene) + 1, ezlopi_scenes_nvs_name);
    }
    return ret;
}

char *ezlopi_nvs_scene_get(void)
{
    return ezlopi_nvs_read_str(ezlopi_scenes_nvs_name);
}

int ezlopi_nvs_factory_reset(void)
{
    int ret = 0;
    if (ESP_OK == nvs_flash_erase())
    {
        ret = 1;
    }

    return ret;
}

int ezlopi_nvs_write_scenes_scripts(char *data)
{
    int ret = 0;

    if (1 == ezlopi_nvs_write_str(data, strlen(data), (char *)ezlopi_scripts_nvs_ids))
    {
        ret = 1;
    }

    return ret;
}

char *ezlopi_nvs_read_scenes_scripts(void)
{
    return ezlopi_nvs_read_str(ezlopi_scripts_nvs_ids);
}

int ezlopi_nvs_write_config_data_str(char *data)
{
    int ret = 0;
    if (1 == ezlopi_nvs_write_str(data, strlen(data), config_nvs_name))
    {
        ret = 1;
    }
    return ret;
}

char *ezlopi_nvs_read_config_data_str(void)
{
    return ezlopi_nvs_read_str(config_nvs_name);
}

int ezlopi_nvs_read_ble_passkey(uint32_t *passkey)
{
    const uint32_t default_passkey = 123456;
    int ret = 0;
    if (passkey)
    {
        if (1 == ezlopi_nvs_init())
        {
            esp_err_t err = ESP_OK;
            err = nvs_get_u32(ezlopi_nvs_handle, passkey_nvs_name, passkey);
            TRACE_W("nvs_get_u32 - error: %s", esp_err_to_name(err));

            if (ESP_OK == err)
            {
                ret = 1;
            }
            else
            {
                *passkey = 0;
            }
        }

        *passkey = ((0 == *passkey) || (*passkey > 999999)) ? default_passkey : *passkey;
    }

    return ret;
}

int ezlopi_nvs_write_ble_passkey(uint32_t passkey)
{
    int ret = 0;

    if (1 == ezlopi_nvs_init())
    {
        esp_err_t err = ESP_OK;
        err = nvs_set_u32(ezlopi_nvs_handle, passkey_nvs_name, passkey);
        TRACE_W("nvs_set_u32 - error: %s", esp_err_to_name(err));

        if (ESP_OK == err)
        {
            ret = 1;
        }
    }

    return ret;
}

int ezlopi_nvs_write_wifi(const char *wifi_info, uint32_t len)
{
    int ret = 0;
    if (1 == ezlopi_nvs_write_str(wifi_info, len, wifi_info_nvs_name))
    {
        ret = 1;
    }
    return ret;
}

int ezlopi_nvs_read_wifi(char *wifi_info, uint32_t len)
{
    int ret = 0;
    if (1 == ezlopi_nvs_init())
    {
        size_t required_size;
        esp_err_t err = nvs_get_blob(ezlopi_nvs_handle, wifi_info_nvs_name, NULL, &required_size);
        if ((ESP_OK == err) && (len >= required_size))
        {
            err = nvs_get_blob(ezlopi_nvs_handle, wifi_info_nvs_name, wifi_info, &required_size);
            TRACE_W("Error nvs_get_blob: %s", esp_err_to_name(err));
            if (ESP_OK == err)
            {
                ret = 1;
                TRACE_D("Load wifi config:: ssid: %s, password: %s", wifi_info, &wifi_info[32]);
            }
        }
        else
        {
            TRACE_E("'wifi config' read-lenght error!, Required: %d | %d, Error: %s", required_size, len, esp_err_to_name(err));
        }
    }
    return ret;
}

int ezlopi_nvs_write_user_id_str(char *data)
{
    int ret = 0;
    if (1 == ezlopi_nvs_init())
    {
        esp_err_t err;
        err = nvs_set_str(ezlopi_nvs_handle, user_id_nvs_name, data);
        TRACE_W("ezlopi_nvs_write_user_id_str - error: %s", esp_err_to_name(err));

        if (ESP_OK == err)
        {
            err = nvs_commit(ezlopi_nvs_handle);

            if (ESP_OK == err)
            {
                ret = 1;
            }
            else
            {
                TRACE_E("ezlopi_nvs_write_user_id_str - error: %s", esp_err_to_name(err));
            }
        }
    }

    return ret;
}

char *ezlopi_nvs_read_user_id_str(void)
{
    return ezlopi_nvs_read_str(user_id_nvs_name);
}

void ezlopi_nvs_deinit(void)
{
    nvs_close(ezlopi_nvs_handle);
    ezlopi_nvs_handle = 0;
}

void ezlopi_nvs_set_provisioning_status(void)
{
    if (ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_set_u32(ezlopi_nvs_handle, provisioning_status_nvs_name, 1);
        TRACE_E("nvs_set_u32 - error: %s", esp_err_to_name(err));
    }
}

uint32_t ezlopi_nvs_get_provisioning_status(void)
{
    uint32_t provisioning_status = 0;
    if (ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_get_u32(ezlopi_nvs_handle, provisioning_status_nvs_name, &provisioning_status);
        TRACE_I("Provisioning_Status: %d", provisioning_status);
        if (ESP_OK != err)
        {
            provisioning_status = 0;
            TRACE_E("Error nvs_get_u32: %s", esp_err_to_name(err));
        }
    }

    return provisioning_status;
}

void ezlopi_nvs_set_boot_count(uint32_t boot_count)
{
    if (ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_set_u32(ezlopi_nvs_handle, boot_count_nvs_name, boot_count);
        TRACE_W("nvs_set_u32 - error: %s", esp_err_to_name(err));
    }
}

uint32_t ezlopi_nvs_get_boot_count(void)
{
    uint32_t boot_count = 1;
    if (ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_get_u32(ezlopi_nvs_handle, boot_count_nvs_name, &boot_count);
        TRACE_I("Boot count: %d", boot_count);
        TRACE_D("Error nvs_get_blob: %s", esp_err_to_name(err));
        if (ESP_OK != err)
        {
            err = nvs_set_u32(ezlopi_nvs_handle, boot_count_nvs_name, boot_count);
            TRACE_W("nvs_set_u32 - error: %s", esp_err_to_name(err));
        }
    }

    return boot_count;
}

uint8_t ezlopi_nvs_get_settings_init_status(void)
{
    uint8_t ret = 0;
    uint32_t ezlopi_settings_magic_number = 0;
    if (ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_get_u32(ezlopi_nvs_handle, settings_initialized_status_name, &ezlopi_settings_magic_number);
        if (ESP_OK == err)
        {
            if (EZLOPI_SETTINGS_MAGIC_NUMBER == ezlopi_settings_magic_number)
            {
                ret = 1;
            }
        }
        else
        {
            TRACE_W("nvs_get_u32 - error: %s", esp_err_to_name(err));
        }
    }
    return ret;
}

uint8_t ezlopi_nvs_set_settings_init_status(void)
{
    uint8_t ret = 0;
    if (ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_set_u32(ezlopi_nvs_handle, settings_initialized_status_name, EZLOPI_SETTINGS_MAGIC_NUMBER);
        if (ESP_OK != err)
        {
            TRACE_W("nvs_set_u32 - error: %s", esp_err_to_name(err));
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

#if 0
int ezlopi_settings_save_settings(const s_ezlopi_hub_settings_t *settings_list, uint16_t num_settings)
{
    int ret = 1;
    esp_err_t err = ESP_OK;
    if (ezlopi_nvs_handle)
    {
        for (size_t i = 0; i < num_settings; i++)
        {
            const s_ezlopi_hub_settings_t *setting = &settings_list[i];
            if (setting->value_type == EZLOPI_SETTINGS_TYPE_TOKEN)
            {
                err = nvs_set_str(ezlopi_nvs_handle, setting->name, setting->value.token_value);
                if (ESP_OK != err)
                {
                    TRACE_E("Error writing string value to NVS for key: %s, Value: %s", setting->name, setting->value.token_value);
                    ret = 0;
                }
                else
                {
                    TRACE_D("Success writing string value to NVS for key: %s, Value: %s", setting->name, setting->value.token_value);
                }
            }
            else if (setting->value_type == EZLOPI_SETTINGS_TYPE_BOOL)
            {
                err = nvs_set_u8(ezlopi_nvs_handle, setting->name, (uint8_t)setting->value.bool_value);
                if (ESP_OK != err)
                {
                    TRACE_E("Error writing u8 value to NVS for key: %s, Value: %s", setting->name, setting->value.bool_value ? "True" : "False");
                    ret = 0;
                }
                else
                {
                    TRACE_D("Success writing u8 value to NVS for key: %s, Value: %s", setting->name, setting->value.bool_value ? "True" : "False");
                }
            }
            else if (setting->value_type == EZLOPI_SETTINGS_TYPE_INT)
            {
                err = nvs_set_i32(ezlopi_nvs_handle, setting->name, setting->value.int_value);
                if (ESP_OK != err)
                {
                    TRACE_E("Error writing int32 value to NVS for key: %s, Value: %d", setting->name, setting->value.int_value);
                    ret = 0;
                }
                else
                {
                    TRACE_D("Success writing int32 value to NVS for key: %s, Value: %d", setting->name, setting->value.int_value);
                }
            }
        }
    }
    else
    {
        TRACE_E("NVS Handle invalid.");
        ret = 0;
    }

    return ret;
}

int ezlopi_settings_retrive_settings(s_ezlopi_hub_settings_t *settings_list, uint16_t num_settings)
{
    int ret = 1;
    esp_err_t err = ESP_OK;
    if (ezlopi_nvs_handle)
    {
        for (size_t i = 0; i < num_settings; i++)
        {

            s_ezlopi_hub_settings_t *setting = &settings_list[i];

            if (setting->value_type == EZLOPI_SETTINGS_TYPE_TOKEN)
            {

                char *enum_settings_value = NULL;
                size_t required_size;
                nvs_get_str(ezlopi_nvs_handle, setting->name, NULL, &required_size);

                enum_settings_value = malloc(required_size);
                err = nvs_get_str(ezlopi_nvs_handle, setting->name, enum_settings_value, &required_size);

                if (ESP_OK != err)
                {
                    TRACE_E("Error reading string value from NVS for key: %s", setting->name);
                    ret = 0;
                }
                else
                {
                    if (NULL != enum_settings_value)
                    {
                        setting->value.token_value = enum_settings_value;
                        TRACE_I("settings_name: %s, settings_value: %s, settings_value_len: %d",
                                setting->name,
                                setting->value.token_value,
                                strlen(setting->value.token_value));
                    }
                    else
                    {
                        TRACE_E("Error: Read NULL string from NVS for key: %s", setting->name);
                        ret = 0;
                    }
                    TRACE_D("Success reading string value from NVS for key: %s", setting->name);
                }
            }
            else if (setting->value_type == EZLOPI_SETTINGS_TYPE_BOOL)
            {

                uint8_t settings_bool_val;
                bool settings_bool_val_bool;
                err = nvs_get_u8(ezlopi_nvs_handle, setting->name, &settings_bool_val);

                if (ESP_OK != err)
                {
                    TRACE_E("Error reading uint8 value from NVS for key: %s", setting->name);
                    ret = 0;
                }
                else
                {
                    settings_bool_val_bool = (settings_bool_val == 1) ? true : false;
                    setting->value.bool_value = settings_bool_val_bool;
                    TRACE_D("Success reading uint8 value from NVS for key: %s", setting->name);
                    TRACE_I("settings_name: %s, settings_value: %s",
                            setting->name,
                            setting->value.bool_value ? "True" : "False");
                }
            }
            else if (setting->value_type == EZLOPI_SETTINGS_TYPE_INT)
            {

                int32_t settings_int_val = 0;
                err = nvs_get_i32(ezlopi_nvs_handle, setting->name, &settings_int_val);
                if (ESP_OK != err)
                {
                    TRACE_E("Error reading int32 value from NVS for key: %s", setting->name);
                    ret = 0;
                }
                else
                {
                    setting->value.int_value = settings_int_val;
                    TRACE_D("Success reading int32 value from NVS for key: %s", setting->name);
                    TRACE_I("settings_name: %s, settings_value: %d",
                            setting->name,
                            setting->value.int_value);
                }
            }
        }
    }
    else
    {
        TRACE_E("NVS Handle invalid.");
        ret = 0;
    }

    return ret;
}

#endif

uint8_t ezlopi_nvs_write_int32(int32_t i, const char *key_name)
{
    uint8_t ret = 0;
    if (ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_set_i32(ezlopi_nvs_handle, key_name, i);
        if (ESP_OK != err)
        {
            TRACE_W("nvs_set_i32 - error: %s", esp_err_to_name(err));
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

uint8_t ezlopi_nvs_read_int32(int32_t *i, const char *key_name)
{
    uint8_t ret = 0;
    if (ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_get_i32(ezlopi_nvs_handle, key_name, i);
        if (ESP_OK == err)
        {
            ret = 1;
        }
        else
        {
            TRACE_W("nvs_get_i32 - error: %s", esp_err_to_name(err));
        }
    }
    return ret;
}

uint8_t ezlopi_nvs_write_float32(float f, const char *key_name)
{
    uint8_t ret = 0;
    if (ezlopi_nvs_handle)
    {
        uint32_t value;
        memcpy(&value, &f, sizeof(uint32_t));

        esp_err_t err = nvs_set_u32(ezlopi_nvs_handle, key_name, value);
        if (err != ESP_OK)
        {
            TRACE_W("nvs_set_u32 - error: %s", esp_err_to_name(err));
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

uint8_t ezlopi_nvs_read_float32(float *f, const char *key_name)
{
    uint8_t ret = 0;
    if (ezlopi_nvs_handle)
    {
        uint32_t value;

        esp_err_t err = nvs_get_u32(ezlopi_nvs_handle, key_name, &value);
        if (err == ESP_OK)
        {
            memcpy(f, &value, sizeof(float));
            ret = 1;
        }
        else
        {
            TRACE_W("nvs_get_u32 - error: %s", esp_err_to_name(err));
        }
    }
    return ret;
}

uint8_t ezlopi_nvs_write_bool(bool b, const char *key_name)
{
    uint8_t ret = 0;
    if (ezlopi_nvs_handle)
    {
        uint8_t bool_val;

        if (true == b)
        {
            bool_val = 1;
        }
        else
        {
            bool_val = 0;
        }

        esp_err_t err = nvs_set_u8(ezlopi_nvs_handle, key_name, bool_val);

        if (ESP_OK != err)
        {
            TRACE_W("nvs_set_u8 - error: %s", esp_err_to_name(err));
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

uint8_t ezlopi_nvs_read_bool(bool *b, const char *key_name)
{
    uint8_t ret = 0;
    if (ezlopi_nvs_handle)
    {
        uint8_t bool_val = 0;
        esp_err_t err = nvs_get_u8(ezlopi_nvs_handle, key_name, &bool_val);
        if (ESP_OK == err)
        {
            if (bool_val)
                *b = true;
            else
                *b = false;
            ret = 1;
        }
        else
        {
            TRACE_W("nvs_get_u8 - error: %s", esp_err_to_name(err));
        }
    }
    return ret;
}

int ezlopi_nvs_write_str(char *data, uint32_t len, char *nvs_name)
{
    int ret = 0;

    if (data && nvs_name && len)
    {
        if (1 == ezlopi_nvs_init())
        {
            esp_err_t err = nvs_set_str(ezlopi_nvs_handle, nvs_name, data);
            if (ESP_OK == err)
            {
                err = nvs_commit(ezlopi_nvs_handle);
                if (ESP_OK == err)
                {
                    ret = 1;
                    TRACE_D("%s commit success.", nvs_name);
                }
                else
                {
                    TRACE_E("%s commit error: %s", nvs_name, esp_err_to_name(err));
                }
            }
            else
            {
                TRACE_E("%s write error: %s", nvs_name, esp_err_to_name(err));
            }
        }
    }

    return ret;
}

char *ezlopi_nvs_read_str(char *nvs_name)
{
    char *return_str = NULL;

    if (1 == ezlopi_nvs_init())
    {
        esp_err_t err = ESP_OK;
        size_t buf_len_needed = 0;
        err = nvs_get_str(ezlopi_nvs_handle, nvs_name, NULL, &buf_len_needed);

        if (buf_len_needed && (ESP_OK == err))
        {
            return_str = malloc(buf_len_needed + 1);

            if (return_str)
            {
                memset(return_str, 0, buf_len_needed + 1);
                err = nvs_get_str(ezlopi_nvs_handle, nvs_name, return_str, &buf_len_needed);

                if (ESP_OK == err)
                {
                    TRACE_D("%s read success. \r\nData[%d]: \r\n%s", nvs_name, buf_len_needed, return_str);
                }
                else
                {
                    TRACE_E("%s read error: %s", nvs_name, esp_err_to_name(err));
                    free(return_str);
                    return_str = NULL;
                }
            }
            else
            {
                TRACE_E("MALLOC ERROR");
            }
        }
        else
        {
            TRACE_E("%s: buf_len_needed: %d, err: %s", nvs_name, buf_len_needed, esp_err_to_name(err));
        }
    }

    return return_str;
}

void ezlopi_nvs_delete_stored_script(uint32_t script_id)
{
    esp_err_t err = ESP_OK;
    if (1 == ezlopi_nvs_init())
    {
        char script_id_str[32];
        snprintf(script_id_str, sizeof(script_id_str), "%08x", script_id);
        if (ESP_OK != (err = nvs_erase_key(ezlopi_nvs_handle, script_id_str)))
        {
            TRACE_E("Erasing nvs-key '%s' failed!, error: %s", script_id_str, esp_err_to_name(err));
        }
    }
}
