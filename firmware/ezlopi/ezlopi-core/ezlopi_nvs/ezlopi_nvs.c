#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "trace.h"
#include "ezlopi_nvs.h"
// #include "interface_common.h"

static nvs_handle_t ezlopi_nvs_handle;
static const char *storage_name = "storage";
static const char *config_nvs_name = "config_data";
static const char *passkey_nvs_name = "passkey";
static const char *user_id_nvs_name = "user_id";
static const char *wifi_info_nvs_name = "wifi_info";
static const char *boot_count_nvs_name = "boot_count";
static const char *provisioning_time_nvs_name = "prov_time";

void ezlopi_nvs_init(void)
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
        TRACE_D("NVS Open success");
    }
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

int ezlopi_nvs_write_config_data_str(char *data)
{
    int ret = 0;

    esp_err_t err;
    nvs_handle_t config_nvs_handle;

    err = nvs_open(storage_name, NVS_READWRITE, &config_nvs_handle);
    TRACE_W("nvs_open - error: %s", esp_err_to_name(err));

    if (ESP_OK == err)
    {
        err = nvs_set_str(config_nvs_handle, config_nvs_name, data);
        TRACE_W("nvs_set_str - error: %s", esp_err_to_name(err));

        if (ESP_OK == err)
        {
            err = nvs_commit(config_nvs_handle);
            TRACE_W("nvs_commit - error: %s", esp_err_to_name(err));

            if (ESP_OK == err)
            {
                ret = 1;
            }
        }

        nvs_close(config_nvs_handle);
    }

    return ret;
}

int ezlopi_nvs_read_config_data_str(char **data)
{
    int ret = 0;
    esp_err_t err = ESP_OK;

    nvs_handle_t config_nvs_handle;
    err = nvs_open(storage_name, NVS_READWRITE, &config_nvs_handle);

    if (ESP_OK == err)
    {
        size_t buf_len_needed;
        err = nvs_get_str(config_nvs_handle, config_nvs_name, NULL, &buf_len_needed);

        if (buf_len_needed && (ESP_OK == err))
        {
            *data = malloc(buf_len_needed + 1);

            if (*data)
            {
                err = nvs_get_str(config_nvs_handle, config_nvs_name, *data, &buf_len_needed);
                TRACE_W("nvs_get_str(data) error: %s", esp_err_to_name(err));

                if (ESP_OK == err)
                {
                    ret = 1;
                }
                else
                {
                    free(*data);
                    *data = NULL;
                }
            }
        }

        nvs_close(config_nvs_handle);
    }

    return ret;
}

int ezlopi_nvs_read_ble_passkey(uint32_t *passkey)
{
    const uint32_t default_passkey = 123456;
    int ret = 0;
    if (passkey)
    {
        esp_err_t err = ESP_OK;

        nvs_handle_t config_nvs_handle;
        err = nvs_open(storage_name, NVS_READWRITE, &config_nvs_handle);

        if (ESP_OK == err)
        {
            err = nvs_get_u32(config_nvs_handle, passkey_nvs_name, passkey);
            TRACE_W("nvs_get_u32 - error: %s", esp_err_to_name(err));

            if (ESP_OK == err)
            {
                ret = 1;
            }
            else
            {
                *passkey = 0;
            }

            nvs_close(config_nvs_handle);
        }

        *passkey = ((0 == *passkey) || (*passkey > 999999)) ? default_passkey : *passkey;
    }

    return ret;
}

int ezlopi_nvs_write_ble_passkey(uint32_t passkey)
{
    int ret = 0;
    esp_err_t err = ESP_OK;

    nvs_handle_t config_nvs_handle;
    err = nvs_open(storage_name, NVS_READWRITE, &config_nvs_handle);

    if (ESP_OK == err)
    {
        err = nvs_set_u32(config_nvs_handle, passkey_nvs_name, passkey);
        TRACE_W("nvs_set_u32 - error: %s", esp_err_to_name(err));

        if (ESP_OK == err)
        {
            ret = 1;
        }

        nvs_close(config_nvs_handle);
    }

    return ret;
}

