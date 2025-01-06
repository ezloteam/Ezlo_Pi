/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/
/**
 * @file    ezlopi_core_nvs.h
 * @brief   Function to perform that nvs-operation
 * @author  xx
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef _EZLOPI_CORE_NVS_H_
#define _EZLOPI_CORE_NVS_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "nvs_flash.h"
#include "ezlopi_core_errors.h"

/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/
#define NVS_WIFI_MAGIC 0x5647ABCD

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

    /**
     * @brief Function to initialize nvs service
     *
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_nvs_init(void);
    /**
     * @brief Function to de-initialize nvs service
     *
     */
    void EZPI_nvs_deinit(void);
    /**
     * @brief Funtion to get latest-update time
     *
     * @return uint32_t
     */
    uint32_t EZPI_core_nvs_config_info_update_time_get(void);
    /**
     * @brief Function to get version number
     *
     * @return uint32_t
     */
    uint32_t EZPI_core_nvs_config_info_version_number_get(void);
    /**
     * @brief Function to update time set
     *
     * @param value Value of time to be set
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_config_info_update_time_set(uint32_t value);
    /**
     * @brief Function to set version number
     *
     * @param value Version value to set
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_config_info_version_number_set(uint32_t value);
    /**
     * @brief Function to reset nvs factory info
     *
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_factory_reset(void);
    /**
     * @brief Function to read str
     *
     * @param nvs_name Str name
     * @return char*
     */
    char *EZPI_core_nvs_read_str(const char *nvs_name);
    /**
     * @brief Function to write str into nvs
     *
     * @param data Pointer to start of data
     * @param len Length of data to be stored
     * @param nvs_name NVS key name
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_str(const char *data, uint32_t len, const char *nvs_name);

    // void ezlopi_nvs_delete_stored_data(char *nvs_name);

    /**
     * @brief Function to write config data into nvs
     *
     * @param data src data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_config_data_str(char *data);
    /**
     * @brief Function to read config data from nvs from nvs
     *
     * @return char*
     */
    char *EZPI_core_nvs_read_config_data_str(void);
    /**
     * @brief Function to read wifi data from nvs
     *
     * @param wifi_info Wifi info to read
     * @param len length of cred
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_wifi(char *wifi_info, uint32_t len);
    /**
     * @brief Function to write wifi data to nvs
     *
     * @param wifi_info Wifi info to write
     * @param len length of cred
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_wifi(const char *wifi_info, uint32_t len);
    /**
     * @brief Function to read ble info from nvs
     *
     * @param passkey Target ble-passkey
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_ble_passkey(uint32_t *passkey);
    /**
     * @brief Function to write ble info into nvs
     *
     * @param passkey Target ble-passkey
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_ble_passkey(uint32_t passkey);
    /**
     * @brief Function to read user_id str
     *
     * @return char*
     */
    char *EZPI_core_nvs_read_user_id_str(void);
    /**
     * @brief Function to write user_ID to nvs
     *
     * @param data Src data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_user_id_str(char *data);
    /**
     * @brief Function to get boot count
     *
     * @return uint32_t
     */
    uint32_t EZPI_core_nvs_get_boot_count(void);
    /**
     * @brief Function to set boot count
     *
     * @param boot_count Count to set
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_set_boot_count(uint32_t boot_count);
    /**
     * @brief Function core nvs scenes soft reset
     *
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_scenes_soft_reset(void);
    /**
     * @brief Function to reset factory info
     *
     */
    void EZPI_core_nvs_scenes_factory_info_reset(void);

#if 0
    int ezlopi_settings_save_settings(const s_ezlopi_hub_settings_t *settings_list, uint16_t num_settings);
    int ezlopi_settings_retrive_settings(s_ezlopi_hub_settings_t *ezlopi_settings_list, uint16_t num_settings);
