#include "string.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "debug.h"
#include "nvs_storage.h"
#include "interface_common.h"

static nvs_handle_t nvs_storage_handle;

void nvs_storage_init(void)
{
    esp_err_t err = nvs_flash_init();

    if (ESP_ERR_NVS_NO_FREE_PAGES == err || ESP_ERR_NVS_NEW_VERSION_FOUND == err)
    {
        TRACE_D("NVS Init Failed once!, Error: %s", esp_err_to_name(err));
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    err = nvs_open("storage", NVS_READWRITE, &nvs_storage_handle);
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

void nvs_storage_write_device_config(void *buffer, uint32_t len)
{
    esp_err_t err = nvs_set_blob(nvs_storage_handle, "devices_sw", buffer, len);
    TRACE_D("'device_config' Error nvs_set_blob: %s", esp_err_to_name(err));
    err = nvs_commit(nvs_storage_handle);
    TRACE_D("'device_config' Error nvs_commit: %s", esp_err_to_name(err));
}

void nvs_storage_write_gpio_config(uint8_t *gpio_conf, uint32_t len)
{
    esp_err_t err = nvs_set_blob(nvs_storage_handle, "gpio_cfg", gpio_conf, len);
    TRACE_D("'gpio_config' Error nvs_set_blob: %s", esp_err_to_name(err));
    err = nvs_commit(nvs_storage_handle);
    TRACE_D("'gpio_config' Error nvs_commit: %s", esp_err_to_name(err));
}

void nvs_storage_write_wifi(const char *wifi_info, uint32_t len)
{
    esp_err_t err = nvs_set_blob(nvs_storage_handle, "wifi_info", wifi_info, len);
    TRACE_D("'write_wifi' Error nvs_set_blob: %s", esp_err_to_name(err));
    err = nvs_commit(nvs_storage_handle);
    TRACE_D("'write_wifi' Error nvs_commit: %s", esp_err_to_name(err));
}

esp_err_t nvs_storage_read_device_config(void *buffer, uint32_t len)
{
    size_t required_size = 0;
    esp_err_t err = nvs_get_blob(nvs_storage_handle, "devices_sw", NULL, &required_size);
    if (len >= required_size)
    {
        err = nvs_get_blob(nvs_storage_handle, "devices_sw", buffer, &required_size);
        TRACE_D("'Device Config' Error nvs_get_blob: %s", esp_err_to_name(err));
    }
    else
    {
        TRACE_E("'Device Config' read-lenght error!");
    }
    return err;
}

esp_err_t nvs_storage_read_gpio_config(uint8_t *gpio_conf, uint32_t len)
{
    size_t req_len;
    esp_err_t err = nvs_get_blob(nvs_storage_handle, "gpio_cfg", NULL, &req_len);
    if (len >= req_len)
    {
        err = nvs_get_blob(nvs_storage_handle, "gpio_cfg", gpio_conf, &req_len);
        TRACE_D("'gpio_cfg': Error nvs_get_blob: %s", esp_err_to_name(err));
    }
    else
    {
        TRACE_E("'gpio_cfg' read-lenght error!");
    }
    return err;
}

void nvs_storage_read_wifi(char *wifi_info, uint32_t len)
{
    size_t required_size;
    esp_err_t err = nvs_get_blob(nvs_storage_handle, "wifi_info", NULL, &required_size);
    if (len >= required_size)
    {
        err = nvs_get_blob(nvs_storage_handle, "wifi_info", wifi_info, &required_size);
        TRACE_D("Error nvs_get_blob: %s", esp_err_to_name(err));
        if (ESP_OK == err)
        {
            TRACE_D("Load wifi config:: ssid: %s, password: %s", wifi_info, &wifi_info[32]);
        }
    }
    else
    {
        TRACE_E("'wifi config' read-lenght error!");
    }
}

void nvs_storage_deinit(void)
{
}
