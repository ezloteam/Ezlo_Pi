#ifndef _EZLOPI_CORE_NVS_H_
#define _EZLOPI_CORE_NVS_H_

#include "nvs_flash.h"
#include "ezlopi_core_errors.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define NVS_WIFI_MAGIC 0x5647ABCD

    ezlopi_error_t ezlopi_nvs_init(void);
    void ezlopi_nvs_deinit(void);

    uint32_t ezlopi_nvs_config_info_update_time_get(void);
    uint32_t ezlopi_nvs_config_info_version_number_get(void);

    void ezlopi_nvs_config_info_update_time_set(uint32_t value);
    void ezlopi_nvs_config_info_version_number_set(uint32_t value);

    int ezlopi_nvs_factory_reset(void);

    char* ezlopi_nvs_read_str(const char* nvs_name);
    int ezlopi_nvs_write_str(const char* data, uint32_t len, const char* nvs_name);

    // void ezlopi_nvs_delete_stored_data(char *nvs_name);

    int ezlopi_nvs_write_config_data_str(char* data);
    char* ezlopi_nvs_read_config_data_str(void);

    int ezlopi_nvs_read_wifi(char* wifi_info, uint32_t len);
    int ezlopi_nvs_write_wifi(const char* wifi_info, uint32_t len);

    int ezlopi_nvs_read_ble_passkey(uint32_t* passkey);
    int ezlopi_nvs_write_ble_passkey(uint32_t passkey);

    char* ezlopi_nvs_read_user_id_str(void);
    int ezlopi_nvs_write_user_id_str(char* data);

    uint32_t ezlopi_nvs_get_boot_count(void);
    void ezlopi_nvs_set_boot_count(uint32_t boot_count);

    void ezlopi_nvs_scenes_soft_reset(void);
    void ezlopi_nvs_scenes_factory_info_reset(void);

#if 0
    int ezlopi_settings_save_settings(const s_ezlopi_hub_settings_t* settings_list, uint16_t num_settings);
    int ezlopi_settings_retrive_settings(s_ezlopi_hub_settings_t* ezlopi_settings_list, uint16_t num_settings);
#endif

    uint8_t ezlopi_nvs_write_int32(int32_t i, const char* key_name);
    uint8_t ezlopi_nvs_read_int32(int32_t* i, const char* key_name);
    uint8_t ezlopi_nvs_write_uint32(uint32_t i, const char* key_name);
    uint8_t ezlopi_nvs_read_uint32(uint32_t* i, const char* key_name);
    uint8_t ezlopi_nvs_write_bool(bool b, const char* key_name);
    uint8_t ezlopi_nvs_read_bool(bool* b, const char* key_name);
    uint8_t ezlopi_nvs_write_float32(float f, const char* key_name);
    uint8_t ezlopi_nvs_read_float32(float* f, const char* key_name);

    char* ezlopi_nvs_scene_get_v2(void);
    int ezlopi_nvs_scene_set_v2(char* scene);

    char* ezlopi_nvs_scene_get(void);
    int ezlopi_nvs_scene_set(char* scene);

    char* ezlopi_nvs_scene_get_v2(void);
    int ezlopi_nvs_scene_set_v2(char* scene);

    char* ezlopi_nvs_read_scenes_scripts(void);
    int ezlopi_nvs_write_scenes_scripts(char* data);
    void ezlopi_nvs_delete_stored_data_by_name(char* nvs_name);
    void ezlopi_nvs_delete_stored_data_by_id(uint32_t script_id);

    char* ezlopi_nvs_read_scenes_expressions(void);
    int ezlopi_nvs_write_scenes_expressions(char* data);

    char* ezlopi_nvs_read_rooms(void);
    int ezlopi_nvs_write_rooms(char* data);

    int EZPI_CORE_nvs_write_time_location(const char* time_loc, uint32_t len);
    char* EZPI_CORE_nvs_read_time_location(void);

    char* ezlopi_nvs_read_modes(void);
    int ezlopi_nvs_write_modes(char* data);

    int EZPI_CORE_nvs_write_time_location(const char* time_loc, uint32_t len);
    char* EZPI_CORE_nvs_read_time_location(void);

    char* ezlopi_nvs_read_modes(void);
    int ezlopi_nvs_write_modes(char* data);

    char* ezlopi_nvs_read_latidtude_longitude();
    int ezlopi_nvs_write_latitude_longitude(char* data);

    bool EZPI_CORE_nvs_write_baud(uint32_t baud);
    uint8_t EZPI_CORE_nvs_read_baud(uint32_t* baud);

    bool EZPI_CORE_nvs_write_parity(uint32_t parity);
    uint8_t EZPI_CORE_nvs_read_parity(uint32_t* parity);

    bool EZPI_CORE_nvs_write_start_bits(uint32_t start_bits);
    uint8_t EZPI_CORE_nvs_read_start_bits(uint32_t* start_bits);

    bool EZPI_CORE_nvs_write_stop_bits(uint32_t stop_bits);
    uint8_t EZPI_CORE_nvs_read_stop_bits(uint32_t* stop_bits);

    bool EZPI_CORE_nvs_write_frame_size(uint32_t frame_size);
    uint8_t EZPI_CORE_nvs_read_frame_size(uint32_t* frame_size);

    bool EZPI_CORE_nvs_write_flow_control(uint32_t flow_control);
    uint8_t EZPI_CORE_nvs_read_flow_control(uint32_t* flow_control);

    bool EZPI_CORE_nvs_write_cloud_log_severity(uint32_t severity);
    uint8_t EZPI_CORE_nvs_read_cloud_log_severity(uint32_t* severity);

    bool EZPI_CORE_nvs_write_serial_log_severity(uint32_t severity);
    uint8_t EZPI_CORE_nvs_read_serial_log_severity(uint32_t* severity);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_NVS_H_
