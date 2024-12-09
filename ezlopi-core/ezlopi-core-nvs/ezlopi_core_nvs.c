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
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <string.h>
#include <time.h>

#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_nvs.h"
#include "EZLOPI_USER_CONFIG.h"

/*******************************************************************************
 *                          Extern Data Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Declarations
 *******************************************************************************/

/*******************************************************************************
 *                          Type & Macro Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Static Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/
static nvs_handle_t ezlopi_nvs_handle = 0;
static const char* storage_name = "storage";
static const char* config_nvs_name = "config_data";
static const char* passkey_nvs_name = "passkey";
static const char* user_id_nvs_name = "user_id";
static const char* wifi_info_nvs_name = "wifi_info";
static const char* boot_count_nvs_name = "boot_count";
static const char* ezlopi_scenes_nvs_name = "ezlopi_scenes";
static const char* ezlopi_scenes_v2_nvs_name = "ez_scenes_v2";
static const char* ezlopi_scripts_nvs_ids = "ezlopi_scripts";
static const char* config_info_update_time_name = "config_time";
static const char* config_info_version_number = "conf_ver_no";
static const char* ezlopi_scenes_expression_ids = "ezlopi_exp";
static const char* ezlopi_device_groups_ids = "dev_grp_list";
static const char* ezlopi_item_groups_ids = "item_grp_list";
static const char* ezlopi_room_ids_nvs_name = "ezlopi_room";
static const char* ezlopi_time_location_nvs_name = "time.local";
static const char* ezlopi_modes_nvs_name = "ezlopi_modes";
static const char* ezlopi_coordinates_nvs_name = "coord_vals";
static const char* ezlopi_serial_baud_name = "ezpi_baud";
static const char* ezlopi_serial_parity = "ezpi_prty";
static const char* ezlopi_serial_start_bits = "ezpi_strt_bt";
static const char* ezlopi_serial_stop_bits = "ezpi_stp_bt";
static const char* ezlopi_serial_frame_size = "ezpi_frm_sz";
static const char* ezlopi_serial_flow_control = "ezpi_fl_ctrl";
static const char* ezlopi_cloud_log_severity = "cld_log_svrt";
static const char* ezlopi_serial_log_severity = "srl_log_svrt";
static const char* ezlopi_temp_scale = "tmp_scale";
static const char* ezlopi_date_format = "date_fmt";
static const char* ezlopi_time_format = "time_fmt";
static const char* ezlopi_network_ping_timeout = "png_tm_out";

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/

/**
 * @brief Global/extern function template example
 * Convention : Use capital letter for initial word on extern function
 * @param arg
 */
ezlopi_error_t ezlopi_nvs_init(void)
{
    ezlopi_error_t error = EZPI_SUCCESS;
    if (0 == ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_flash_init();

        if (ESP_ERR_NVS_NO_FREE_PAGES == err || ESP_ERR_NVS_NEW_VERSION_FOUND == err)
        {
            TRACE_D("```nvs_flash_init``` Failed once!, Error: %s", esp_err_to_name(err));
            ESP_ERROR_CHECK(nvs_flash_erase());
            err = nvs_flash_init();
        }

        err = nvs_open(storage_name, NVS_READWRITE, &ezlopi_nvs_handle);
        if (ESP_OK != err)
        {
            TRACE_E("```nvs_open``` Error!");
            error = EZPI_ERR_NVS_INIT_FAILED;

            // vTaskDelay(2000 / portTICK_RATE_MS);
        }
        else
        {
            TRACE_D("```nvs_open``` success");
        }
    }
    return error;
}

uint32_t ezlopi_nvs_config_info_update_time_get(void)
{
    uint32_t ret = 0;
    if (EZPI_SUCCESS == ezlopi_nvs_init())
    {
        esp_err_t err = nvs_get_u32(ezlopi_nvs_handle, config_info_update_time_name, &ret);
        TRACE_S("config-update-time-get: %d", ret);
        if (ESP_OK != err)
        {
            ret = 0;
            TRACE_W("(nvs_get_u32)-%s:: error: %s", config_info_update_time_name, esp_err_to_name(err));
        }
    }
    return ret;
}

