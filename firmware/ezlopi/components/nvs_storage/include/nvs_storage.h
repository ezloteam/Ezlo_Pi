#ifndef __NVS_STORAGE_H__
#define __NVS_STORAGE_H__

#include "nvs_flash.h"

#ifdef __cplusplus
extern "C"
{
#endif

    void nvs_storage_init(void);
    void nvs_storaget_deinit(void);

    void nvs_storage_read_wifi(char *wifi_info, uint32_t len);
    void nvs_storage_write_wifi(const char *wifi_info, uint32_t len);

    void nvs_storage_write_gpio_config(uint8_t *gpio_conf, uint32_t len);
    esp_err_t nvs_storage_read_gpio_config(uint8_t *gpio_conf, uint32_t len);

    void nvs_storage_write_device_config(void *buffer, uint32_t len);
    esp_err_t nvs_storage_read_device_config(void *buffer, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif // __NVS_STORAGE_H__