void ezlopi_nvs_write_wifi(const char *wifi_info, uint32_t len)
{
    esp_err_t err = nvs_set_blob(ezlopi_nvs_handle, wifi_info_nvs_name, wifi_info, len);
    TRACE_D("'write_wifi' Error nvs_set_blob: %s", esp_err_to_name(err));
    err = nvs_commit(ezlopi_nvs_handle);
    TRACE_D("'write_wifi' Error nvs_commit: %s", esp_err_to_name(err));
}

void ezlopi_nvs_read_wifi(char *wifi_info, uint32_t len)
{
    size_t required_size;
    esp_err_t err = nvs_get_blob(ezlopi_nvs_handle, wifi_info_nvs_name, NULL, &required_size);
    if ((ESP_OK == err) && (len >= required_size))
    {
        err = nvs_get_blob(ezlopi_nvs_handle, wifi_info_nvs_name, wifi_info, &required_size);
        TRACE_D("Error nvs_get_blob: %s", esp_err_to_name(err));
        if (ESP_OK == err)
        {
            TRACE_D("Load wifi config:: ssid: %s, password: %s", wifi_info, &wifi_info[32]);
        }
    }
    else
    {
        TRACE_E("'wifi config' read-lenght error!, Required: %d | %d, Error: %s", required_size, len, esp_err_to_name(err));
    }
}

int ezlopi_nvs_write_user_id_str(char *data)
{
    int ret = 0;

    esp_err_t err;
    nvs_handle_t config_nvs_handle;

    err = nvs_open(storage_name, NVS_READWRITE, &config_nvs_handle);
    TRACE_W("nvs_open - error: %s", esp_err_to_name(err));
    err = nvs_open(storage_name, NVS_READWRITE, &config_nvs_handle);
    TRACE_W("nvs_open - error: %s", esp_err_to_name(err));

    if (ESP_OK == err)
    {
        err = nvs_set_str(config_nvs_handle, user_id_nvs_name, data);
        TRACE_W("nvs_set_str - error: %s", esp_err_to_name(err));

        if (ESP_OK == err)
        {
            err = nvs_commit(config_nvs_handle);
            TRACE_W("nvs_commit - error: %s", esp_err_to_name(err));

            if (ESP_OK == err)
            {
                ret = 1;
            }
        }

        nvs_close(config_nvs_handle);
    }

    return ret;
}

int ezlopi_nvs_read_user_id_str(char **data)
{
    int ret = 0;
    esp_err_t err = ESP_OK;

    nvs_handle_t config_nvs_handle;
    err = nvs_open(storage_name, NVS_READWRITE, &config_nvs_handle);

    if (ESP_OK == err)
    {
        size_t buf_len_needed;
        err = nvs_get_str(config_nvs_handle, user_id_nvs_name, NULL, &buf_len_needed);

        if (buf_len_needed && (ESP_OK == err))
        {
            *data = malloc(buf_len_needed + 1);

            if (*data)
            {
                err = nvs_get_str(config_nvs_handle, user_id_nvs_name, *data, &buf_len_needed);
                data[buf_len_needed + 1] = '\0';
                TRACE_W("nvs_get_str(data) error: %s", esp_err_to_name(err));

                if (ESP_OK == err)
                {
                    ret = 1;
                }
                else
                {
                    free(*data);
                    *data = NULL;
                }
            }
        }

        nvs_close(config_nvs_handle);
    }

    return ret;
}

void ezlopi_nvs_deinit(void)
{
    nvs_close(ezlopi_nvs_handle);
    ezlopi_nvs_handle = 0;
}

void ezlopi_nvs_set_provisioning_time(uint32_t epoch_time)
{
    if (ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_set_u32(ezlopi_nvs_handle, provisioning_time_nvs_name, epoch_time);
        TRACE_W("nvs_set_u32 - error: %s", esp_err_to_name(err));
    }
}

uint32_t ezlopi_nvs_get_provisioning_time(void)
{
    uint32_t provisioning_time = 1;
    if (ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_get_u32(ezlopi_nvs_handle, provisioning_time_nvs_name, &provisioning_time);
        TRACE_I("Boot count: %d", provisioning_time);
        TRACE_D("Error nvs_get_blob: %s", esp_err_to_name(err));
        if (ESP_OK != err)
        {
            err = nvs_set_u32(ezlopi_nvs_handle, provisioning_time_nvs_name, provisioning_time);
            TRACE_W("nvs_set_u32 - error: %s", esp_err_to_name(err));
        }
    }

    return provisioning_time;
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