uint32_t ezlopi_nvs_config_info_version_number_get(void)
{
    uint32_t ret = 0;

    if (EZPI_SUCCESS == ezlopi_nvs_init())
    {
        esp_err_t err = nvs_get_u32(ezlopi_nvs_handle, config_info_version_number, &ret);
        TRACE_S("config-version-number-get: %d", ret);

        if (ESP_OK != err)
        {
            ret = 0;
            TRACE_W("(nvs_get_u32)-%s:: error: %s", config_info_version_number, esp_err_to_name(err));
        }
    }

    return ret;
}

ezlopi_error_t ezlopi_nvs_config_info_update_time_set(uint32_t value)
{
    ezlopi_error_t error = EZPI_FAILED;
    if (EZPI_SUCCESS == ezlopi_nvs_init())
    {
        nvs_erase_key(ezlopi_nvs_handle, config_info_update_time_name);
        esp_err_t err = nvs_set_u32(ezlopi_nvs_handle, config_info_update_time_name, value);
        TRACE_W("(nvs_set_u32)-%s:: error: %s", config_info_update_time_name, esp_err_to_name(err));
        error = (ESP_OK == err) ? EZPI_SUCCESS : error;
    }
    return error;
}

ezlopi_error_t ezlopi_nvs_config_info_version_number_set(uint32_t value)
{
    ezlopi_error_t error = EZPI_FAILED;
    if (EZPI_SUCCESS == ezlopi_nvs_init())
    {
        esp_err_t err = nvs_set_u32(ezlopi_nvs_handle, config_info_version_number, value);
        TRACE_W("(nvs_set_u32)-%s:: - error: %s", config_info_version_number, esp_err_to_name(err));
        error = (ESP_OK == err) ? EZPI_SUCCESS : error;
    }
    return error;
}

ezlopi_error_t ezlopi_nvs_scene_set_v2(char* scene)
{
    return ezlopi_nvs_write_str(scene, strlen(scene) + 1, ezlopi_scenes_v2_nvs_name);
}

char *ezlopi_nvs_scene_get_v2(void)
{
    return ezlopi_nvs_read_str(ezlopi_scenes_v2_nvs_name);
}

ezlopi_error_t ezlopi_nvs_scene_set(char* scene)
{
    return ezlopi_nvs_write_str(scene, strlen(scene) + 1, ezlopi_scenes_nvs_name);
}

char *ezlopi_nvs_scene_get(void)
{
    return ezlopi_nvs_read_str(ezlopi_scenes_nvs_name);
}

ezlopi_error_t ezlopi_nvs_factory_reset(void)
{
    ezlopi_error_t error = EZPI_SUCCESS;
    if (ESP_OK == nvs_flash_erase())
    {
        error = EZPI_SUCCESS;
    }

    return error;
}

ezlopi_error_t ezlopi_nvs_write_scenes_scripts(char* data)
{
    return ezlopi_nvs_write_str(data, strlen(data), (char *)ezlopi_scripts_nvs_ids);
}

char *ezlopi_nvs_read_scenes_scripts(void)
{
    return ezlopi_nvs_read_str(ezlopi_scripts_nvs_ids);
}

ezlopi_error_t ezlopi_nvs_write_scenes_expressions(char* data)
{
    return ezlopi_nvs_write_str(data, strlen(data), ezlopi_scenes_expression_ids);
}

char *ezlopi_nvs_read_scenes_expressions(void)
{
    return ezlopi_nvs_read_str(ezlopi_scenes_expression_ids);
}

ezlopi_error_t ezlopi_nvs_write_device_groups(char *data)
{
    return ezlopi_nvs_write_str(data, strlen(data) + 1, ezlopi_device_groups_ids);
}

char *ezlopi_nvs_read_device_groups(void)
{
    return ezlopi_nvs_read_str(ezlopi_device_groups_ids);
}