#endif

    /**
     * @brief Function to write nvs int32 data
     *
     * @param i int32_t value to write
     * @param key_name key-name
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_int32(int32_t i, const char *key_name);
    /**
     * @brief Function to read nvs int32 data
     *
     * @param i Destination address to store the extracted value
     * @param key_name key-name
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_int32(int32_t *i, const char *key_name);
    /**
     * @brief Function to write nvs uint32 data
     *
     * @param i int32_t value to write
     * @param key_name key-name
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_uint32(uint32_t i, const char *key_name);
    /**
     * @brief Function to read nvs uint32 data
     *
     * @param i Destination address to store the extracted value
     * @param key_name key-name
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_uint32(uint32_t *i, const char *key_name);
    /**
     * @brief Funtion to write nvs bool data
     *
     * @param b bool value to write
     * @param key_name key-name
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_bool(bool b, const char *key_name);
    /**
     * @brief Function to read bool  from nvs
     *
     * @param b Address to store extracted bool data
     * @param key_name key-name
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_bool(bool *b, const char *key_name);
    /**
     * @brief Function to write float data into nvs
     *
     * @param f Float value to store
     * @param key_name key-name
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_float32(float f, const char *key_name);
    /**
     * @brief Function to read float data from
     *
     * @param f Address to store extracted float data
     * @param key_name key_name
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_float32(float *f, const char *key_name);
    /**
     * @brief Function to read scenes from nvs
     *
     * @return char*
     */
    char *EZPI_core_nvs_scene_get(void);
    /**
     * @brief Function to read nvs scene set
     *
     * @param scene
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_scene_set(char *scene);
    /**
     * @brief Function to get string version of scene
     *
     * @return char*
     */
    char *EZPI_core_nvs_scene_get_v2(void);
    /**
     * @brief Function to set scene in nvs
     *
     * @param scene Pointer to scene
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_scene_set_v2(char *scene);
    /**
     * @brief Function to read scenes scripts
     *
     * @return char*
     */
    char *EZPI_core_nvs_read_scenes_scripts(void);
    /**
     * @brief Function to write scenes scritps
     *
     * @param data Target data to be written
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_scenes_scripts(char *data);
    /**
     * @brief Function to delete nvs data by name
     *
     * @param nvs_name Target name
     */
    void EZPI_core_nvs_delete_stored_data_by_name(char *nvs_name);
    /**
     * @brief Function to delete nvs data by id
     *
     * @param script_id Target script_id
     */
    void EZPI_core_nvs_delete_stored_data_by_id(uint32_t script_id);
    /**
     * @brief Function to read scenes_expression
     *
     * @return char*
     */
    char *EZPI_core_nvs_read_scenes_expressions(void);
    /**
     * @brief Function to write
     *
     * @param data Pointer to src data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_scenes_expressions(char *data);
    /**
     * @brief Function to read device_groups
     *
     * @return char*
     */
    char *EZPI_core_nvs_read_device_groups(void);
    /**
     * @brief Function to write device_groups
     *
     * @param data Pointer to src data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_device_groups(char *data);
    /**
     * @brief Function to read item_groups
     *
     * @return char*
     */
    char *EZPI_core_nvs_read_item_groups(void);
    /**
     * @brief Function to write item_groups
     *
     * @param data Pointer to src data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_item_groups(char *data);
    /**
     * @brief Function to read rooms
     *
     * @return char*
     */
    char *EZPI_core_nvs_read_rooms(void);
    /**
     * @brief Function to write rooms
     *
     * @param data Pointer to src data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_rooms(char *data);
    /**
     * @brief Function to write time_location
     *
     * @param time_loc Pointer time & location data
     * @param len Length of the data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_time_location(const char *time_loc, uint32_t len);
    /**
     * @brief Function to read time_location
     *
     * @return char*
     */
    char *EZPI_core_nvs_read_time_location(void);
    /**
     * @brief Function to read modes
     *
     * @return char*
     */
    char *EZPI_core_nvs_read_modes(void);
    /**
     * @brief Function to write modes
     *
     * @param data Pointer to src data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_modes(char *data);
    /**
     * @brief Function to read latidtude_longitude
     *
     * @return char*
     */
    char *EZPI_core_nvs_read_latidtude_longitude();
    /**
     * @brief Function to write latitude_longitude
     *
     * @param data Pointer to src data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_latitude_longitude(char *data);

    /**
     * @brief Function to write 'baud' to nvs
     *
     * @param baud Baudrate
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_baud(uint32_t baud);
    /**
     * @brief Function to read from nvs
     *
     * @param baud Mem-Address to store extracted baudrate
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_baud(uint32_t *baud);

    /**
     * @brief Function to write 'parity' to nvs
     *
     * @param parity parity-type value
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_parity(uint32_t parity);
    /**
     * @brief Function to read 'parity' from nvs
     *
     * @param parity Mem-Address to store extracted 'parity-type' data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_parity(uint32_t *parity);

    /**
     * @brief Function to write 'start_bits' to nvs
     *
     * @param start_bits start_bits val
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_start_bits(uint32_t start_bits);
    /**
     * @brief Function to read 'start_bits' from nvs
     *
     * @param start_bits Mem-Address to store extracted 'start_bits' data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_start_bits(uint32_t *start_bits);

    /**
     * @brief Function to write 'stop_bits' to nvs
     *
     * @param stop_bits stop_bits value
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_stop_bits(uint32_t stop_bits);
    /**
     * @brief Function to read 'stop_bits' from nvs
     *
     * @param stop_bits Mem-Address to store extracted 'stop_bits' data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_stop_bits(uint32_t *stop_bits);

    /**
     * @brief Function to write 'frame_size' to nvs
     *
     * @param frame_size frame_size value
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_frame_size(uint32_t frame_size);
    /**
     * @brief Function to read 'frame_size' from nvs
     *
     * @param frame_size Mem-Address to store extracted 'frame_size' data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_frame_size(uint32_t *frame_size);

    /**
     * @brief Function to write 'flow_control' to nvs
     *
     * @param flow_control flow_control value
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_flow_control(uint32_t flow_control);
    /**
     * @brief Function to read 'flow_control' from nvs
     *
     * @param flow_control Mem-Address to store extracted 'flow_control' data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_flow_control(uint32_t *flow_control);

    /**
     * @brief Function to write 'cloud_log_severity' to nvs
     *
     * @param severity severity lvl
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_cloud_log_severity(uint32_t severity);
    /**
     * @brief Function to read 'cloud_log_severity' from nvs
     *
     * @param severity Mem-Address to store extracted 'cloud_log_severity' data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_cloud_log_severity(uint32_t *severity);

    /**
     * @brief Function to write 'serial_log_severity' to nvs
     *
     * @param severity severity lvl
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_serial_log_severity(uint32_t severity);
    /**
     * @brief Function to read 'serial_log_severity' from nvs
     *
     * @param severity Mem-Address to store extracted 'serial_log_severity' data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_serial_log_severity(uint32_t *severity);

    /**
     * @brief Function to write 'temperature_scale' to nvs
     *
     * @param scale temp scale type
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_temperature_scale(uint32_t scale);
    /**
     * @brief Function to read 'temperature_scale' from nvs
     *
     * @param scale Mem-Address to store extracted 'temperature_scale' data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_temperature_scale(uint32_t *scale);

    /**
     * @brief Function to write 'date_format' to nvs
     *
     * @param format date - format
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_date_format(uint32_t format);
    /**
     * @brief Function to read 'date_format' from nvs
     *
     * @param format Mem-Address to store extracted 'date_format' data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_date_format(uint32_t *format);

    /**
     * @brief Function to write 'time_format' to nvs
     *
     * @param format time - format
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_time_format(uint32_t format);
    /**
     * @brief Function to read 'time_format' from nvs
     *
     * @param format Mem-Address to store extracted 'time_format' data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_time_format(uint32_t *format);

    /**
     * @brief Function to write 'network_ping_timeout' to nvs
     *
     * @param ping_timeout ping_timeout
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_write_network_ping_timeout(uint32_t ping_timeout);
    /**
     * @brief Function to read 'network_ping_timeout' from nvs
     *
     * @param ping_timeout Mem-Address to store extracted 'network_ping_timeout' data
     * @return ezlopi_error_t
     */
    ezlopi_error_t EZPI_core_nvs_read_network_ping_timeout(uint32_t *ping_timeout);

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_NVS_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
