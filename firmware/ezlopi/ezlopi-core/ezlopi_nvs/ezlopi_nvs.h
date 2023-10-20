#ifndef __NVS_STORAGE_H__
#define __NVS_STORAGE_H__

#include "nvs_flash.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define NVS_WIFI_MAGIC 0x5647ABCD

    int ezlopi_nvs_init(void);
    void ezlopi_nvs_deinit(void);

    int ezlopi_nvs_factory_reset(void);

    char *ezlopi_nvs_read_str(char *nvs_name);
    int ezlopi_nvs_write_str(char *data, uint32_t len, char *nvs_name);

    // void ezlopi_nvs_delete_stored_data(char *nvs_name);

    int ezlopi_nvs_write_config_data_str(char *data);
    char *ezlopi_nvs_read_config_data_str(void);

    int ezlopi_nvs_read_wifi(char *wifi_info, uint32_t len);
    int ezlopi_nvs_write_wifi(const char *wifi_info, uint32_t len);

    int ezlopi_nvs_read_ble_passkey(uint32_t *passkey);
    int ezlopi_nvs_write_ble_passkey(uint32_t passkey);

    char *ezlopi_nvs_read_user_id_str(void);
    int ezlopi_nvs_write_user_id_str(char *data);

    uint32_t ezlopi_nvs_get_boot_count(void);
    void ezlopi_nvs_set_boot_count(uint32_t boot_count);

    uint32_t ezlopi_nvs_get_provisioning_status(void);
    void ezlopi_nvs_set_provisioning_status(void);

    char *ezlopi_nvs_scene_get(void);
    int ezlopi_nvs_scene_set(char *scene);

    char *ezlopi_nvs_scene_get_v2(void);
    int ezlopi_nvs_scene_set_v2(char *scene);

    char *ezlopi_nvs_read_scenes_scripts(void);
    int ezlopi_nvs_write_scenes_scripts(char *data);
    void ezlopi_nvs_delete_stored_script(uint32_t script_id);

#ifdef __cplusplus
}
#endif

#endif // __NVS_STORAGE_H__