ezlopi_error_t ezlopi_nvs_write_item_groups(char *data)
{
    return ezlopi_nvs_write_str(data, strlen(data) + 1, ezlopi_item_groups_ids);
}

char *ezlopi_nvs_read_item_groups(void)
{
    return ezlopi_nvs_read_str(ezlopi_item_groups_ids);
}

char *ezlopi_nvs_read_rooms(void)
{
    return ezlopi_nvs_read_str(ezlopi_room_ids_nvs_name);
}

ezlopi_error_t ezlopi_nvs_write_rooms(char* data)
{
    return ezlopi_nvs_write_str(data, strlen(data), ezlopi_room_ids_nvs_name);
}

char *ezlopi_nvs_read_modes(void)
{
    return ezlopi_nvs_read_str(ezlopi_modes_nvs_name);
}

ezlopi_error_t ezlopi_nvs_write_modes(char* data)
{
    return ezlopi_nvs_write_str(data, strlen(data), ezlopi_modes_nvs_name);
}

ezlopi_error_t ezlopi_nvs_write_config_data_str(char* data)
{
    return ezlopi_nvs_write_str(data, strlen(data), config_nvs_name);
}

char *ezlopi_nvs_read_config_data_str(void)
{
    return ezlopi_nvs_read_str(config_nvs_name);
}

ezlopi_error_t ezlopi_nvs_read_ble_passkey(uint32_t* passkey)
{
    const uint32_t default_passkey = 123456;
    ezlopi_error_t error = EZPI_ERR_NVS_READ_FAILED;
    if (passkey)
    {
        if (EZPI_SUCCESS == ezlopi_nvs_init())
        {
            esp_err_t err = ESP_OK;
            err = nvs_get_u32(ezlopi_nvs_handle, passkey_nvs_name, passkey);
            TRACE_W("(nvs_get_u32)-%s:: error: %s", passkey_nvs_name, esp_err_to_name(err));

            if (ESP_OK == err)
            {
                error = EZPI_SUCCESS;
            }
            else
            {
                *passkey = 0;
            }
        }

        *passkey = ((0 == *passkey) || (*passkey > 999999)) ? default_passkey : *passkey;
    }

    return error;
}

ezlopi_error_t ezlopi_nvs_write_ble_passkey(uint32_t passkey)
{
    ezlopi_error_t error = EZPI_ERR_NVS_WRITE_FAILED;

    if (EZPI_SUCCESS == ezlopi_nvs_init())
    {
        esp_err_t err = ESP_OK;
        err = nvs_set_u32(ezlopi_nvs_handle, passkey_nvs_name, passkey);
        TRACE_W("(nvs_set_u32)-passkey_nvs_name:: error: %s", esp_err_to_name(err));

        if (ESP_OK == err)
        {
            error = EZPI_SUCCESS;
        }
    }

    return error;
}

ezlopi_error_t ezlopi_nvs_write_wifi(const char* wifi_info, uint32_t len)
{
    return ezlopi_nvs_write_str(wifi_info, len, wifi_info_nvs_name);
}

ezlopi_error_t ezlopi_nvs_read_wifi(char* wifi_info, uint32_t len)
{
    ezlopi_error_t error = EZPI_ERR_NVS_READ_FAILED;
    if (EZPI_SUCCESS == ezlopi_nvs_init())
    {
        size_t required_size;
        esp_err_t err = nvs_get_blob(ezlopi_nvs_handle, wifi_info_nvs_name, NULL, &required_size);
        if ((ESP_OK == err) && (len >= required_size))
        {
            err = nvs_get_blob(ezlopi_nvs_handle, wifi_info_nvs_name, wifi_info, &required_size);
            TRACE_W("(nvs_get_blob)-%s:: Error: %s", wifi_info_nvs_name, esp_err_to_name(err));

            if (ESP_OK == err)
            {
                error = EZPI_SUCCESS;
                TRACE_D("Load wifi config:: ssid: %s, password: %s", wifi_info, &wifi_info[32]);
            }
        }
        else
        {
            TRACE_E("'wifi config' read-lenght error!, Required: %d | %d, Error: %s", required_size, len, esp_err_to_name(err));
        }
    }
    return error;
}

