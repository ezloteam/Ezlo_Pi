#ifndef __NVS_STORAGE_H__
#define __NVS_STORAGE_H__

#include "nvs_flash.h"

class nvs_storage
{
private:
    nvs_handle_t nvs_handle;
    void init(void);

protected:
    nvs_storage() {}
    static nvs_storage *nvs_storage_;

public:
    void deinit(void);
    static nvs_storage *get_instance(void);

    void read_wifi(char *wifi_info, uint32_t len);
    void write_wifi(const char *wifi_info, uint32_t len);

    void write_gpio_config(uint8_t *gpio_conf, uint32_t len);
    esp_err_t read_gpio_config(uint8_t *gpio_conf, uint32_t len);

    void write_device_config(void *buffer, uint32_t len);
    esp_err_t read_device_config(void *buffer, uint32_t len);

    nvs_storage(nvs_storage &other) = delete;
    void operator=(const nvs_storage &) = delete;
};

#endif // __NVS_STORAGE_H__