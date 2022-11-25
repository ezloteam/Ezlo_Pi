#ifndef __NVS_STORAGE_H__
#define __NVS_STORAGE_H__

#include "nvs_flash.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define NVS_WIFI_MAGIC 0x5647ABCD

    void ezlopi_nvs_init(void);
    void ezlopi_nvs_deinit(void);

    int ezlopi_nvs_write_config_data_str(char *data);
    int ezlopi_nvs_read_config_data_str(char **data);

    void ezlopi_nvs_read_wifi(char *wifi_info, uint32_t len);
    void ezlopi_nvs_write_wifi(const char *wifi_info, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif // __NVS_STORAGE_H__