ezlopi_error_t ezlopi_nvs_write_user_id_str(char* data)
{
    ezlopi_error_t error = EZPI_ERR_NVS_WRITE_FAILED;
    if (EZPI_SUCCESS == ezlopi_nvs_init())
    {
        esp_err_t err;
        err = nvs_set_str(ezlopi_nvs_handle, user_id_nvs_name, data);
        TRACE_W("ezlopi_nvs_write_user_id_str - error: %s", esp_err_to_name(err));

        if (ESP_OK == err)
        {
            err = nvs_commit(ezlopi_nvs_handle);

            if (ESP_OK == err)
            {
                error = EZPI_SUCCESS;
            }
            else
            {
                TRACE_E("ezlopi_nvs_write_user_id_str - error: %s", esp_err_to_name(err));
            }
        }
    }

    return error;
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

ezlopi_error_t ezlopi_nvs_set_boot_count(uint32_t boot_count)
{
    ezlopi_error_t error = EZPI_ERR_NVS_WRITE_FAILED;
    if (EZPI_SUCCESS == ezlopi_nvs_init())
    {
        esp_err_t err = nvs_set_u32(ezlopi_nvs_handle, boot_count_nvs_name, boot_count);
        TRACE_W("nvs_set_u32 - error: %s", esp_err_to_name(err));
        error = (ESP_OK == err) ? EZPI_SUCCESS : error;
    }
    return error;
}

uint32_t ezlopi_nvs_get_boot_count(void)
{
    uint32_t boot_count = 1;
    if (EZPI_SUCCESS == ezlopi_nvs_init())
    {
        esp_err_t err = nvs_get_u32(ezlopi_nvs_handle, boot_count_nvs_name, &boot_count);
        TRACE_S("Boot count: %d", boot_count);
        TRACE_D("Error nvs_get_blob: %s", esp_err_to_name(err));
        if (ESP_OK != err)
        {
            err = nvs_set_u32(ezlopi_nvs_handle, boot_count_nvs_name, boot_count);
            TRACE_W("nvs_set_u32 - error: %s", esp_err_to_name(err));
        }
    }

    return boot_count;
}

ezlopi_error_t ezlopi_nvs_scenes_soft_reset(void)
{
    ezlopi_error_t error = EZPI_SUCCESS;
    esp_err_t err = ESP_OK;
    if (EZPI_SUCCESS == ezlopi_nvs_init())
    {
        // only 'wifi' key is cleared during 'soft reset'
        if (ESP_OK != (err = nvs_erase_key(ezlopi_nvs_handle, wifi_info_nvs_name)))
        {
            TRACE_E("Erasing nvs-key '%s' failed!, error: %s", wifi_info_nvs_name, esp_err_to_name(err));
            error = EZPI_FAILED;
        }
    }
    return error;
}

void ezlopi_nvs_scenes_factory_info_reset(void)
{
    esp_err_t err = ESP_OK;
    if (EZPI_SUCCESS == ezlopi_nvs_init())
    {
        // list of nvs keys cleared during 'factory reset'
        if (ESP_OK != (err = nvs_erase_key(ezlopi_nvs_handle, user_id_nvs_name))) // ble
        {
            TRACE_E("Erasing nvs-key '%s' failed!, error: %s", user_id_nvs_name, esp_err_to_name(err));
        }
        if (ESP_OK != (err = nvs_erase_key(ezlopi_nvs_handle, passkey_nvs_name))) // ble
        {
            TRACE_E("Erasing nvs-key '%s' failed!, error: %s", passkey_nvs_name, esp_err_to_name(err));
        }
        if (ESP_OK != (err = nvs_erase_key(ezlopi_nvs_handle, ezlopi_scenes_v2_nvs_name)))
        {
            TRACE_E("Erasing nvs-key '%s' failed!, error: %s", ezlopi_scenes_v2_nvs_name, esp_err_to_name(err));
        }
        if (ESP_OK != (err = nvs_erase_key(ezlopi_nvs_handle, ezlopi_scripts_nvs_ids)))
        {
            TRACE_E("Erasing nvs-key '%s' failed!, error: %s", ezlopi_scripts_nvs_ids, esp_err_to_name(err));
        }
        if (ESP_OK != (err = nvs_erase_key(ezlopi_nvs_handle, ezlopi_scenes_expression_ids)))
        {
            TRACE_E("Erasing nvs-key '%s' failed!, error: %s", ezlopi_scenes_expression_ids, esp_err_to_name(err));
        }
        // if (ESP_OK != (err = nvs_erase_key(ezlopi_nvs_handle, settings_initialized_status_name)))                // "settings_magic"
        // {
        //     TRACE_E("Erasing nvs-key '%s' failed!, error: %s", settings_initialized_status_name, esp_err_to_name(err));
        // }
    }
    ezlopi_nvs_scenes_soft_reset();
}

ezlopi_error_t ezlopi_nvs_write_int32(int32_t i, const char* key_name)
{
    ezlopi_error_t error = EZPI_ERR_NVS_WRITE_FAILED;
    if (ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_set_i32(ezlopi_nvs_handle, key_name, i);
        if (ESP_OK != err)
        {
            TRACE_W("(nvs_set_i32)-%s:: error: %s", key_name, esp_err_to_name(err));
        }
        else
        {
            err = nvs_commit(ezlopi_nvs_handle);
            if (ESP_OK != err)
            {
                TRACE_E("(nvs_commit)-%s:: error: %s", key_name, esp_err_to_name(err));
            }
            else
            {
                TRACE_D("(nvs_commit)-%s successful.", key_name);
                error = EZPI_SUCCESS;
            }
        }
    }

    return error;
}

ezlopi_error_t ezlopi_nvs_read_int32(int32_t* i, const char* key_name)
{
    ezlopi_error_t error = EZPI_ERR_NVS_READ_FAILED;
    if (ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_get_i32(ezlopi_nvs_handle, key_name, i);
        if (ESP_OK == err)
        {
            error = EZPI_SUCCESS;
        }
        else
        {
            TRACE_W("(nvs_get_i32)-%s:: error: %s", key_name, esp_err_to_name(err));
        }
    }
    return error;
}

ezlopi_error_t ezlopi_nvs_write_uint32(uint32_t i, const char* key_name)
{
    ezlopi_error_t error = EZPI_ERR_NVS_READ_FAILED;
    if (ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_set_u32(ezlopi_nvs_handle, key_name, (uint32_t)i);
        if (ESP_OK != err)
        {
            TRACE_W("(nvs_set_u32)-%s:: error: %s", key_name, esp_err_to_name(err));
        }
        else
        {
            err = nvs_commit(ezlopi_nvs_handle);
            if (ESP_OK != err)
            {
                TRACE_W("(nvs_commit)-%s:: error: %s", key_name, esp_err_to_name(err));
            }
            else
            {
                TRACE_D("(nvs_commit)-%s:: Success.", key_name);
                error = EZPI_SUCCESS;
            }
        }
    }
    return error;
}

ezlopi_error_t ezlopi_nvs_read_uint32(uint32_t* i, const char* key_name)
{
    ezlopi_error_t error = EZPI_ERR_NVS_READ_FAILED;
    if (ezlopi_nvs_handle)
    {
        esp_err_t err = nvs_get_u32(ezlopi_nvs_handle, key_name, i);
        if (ESP_OK == err)
        {
            TRACE_D("(nvs_get_u32)-%s - NVS read success", key_name);
            error = EZPI_SUCCESS;
        }
        else
        {
            TRACE_E("(nvs_get_u32)-%s - error: %s", key_name, esp_err_to_name(err));
        }
    }
    return error;
}

ezlopi_error_t ezlopi_nvs_write_float32(float f, const char* key_name)
{
    ezlopi_error_t error = EZPI_ERR_NVS_WRITE_FAILED;
    if (ezlopi_nvs_handle)
    {
        uint32_t value;
        memcpy(&value, &f, sizeof(uint32_t));

        esp_err_t err = nvs_set_u32(ezlopi_nvs_handle, key_name, value);
        if (err != ESP_OK)
        {
            TRACE_W("(nvs_set_u32)-%s:: error: %s", key_name, esp_err_to_name(err));
        }
        else
        {
            err = nvs_commit(ezlopi_nvs_handle);
            if (ESP_OK != err)
            {
                TRACE_W("(nvs_commit)-%s:: error: %s", key_name, esp_err_to_name(err));
            }
            else
            {
                TRACE_D("(nvs_commit)-%s:: Success", key_name);
                error = EZPI_SUCCESS;
            }
        }
    }
    return error;
}

ezlopi_error_t ezlopi_nvs_read_float32(float* f, const char* key_name)
{
    ezlopi_error_t error = EZPI_ERR_NVS_READ_FAILED;
    if (ezlopi_nvs_handle)
    {
        uint32_t value;

        esp_err_t err = nvs_get_u32(ezlopi_nvs_handle, key_name, &value);
        if (err == ESP_OK)
        {
            memcpy(f, &value, sizeof(float));
            error = EZPI_SUCCESS;
        }
        else
        {
            TRACE_W("(nvs_get_u32)-%s:: error: %s", key_name, esp_err_to_name(err));
        }
    }
    return error;
}

ezlopi_error_t ezlopi_nvs_write_bool(bool b, const char* key_name)
{
    ezlopi_error_t error = EZPI_ERR_NVS_WRITE_FAILED;
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
            TRACE_W("(nvs_set_u8)-%s:: error: %s", key_name, esp_err_to_name(err));
        }
        else
        {
            err = nvs_commit(ezlopi_nvs_handle);
            if (ESP_OK != err)
            {
                TRACE_W("(nvs_commit)-%s:: error: %s", key_name, esp_err_to_name(err));
            }
            else
            {
                TRACE_W("(nvs_commit)-%s:: Success.", key_name);
                error = EZPI_SUCCESS;
            }
        }
    }
    return error;
}

ezlopi_error_t ezlopi_nvs_read_bool(bool* b, const char* key_name)
{
    ezlopi_error_t error = EZPI_ERR_NVS_READ_FAILED;
    if (ezlopi_nvs_handle)
    {
        uint8_t bool_val = 0;
        esp_err_t err = nvs_get_u8(ezlopi_nvs_handle, key_name, &bool_val);
        if (ESP_OK == err)
        {
            if (bool_val)
            {
                *b = true;
            }
            else
            {
                *b = false;
            }
            error = EZPI_SUCCESS;
        }
        else
        {
            TRACE_W("(nvs_get_u8)-%s:: error: %s", key_name, esp_err_to_name(err));
        }
    }
    return error;
}

ezlopi_error_t ezlopi_nvs_write_str(const char* data, uint32_t len, const char* nvs_name)
{
    ezlopi_error_t error = EZPI_ERR_NVS_WRITE_FAILED;

    if (data && nvs_name && len)
    {
        if (EZPI_SUCCESS == ezlopi_nvs_init())
        {
            esp_err_t err = nvs_set_str(ezlopi_nvs_handle, nvs_name, data);
            if (ESP_OK == err)
            {
                err = nvs_commit(ezlopi_nvs_handle);
                if (ESP_OK == err)
                {
                    error = EZPI_SUCCESS;
                    TRACE_D("(nvs_commit)-%s:: Success.", nvs_name);
                }
                else
                {
                    TRACE_E("(nvs_commit)-%s:: error: %s", nvs_name, esp_err_to_name(err));
                }
            }
            else
            {
                TRACE_W("(nvs_set_str)-%s:: error: %s", nvs_name, esp_err_to_name(err));
            }
        }
    }

    return error;
}

char *ezlopi_nvs_read_str(const char *nvs_name)
{
    char *return_str = NULL;

    if (nvs_name)
    {
        if (EZPI_SUCCESS == ezlopi_nvs_init())
        {
            esp_err_t err = ESP_OK;
            size_t buf_len_needed = 0;
            err = nvs_get_str(ezlopi_nvs_handle, nvs_name, NULL, &buf_len_needed);

            if (buf_len_needed && (ESP_OK == err))
            {
                return_str = ezlopi_malloc(__FUNCTION__, buf_len_needed + 1);

                if (return_str)
                {
                    memset(return_str, 0, buf_len_needed + 1);
                    err = nvs_get_str(ezlopi_nvs_handle, nvs_name, return_str, &buf_len_needed);

                    if (ESP_OK == err)
                    {
                        TRACE_D("(nvs_get_str)-%s:: Success.", nvs_name);
                        // TRACE_D("%s read success. \r\nData[%d]: \n%s", nvs_name, buf_len_needed, return_str);
                    }
                    else
                    {
                        TRACE_E("(nvs_get_str)-%s:: error: %s", nvs_name, esp_err_to_name(err));
                        ezlopi_free(__FUNCTION__, return_str);
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
                TRACE_W("%s: buf_len_needed: %d, err: %s", nvs_name, buf_len_needed, esp_err_to_name(err));
            }
        }
    }

    return return_str;
}

void ezlopi_nvs_delete_stored_data_by_id(uint32_t script_id)
{
    char script_id_str[32];
    snprintf(script_id_str, sizeof(script_id_str), "%08x", script_id);
    ezlopi_nvs_delete_stored_data_by_name(script_id_str);
}

void ezlopi_nvs_delete_stored_data_by_name(char *nvs_name)
{
    if (EZPI_SUCCESS == ezlopi_nvs_init())
    {
        esp_err_t err = ESP_OK;
        if (ESP_OK != (err = nvs_erase_key(ezlopi_nvs_handle, nvs_name)))
        {
            TRACE_E("(nvs_erase_key)-%s:: error: %s", nvs_name, esp_err_to_name(err));
        }
    }
}

ezlopi_error_t EZPI_CORE_nvs_write_time_location(const char* time_loc, uint32_t len)
{
    return ezlopi_nvs_write_str(time_loc, len, ezlopi_time_location_nvs_name);
}

char *EZPI_CORE_nvs_read_time_location(void)
{
    return ezlopi_nvs_read_str(ezlopi_time_location_nvs_name);
}

char *ezlopi_nvs_read_latidtude_longitude()
{
    return ezlopi_nvs_read_str(ezlopi_coordinates_nvs_name);
}

ezlopi_error_t ezlopi_nvs_write_latitude_longitude(char* data)
{
    ezlopi_error_t error = EZPI_ERR_NVS_WRITE_FAILED;
    if (data)
    {
        error = ezlopi_nvs_write_str(data, strlen(data), ezlopi_coordinates_nvs_name);
        if (EZPI_SUCCESS == error)
        {
            TRACE_E("Error writing latitude an longitude in nvs.");
        }
    }
    return error;
}

ezlopi_error_t EZPI_CORE_nvs_write_baud(uint32_t baud)
{
    // Key ezlopi_serial_baud_name
    return ezlopi_nvs_write_uint32(baud, ezlopi_serial_baud_name);
}

ezlopi_error_t EZPI_CORE_nvs_read_baud(uint32_t* baud)
{
    return ezlopi_nvs_read_uint32(baud, ezlopi_serial_baud_name);
}

ezlopi_error_t EZPI_CORE_nvs_write_parity(uint32_t parity)
{
    return ezlopi_nvs_write_uint32(parity, ezlopi_serial_parity);
}

ezlopi_error_t EZPI_CORE_nvs_read_parity(uint32_t* parity)
{
    return ezlopi_nvs_read_uint32(parity, ezlopi_serial_parity);
}

ezlopi_error_t EZPI_CORE_nvs_write_start_bits(uint32_t start_bits)
{
    return ezlopi_nvs_write_uint32(start_bits, ezlopi_serial_start_bits);
}
ezlopi_error_t EZPI_CORE_nvs_read_start_bits(uint32_t* start_bits)
{
    return ezlopi_nvs_read_uint32(start_bits, ezlopi_serial_start_bits);
}

ezlopi_error_t EZPI_CORE_nvs_write_stop_bits(uint32_t stop_bits)
{
    return ezlopi_nvs_write_uint32(stop_bits, ezlopi_serial_stop_bits);
}

ezlopi_error_t EZPI_CORE_nvs_read_stop_bits(uint32_t* stop_bits)
{
    return ezlopi_nvs_read_uint32(stop_bits, ezlopi_serial_stop_bits);
}

ezlopi_error_t EZPI_CORE_nvs_write_frame_size(uint32_t frame_size)
{
    return ezlopi_nvs_write_uint32(frame_size, ezlopi_serial_frame_size);
}

ezlopi_error_t EZPI_CORE_nvs_read_frame_size(uint32_t* frame_size)
{
    return ezlopi_nvs_read_uint32(frame_size, ezlopi_serial_frame_size);
}

ezlopi_error_t EZPI_CORE_nvs_write_flow_control(uint32_t flow_control)
{
    return ezlopi_nvs_write_uint32(flow_control, ezlopi_serial_flow_control);
}

ezlopi_error_t EZPI_CORE_nvs_read_flow_control(uint32_t* flow_control)
{
    return ezlopi_nvs_read_uint32(flow_control, ezlopi_serial_flow_control);
}

ezlopi_error_t EZPI_CORE_nvs_write_cloud_log_severity(uint32_t severity)
{
    return ezlopi_nvs_write_uint32(severity, ezlopi_cloud_log_severity);
}

ezlopi_error_t EZPI_CORE_nvs_read_cloud_log_severity(uint32_t* severity)
{
    return ezlopi_nvs_read_uint32(severity, ezlopi_cloud_log_severity);
}

ezlopi_error_t EZPI_CORE_nvs_write_serial_log_severity(uint32_t severity)
{
    return ezlopi_nvs_write_uint32(severity, ezlopi_serial_log_severity);
}

ezlopi_error_t EZPI_CORE_nvs_read_serial_log_severity(uint32_t* severity)
{
    return ezlopi_nvs_read_uint32(severity, ezlopi_serial_log_severity);
}

ezlopi_error_t EZPI_CORE_nvs_write_temperature_scale(uint32_t scale)
{
    return ezlopi_nvs_write_uint32(scale, ezlopi_temp_scale);
}

ezlopi_error_t EZPI_CORE_nvs_read_temperature_scale(uint32_t *scale)
{
    return ezlopi_nvs_read_uint32(scale, ezlopi_temp_scale);
}

ezlopi_error_t EZPI_CORE_nvs_write_date_format(uint32_t format)
{
    return ezlopi_nvs_write_uint32(format, ezlopi_date_format);
}

ezlopi_error_t EZPI_CORE_nvs_read_date_format(uint32_t *format)
{
    return ezlopi_nvs_read_uint32(format, ezlopi_date_format);
}

ezlopi_error_t EZPI_CORE_nvs_write_time_format(uint32_t format)
{
    return ezlopi_nvs_write_uint32(format, ezlopi_time_format);
}

ezlopi_error_t EZPI_CORE_nvs_read_time_format(uint32_t *format)
{
    return ezlopi_nvs_read_uint32(format, ezlopi_time_format);
}

ezlopi_error_t EZPI_CORE_nvs_write_network_ping_timeout(uint32_t ping_timeout)
{
    return ezlopi_nvs_write_uint32(ping_timeout, ezlopi_network_ping_timeout);
}

ezlopi_error_t EZPI_CORE_nvs_read_network_ping_timeout(uint32_t *ping_timeout)
{
    return ezlopi_nvs_read_uint32(ping_timeout, ezlopi_network_ping_timeout);
}

/*******************************************************************************
 *                          Static Function Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
