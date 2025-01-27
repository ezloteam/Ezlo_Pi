/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
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
 * @file    hilink_presence_sensor_setting.c
 * @brief   perform some function on hilink_presence_sensor_setting
 * @author
 * @version 0.1
 * @date    xx
 */

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include <string.h>
#include <stdlib.h>

#include "ld2410.h"

#include "ezlopi_core_nvs.h"
// #include "ezlopi_core_cloud.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_cloud_settings.h"
#include "ezlopi_cloud_constants.h"

#include "hilink_presence_sensor_setting.h"
#include "sensor_0067_hilink_presence_sensor.h"
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
static int __settings_callback(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3 *setting, void *arg, void *user_arg);
static int __settings_get(void *arg, l_ezlopi_device_settings_v3_t *setting);
static int __settings_set(void *arg, l_ezlopi_device_settings_v3_t *setting);
static int __settings_reset(void *arg, l_ezlopi_device_settings_v3_t *setting);
static int __settings_update(void *arg, l_ezlopi_device_settings_v3_t *setting);
// Setting initialization declarations
static int __setting_initialize_hilink_presence_sensor_predefined_settings(l_ezlopi_device_t *device);
static int __setting_initialize_hilink_presence_sensor_userdefined_settings(l_ezlopi_device_t *device);
static int __setting_initialize_hilink_presence_sensor_radar_distance_sensitivity(l_ezlopi_device_t *device);
// Setting getters declarations
static int __setting_get_sensor_enum(cJSON *cj_enum);
static int __setting_get_pre_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting);
static int __setting_get_user_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting);
static int __setting_get_radar_distance_sensitivity_setting(void *arg, l_ezlopi_device_settings_v3_t *setting);
// Setting setter declearations
static int __setting_set_change_user_defined_template(bool set_active);
static int __setting_set_change_setting_template(ld2410_template_t template, uint32_t setting_id);
static int __setting_set_find_predefined_setting_template(s_hilink_predefined_setting_value_t *setting_value, ld2410_template_t *template);
static int __setting_set_pre_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting);
static int __setting_set_user_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting);
// Setting reset declarations
static int __setting_reset_pre_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting);
static int __setting_reset_user_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting);
static int __setting_reset_radar_distance_sensitivity_setting(void *arg, l_ezlopi_device_settings_v3_t *setting);
// Setting update declarations
static int __setting_update_pre_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting);
static int __setting_update_user_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting);
static int __setting_update_radar_distance_sensitivity_setting(void *arg, l_ezlopi_device_settings_v3_t *setting);

// common static function
static inline cJSON *__setting_add_text_and_lang_tag(const char *const object_text, const char *const object_lang_tag);
static inline void __setting_extract_user_defined_setting(cJSON *cj_value, s_hilink_userdefined_setting_value_t *user_defined_setting_val);
static inline int __prepare_user_defined_setting_cjson(cJSON *cj_value, s_hilink_userdefined_setting_value_t *setting_val);
static inline char *__prepare_user_defined_setting_str(s_hilink_userdefined_setting_value_t *setting_val);
static inline distance_t __setting_user_defined_setting_get_enum(float val);

/*******************************************************************************
 *                          Static Data Definitions
 *******************************************************************************/

// static const char *nvs_key_hilink_presence_sensor_predefined_setting = "predef";
// static const char *nvs_key_hilink_presence_sensor_userdefined_setting = "userdef";
// static const char *nvs_key_hilink_presence_sensor_radar_distance_sensitivity = "rdrsens";
#warning "NABIN ; Need to remove these global variables";
static uint32_t hilink_presence_sensor_setting_ids[3] = {0, 0, 0};

static l_ezlopi_device_settings_v3_t *hilink_presence_sensor_radar_distance_sensitivity_setting;
static l_ezlopi_device_settings_v3_t *hilink_presence_sensor_user_defined_setting;
static l_ezlopi_device_settings_v3_t *hilink_presence_sensor_predefined_setting;

// static const char *hilink_presence_sensor_setting_enum[] = {
//     "sleep_mode_close",
//     "sleep_mode_mid",
//     "sleep_mode_long",
//     "movement_mode_close",
//     "movement_mode_mid",
//     "movement_mode_long",
//     "basic_routine_mode_close",
//     "basic_routine_mode_mid",
//     "basic_routine_mode_long",
//     "user_defined_mode",
// };

static const s_hilink_userdefined_setting_value_t hilink_user_defined_setting_default_value = {
    .min_move_distance = 0.75,
    .max_move_distance = 6.0,
    .min_still_distance = 0.75,
    .max_still_distance = 6.0,
    .timeout = 50,
    .is_active = false,
};

/*******************************************************************************
 *                          Extern Data Definitions
 *******************************************************************************/

/*******************************************************************************
 *                          Extern Function Definitions
 *******************************************************************************/
int HILINK_presence_sensor_apply_settings(void)
{
    int ret = 0;
    if (hilink_presence_sensor_predefined_setting && hilink_presence_sensor_user_defined_setting)
    {
        // Only the predefined setting mode is required to be accessed, if it is set to user_defined_mode, the settings are changed according to the user defined setting automatically.
        s_hilink_predefined_setting_value_t *predef_setting_value = (s_hilink_predefined_setting_value_t *)hilink_presence_sensor_predefined_setting->user_arg;
        if (predef_setting_value)
        {
            ld2410_template_t template;
            if (!__setting_set_find_predefined_setting_template(predef_setting_value, &template))
            {
                ret = __setting_set_change_setting_template(template, hilink_presence_sensor_predefined_setting->cloud_properties.setting_id);
            }
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }
    return ret;
}
int HILINK_presence_sensor_initialize_settings(l_ezlopi_device_t *device)
{
    int ret = 0;

    ESP_ERROR_CHECK(__setting_initialize_hilink_presence_sensor_predefined_settings(device));
    ESP_ERROR_CHECK(__setting_initialize_hilink_presence_sensor_userdefined_settings(device));
    ESP_ERROR_CHECK(__setting_initialize_hilink_presence_sensor_radar_distance_sensitivity(device));

    return ret;
}
bool HILINK_presence_sensor_target_in_detectable_range(const uint16_t moving_target_distance)
{
    bool in_range = false;
    s_hilink_radar_distance_sensitivity_value_t *setting_val = (s_hilink_radar_distance_sensitivity_value_t *)hilink_presence_sensor_radar_distance_sensitivity_setting->user_arg;
    if (setting_val)
    {
        if (setting_val->distance_sensitivity_value <= moving_target_distance)
        {
            in_range = true;
        }
        else
        {
            in_range = false;
        }
    }
    else
    {
        in_range = false;
    }
    return in_range;
}

/*******************************************************************************
 *                         Static Function Definitions
 *******************************************************************************/

// ********************************************* Setting initialization related start ********************************************* //
static int __setting_initialize_hilink_presence_sensor_predefined_settings(l_ezlopi_device_t *device)
{
    int ret = 0;

    hilink_presence_sensor_setting_ids[0] = EZPI_core_cloud_generate_settings_id();
    hilink_presence_sensor_predefined_setting = EZPI_core_device_add_settings_to_device_v3(device, __settings_callback);
    if (hilink_presence_sensor_predefined_setting)
    {
        hilink_presence_sensor_predefined_setting->cloud_properties.setting_id = hilink_presence_sensor_setting_ids[0];
        s_hilink_predefined_setting_value_t *hilink_presence_sensor_setting_value = (s_hilink_predefined_setting_value_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_hilink_predefined_setting_value_t));
        if (hilink_presence_sensor_setting_value)
        {
            memset(hilink_presence_sensor_setting_value, 0, sizeof(s_hilink_predefined_setting_value_t));
            char *read_value = EZPI_core_nvs_read_str("predef");
            if (NULL != read_value)
            {
                TRACE_I("Setting already exist");
                snprintf(hilink_presence_sensor_setting_value->setting_value, 50, "%s", read_value);
            }
            else
            {
                TRACE_W("Not found saved setting for predefined setting value!");
                snprintf(hilink_presence_sensor_setting_value->setting_value, 50, "%s", "basic_routine_mode_mid");
                if (EZPI_SUCCESS == EZPI_core_nvs_write_str(hilink_presence_sensor_setting_value->setting_value, strlen(hilink_presence_sensor_setting_value->setting_value), "predef"))
                {
                    TRACE_E("Failed to write to NVS");
                    ret = 1;
                }
            }
            hilink_presence_sensor_predefined_setting->user_arg = (void *)hilink_presence_sensor_setting_value;
        }
        else
        {
            ezlopi_free(__FUNCTION__, hilink_presence_sensor_predefined_setting);
            ret = 1;
        }
    }

    return ret;
}
static int __setting_initialize_hilink_presence_sensor_userdefined_settings(l_ezlopi_device_t *device)
{
    int ret = 0;

    hilink_presence_sensor_setting_ids[1] = EZPI_core_cloud_generate_settings_id();
    hilink_presence_sensor_user_defined_setting = EZPI_core_device_add_settings_to_device_v3(device, __settings_callback);
    if (hilink_presence_sensor_user_defined_setting)
    {
        hilink_presence_sensor_user_defined_setting->cloud_properties.setting_id = hilink_presence_sensor_setting_ids[1];
        s_hilink_userdefined_setting_value_t *hilink_presence_sensor_user_defined_setting_val = (s_hilink_userdefined_setting_value_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_hilink_userdefined_setting_value_t));
        if (hilink_presence_sensor_user_defined_setting_val)
        {
            memset(hilink_presence_sensor_user_defined_setting_val, 0, sizeof(s_hilink_userdefined_setting_value_t));
            char *read_value = EZPI_core_nvs_read_str("userdef");
            if (NULL != read_value)
            {
                TRACE_I("Setting already exist.");
                cJSON *cj_value = cJSON_Parse(__FUNCTION__, read_value);
                ezlopi_free(__FUNCTION__, read_value);

                if (cj_value)
                {
                    __setting_extract_user_defined_setting(cj_value, hilink_presence_sensor_user_defined_setting_val);
                    cJSON_Delete(__FUNCTION__, cj_value);
                }
            }
            else
            {
                TRACE_W("Not found saved setting for predefined setting value!");
                memcpy(hilink_presence_sensor_user_defined_setting_val, &hilink_user_defined_setting_default_value, sizeof(s_hilink_userdefined_setting_value_t));
                char *user_defined_value = __prepare_user_defined_setting_str(hilink_presence_sensor_user_defined_setting_val);
                if (user_defined_value)
                {
                    if (EZPI_SUCCESS == EZPI_core_nvs_write_str(user_defined_value, strlen(user_defined_value), "userdef"))
                    {
                        TRACE_E("Failed to write to NVS");
                        ret = 1;
                    }
                }
                else
                {
                    ret = 1;
                }
            }
            hilink_presence_sensor_user_defined_setting->user_arg = (void *)hilink_presence_sensor_user_defined_setting_val;
        }
        else
        {
            ezlopi_free(__FUNCTION__, hilink_presence_sensor_user_defined_setting);
            ret = 1;
        }
    }
    else
    {
        ezlopi_free(__FUNCTION__, hilink_presence_sensor_user_defined_setting);
        ret = 1;
    }

    return ret;
}
static int __setting_initialize_hilink_presence_sensor_radar_distance_sensitivity(l_ezlopi_device_t *device)
{
    int ret = 0;
    if (device)
    {
        hilink_presence_sensor_setting_ids[2] = EZPI_core_cloud_generate_settings_id();
        hilink_presence_sensor_radar_distance_sensitivity_setting = EZPI_core_device_add_settings_to_device_v3(device, __settings_callback);
        if (hilink_presence_sensor_radar_distance_sensitivity_setting)
        {
            hilink_presence_sensor_radar_distance_sensitivity_setting->cloud_properties.setting_id = hilink_presence_sensor_setting_ids[2];
            s_hilink_radar_distance_sensitivity_value_t *distance_sensitivity_value = (s_hilink_radar_distance_sensitivity_value_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_hilink_radar_distance_sensitivity_value_t));
            if (distance_sensitivity_value)
            {
                memset(distance_sensitivity_value, 0, sizeof(s_hilink_radar_distance_sensitivity_value_t));
                int read_value = 0;
                ezlopi_error_t error = EZPI_core_nvs_read_int32(&read_value, "rdrsens");
                if (EZPI_SUCCESS == error)
                {
                    TRACE_I("Setting already exist.");
                    distance_sensitivity_value->distance_sensitivity_value = read_value;
                }
                else
                {
                    TRACE_W("Not found saved setting for predefined setting value!");
                    distance_sensitivity_value->distance_sensitivity_value = 10;
                    if (EZPI_SUCCESS != EZPI_core_nvs_write_int32(distance_sensitivity_value->distance_sensitivity_value, "rdrsens"))
                    {
                        TRACE_E("Failed to write to NVS");
                        ret = 1;
                    }
                }
                hilink_presence_sensor_radar_distance_sensitivity_setting->user_arg = distance_sensitivity_value;
            }
            else
            {
                ezlopi_free(__FUNCTION__, distance_sensitivity_value);
                ret = 0;
            }
        }
        else
        {
            ezlopi_free(__FUNCTION__, hilink_presence_sensor_radar_distance_sensitivity_setting);
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }
    return ret;
}

// ********************************************* Setting initialization related end ********************************************* //
static int __settings_callback(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3 *setting, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_SETTINGS_ACTION_GET_SETTING:
    {
        TRACE_I("%s", stringify(EZLOPI_SETTINGS_ACTION_GET_SETTING));
        __settings_get(arg, setting);
        break;
    }
    case EZLOPI_SETTINGS_ACTION_SET_SETTING:
    {
        TRACE_I("%s", stringify(EZLOPI_SETTINGS_ACTION_SET_SETTING));
        __settings_set(arg, setting);
        break;
    }
    case EZLOPI_SETTINGS_ACTION_RESET_SETTING:
    {
        TRACE_I("%s", stringify(EZLOPI_SETTINGS_ACTION_RESET_SETTING));
        __settings_reset(arg, setting);
        break;
    }
    case EZLOPI_SETTINGS_ACTION_UPDATE_SETTING:
    {
        TRACE_I("%s", stringify(EZLOPI_SETTINGS_ACTION_UPDATE_SETTING));
        __settings_update(arg, setting);
        break;
    }

    default:
    {
        break;
    }
    }

    return ret;
}
// ********************************************* hub.device.settings.list related start ********************************************* //
static int __setting_get_sensor_enum(cJSON *cj_enum)
{
    int ret = 0;

    if (cj_enum)
    {
        cJSON_AddItemToObject(__FUNCTION__, cj_enum, "sleep_mode_close", __setting_add_text_and_lang_tag(SLEEP_MODE_CLOSE_TEXT, SLEEP_MODE_CLOSE_LANG_TAG));
        cJSON_AddItemToObject(__FUNCTION__, cj_enum, "sleep_mode_mid", __setting_add_text_and_lang_tag(SLEEP_MODE_MID_TEXT, SLEEP_MODE_MID_LANG_TAG));
        cJSON_AddItemToObject(__FUNCTION__, cj_enum, "sleep_mode_long", __setting_add_text_and_lang_tag(SLEEP_MODE_LONG_TEXT, SLEEP_MODE_LONG_LANG_TAG));
        cJSON_AddItemToObject(__FUNCTION__, cj_enum, "movement_mode_close", __setting_add_text_and_lang_tag(MOVEMENT_MODE_CLOSE_TEXT, MOVEMENT_MODE_CLOSE_LANG_TAG));
        cJSON_AddItemToObject(__FUNCTION__, cj_enum, "movement_mode_mid", __setting_add_text_and_lang_tag(MOVEMENT_MODE_MID_TEXT, MOVEMENT_MODE_MID_LANG_TAG));
        cJSON_AddItemToObject(__FUNCTION__, cj_enum, "movement_mode_long", __setting_add_text_and_lang_tag(MOVEMENT_MODE_LONG_TEXT, MOVEMENT_MODE_LONG_LANG_TAG));
        cJSON_AddItemToObject(__FUNCTION__, cj_enum, "basic_routine_mode_close", __setting_add_text_and_lang_tag(BASIC_ROUTINE_MODE_CLOSE_TEXT, BASIC_ROUTINE_MODE_CLOSE_LANG_TAG));
        cJSON_AddItemToObject(__FUNCTION__, cj_enum, "basic_routine_mode_mid", __setting_add_text_and_lang_tag(BASIC_ROUTINE_MODE_MID_TEXT, BASIC_ROUTINE_MODE_MID_LANG_TAG));
        cJSON_AddItemToObject(__FUNCTION__, cj_enum, "basic_routine_mode_long", __setting_add_text_and_lang_tag(BASIC_ROUTINE_MODE_LONG_TEXT, BASIC_ROUTINE_MODE_LONG_LANG_TAG));
        cJSON_AddItemToObject(__FUNCTION__, cj_enum, "user_defined_mode", __setting_add_text_and_lang_tag(USER_DEFINED_MODE_TEXT, USER_DEFINED_MODE_LANG_TAG));
    }
    else
    {
        ret = 1;
    }
    return ret;
}
static int __setting_get_pre_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    cJSON *cj_properties = (cJSON *)arg;
    s_hilink_predefined_setting_value_t *setting_value = (s_hilink_predefined_setting_value_t *)setting->user_arg;
    if (cj_properties && setting_value)
    {
        cJSON_AddItemToObject(__FUNCTION__, cj_properties, ezlopi_label_str, __setting_add_text_and_lang_tag(HILINK_PRESENCE_SENSOR_SETTING_PREDEFINED_LABEL_TEXT, HILINK_PRESENCE_SENSOR_SETTING_PREDEFINED_LABEL_LANG_TAG));
        cJSON_AddItemToObject(__FUNCTION__, cj_properties, ezlopi_description_str, __setting_add_text_and_lang_tag(HILINK_PRESENCE_SENSOR_SETTING_PREDEFINED_DESCRIPTION_TEXT, HILINK_PRESENCE_SENSOR_SETTING_PREDEFINED_DESCRIPTION_LANG_TAG));

        cJSON_AddStringToObject(__FUNCTION__, cj_properties, ezlopi_status_str, ezlopi_synced_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_properties, ezlopi_valueType_str, value_type_token);

        cJSON *enum_object = cJSON_AddObjectToObject(__FUNCTION__, cj_properties, ezlopi_enum_str);
        if (enum_object)
        {

            if (ESP_OK == __setting_get_sensor_enum(enum_object))
            {
                ret = 0;
            }
            else
            {
                ret = 1;
            }
        }
        else
        {
            ret = 1;
        }
        cJSON_AddStringToObject(__FUNCTION__, cj_properties, ezlopi_value_str, setting_value->setting_value);
        cJSON_AddStringToObject(__FUNCTION__, cj_properties, ezlopi_valueDefault_str, "basic_routine_mode_mid");
    }
    else
    {
        ret = 1;
    }
    return ret;
}
static int __setting_get_user_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    cJSON *cj_properties = (cJSON *)arg;
    s_hilink_userdefined_setting_value_t *setting_value = (s_hilink_userdefined_setting_value_t *)setting->user_arg;
    if (cj_properties && setting_value)
    {
        cJSON_AddItemToObject(__FUNCTION__, cj_properties, ezlopi_label_str, __setting_add_text_and_lang_tag(HILINK_PRESENCE_SENSOR_SETTING_USER_DEFINED_LABEL_TEXT, HILINK_PRESENCE_SENSOR_SETTING_USER_DEFINED_LABEL_LANG_TAG));
        cJSON_AddItemToObject(__FUNCTION__, cj_properties, ezlopi_description_str, __setting_add_text_and_lang_tag(HILINK_PRESENCE_SENSOR_SETTING_USER_DEFINED_DESCRIPTION_TEXT, HILINK_PRESENCE_SENSOR_SETTING_USER_DEFINED_DESCRIPTION_LANG_TAG));

        cJSON_AddStringToObject(__FUNCTION__, cj_properties, ezlopi_status_str, ezlopi_synced_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_properties, ezlopi_valueType_str, value_type_presence_operation_mode);

        cJSON *cj_value = cJSON_AddObjectToObject(__FUNCTION__, cj_properties, ezlopi_value_str);
        if (cj_value)
        {
            ESP_ERROR_CHECK(__prepare_user_defined_setting_cjson(cj_value, setting_value));
        }
        cJSON *cj_value_default = cJSON_AddObjectToObject(__FUNCTION__, cj_properties, ezlopi_valueDefault_str);
        if (cj_value_default)
        {
            ESP_ERROR_CHECK(__prepare_user_defined_setting_cjson(cj_value_default, (s_hilink_userdefined_setting_value_t *)&hilink_user_defined_setting_default_value));
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}
static int __setting_get_radar_distance_sensitivity_setting(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    cJSON *cj_properties = (cJSON *)arg;
    s_hilink_radar_distance_sensitivity_value_t *setting_value = (s_hilink_radar_distance_sensitivity_value_t *)setting->user_arg;
    if (cj_properties && setting_value)
    {
        cJSON_AddItemToObject(__FUNCTION__, cj_properties, ezlopi_label_str, __setting_add_text_and_lang_tag(HILINK_PRESENCE_SENSOR_SETTING_RADAR_DISTANCE_SENSITIVITY_LABEL_TEXT, HILINK_PRESENCE_SENSOR_SETTING_RADAR_DISTANCE_SENSITIVITY_LABEL_LANG_TAG));
        cJSON_AddItemToObject(__FUNCTION__, cj_properties, ezlopi_description_str, __setting_add_text_and_lang_tag(HILINK_PRESENCE_SENSOR_SETTING_RADAR_DISTANCE_SENSITIVITY_DESCRIPTION_TEXT, HILINK_PRESENCE_SENSOR_SETTING_RADAR_DISTANCE_SENSITIVITY_DESCRIPTION_LANG_TAG));

        cJSON_AddStringToObject(__FUNCTION__, cj_properties, ezlopi_status_str, ezlopi_synced_str);
        cJSON_AddStringToObject(__FUNCTION__, cj_properties, ezlopi_valueType_str, value_type_int);
        cJSON_AddNumberToObject(__FUNCTION__, cj_properties, ezlopi_value_str, setting_value->distance_sensitivity_value);
        cJSON_AddNumberToObject(__FUNCTION__, cj_properties, ezlopi_valueMin_str, 5);
        cJSON_AddNumberToObject(__FUNCTION__, cj_properties, ezlopi_valueMax_str, 30);
        cJSON_AddNumberToObject(__FUNCTION__, cj_properties, ezlopi_valueDefault_str, 10);
    }
    else
    {
        ret = 1;
    }

    return ret;
}
static int __settings_get(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    if (hilink_presence_sensor_setting_ids[0] == setting->cloud_properties.setting_id)
    {
        ESP_ERROR_CHECK(__setting_get_pre_defined_setting(arg, setting));
    }
    if (hilink_presence_sensor_setting_ids[1] == setting->cloud_properties.setting_id)
    {
        ESP_ERROR_CHECK(__setting_get_user_defined_setting(arg, setting));
    }
    if (hilink_presence_sensor_setting_ids[2] == setting->cloud_properties.setting_id)
    {
        ESP_ERROR_CHECK(__setting_get_radar_distance_sensitivity_setting(arg, setting));
    }
    return ret;
}
// ********************************************* hub.device.settings.list related end ********************************************* //

// ********************************************* hub.device.setting.value.set related start ********************************************* //
// If set_active is true, is_active member is set to true and sent to the cloud accordingly.
static int __setting_set_change_user_defined_template(bool set_active)
{
    int ret = 0;

    s_hilink_userdefined_setting_value_t *setting_val = (s_hilink_userdefined_setting_value_t *)hilink_presence_sensor_user_defined_setting->user_arg;
    if (setting_val)
    {
        if (set_active)
        {
            ld2410_settings_t ld2410_setting = {
                .max_move_distance = __setting_user_defined_setting_get_enum(setting_val->max_move_distance),
                .max_still_distance = __setting_user_defined_setting_get_enum(setting_val->max_still_distance),
                .min_move_distance = __setting_user_defined_setting_get_enum(setting_val->min_move_distance),
                .min_still_distance = __setting_user_defined_setting_get_enum(setting_val->min_still_distance),
                .no_one_duration = setting_val->timeout,
            };
            ld2410_set_template(CUSTOM_TEMPLATE, &ld2410_setting);
            setting_val->is_active = true;
            char *value_str = __prepare_user_defined_setting_str(setting_val);
            if (value_str)
            {
                if (EZPI_SUCCESS == EZPI_core_nvs_write_str(value_str, strlen(value_str), "userdef"))
                {
                    ret = 1;
                }
            }
        }
    }

    return ret;
}
static int __setting_set_change_setting_template(ld2410_template_t template, uint32_t setting_id)
{
    int ret = 0;

    if (template != CUSTOM_TEMPLATE)
    {
        // Directly change the setting if template is other predefined templates.
        ret = 0;
        esp_err_t error = ld2410_set_template(template, NULL);
        if (ESP_OK != error)
        {
            ret = 1;
        }
    }
    else
    {
        // Check if the setting change request comes from predefined mode setting.
        if (setting_id == hilink_presence_sensor_setting_ids[0])
        {
            __setting_set_change_user_defined_template(true);
            EZPI_core_device_value_updated_settings_broadcast(hilink_presence_sensor_user_defined_setting);
        }
        else
        {
            s_hilink_predefined_setting_value_t *predef_value = (s_hilink_predefined_setting_value_t *)hilink_presence_sensor_predefined_setting->user_arg;
            // If setting change request comes from user defined setting, check if predefined setting mode is set to user defined mode; in this case 9th index in enum str array.
            if (predef_value && (0 == strncmp(predef_value->setting_value, "user_defined_mode", ((strlen(predef_value->setting_value) + 1) > 18 ? (strlen(predef_value->setting_value) + 1) : 18))))
            {
                __setting_set_change_user_defined_template(true);
                EZPI_core_device_value_updated_settings_broadcast(hilink_presence_sensor_predefined_setting);
            }
            else
            {
                // Don't change is_active member if user defined setting is changed but predefined setting is not set to user_defined_mode.
                __setting_set_change_user_defined_template(false);
            }
        }
    }
    return ret;
}
static int __setting_set_find_predefined_setting_template(s_hilink_predefined_setting_value_t *setting_value, ld2410_template_t *template)
{
    int ret = 0;
    if (0 == strncmp(setting_value->setting_value, "sleep_mode_close", ((strlen(setting_value->setting_value) + 1) > 17 ? (strlen(setting_value->setting_value) + 1) : 17))) // sleep_mode_close
    {
        *template = SLEEP_TEMPLATE_CLOSE_RANGE;
    }
    else if (0 == strncmp(setting_value->setting_value, "sleep_mode_mid", ((strlen(setting_value->setting_value) + 1) > 15 ? (strlen(setting_value->setting_value) + 1) : 15))) // sleep_mode_mid
    {
        *template = SLEEP_TEMPLATE_MID_RANGE;
    }
    else if (0 == strncmp(setting_value->setting_value, "sleep_mode_long", ((strlen(setting_value->setting_value) + 1) > 16 ? (strlen(setting_value->setting_value) + 1) : 16))) // sleep_mode_long
    {
        *template = SLEEP_TEMPLATE_LONG_RANGE;
    }
    else if (0 == strncmp(setting_value->setting_value, "movement_mode_close", ((strlen(setting_value->setting_value) + 1) > 20 ? (strlen(setting_value->setting_value) + 1) : 20))) // movement_mode_close
    {
        *template = MOVEMENT_TEMPLATE_CLOSE_RANGE;
    }
    else if (0 == strncmp(setting_value->setting_value, "movement_mode_mid", ((strlen(setting_value->setting_value) + 1) > 18 ? (strlen(setting_value->setting_value) + 1) : 18))) // movement_mode_mid
    {
        *template = MOVEMENT_TEMPLATE_MID_RANGE;
    }
    else if (0 == strncmp(setting_value->setting_value, "movement_mode_long", ((strlen(setting_value->setting_value) + 1) > 19 ? (strlen(setting_value->setting_value) + 1) : 19))) // movement_mode_long
    {
        *template = MOVEMENT_TEMPLATE_LONG_RANGE;
    }
    else if (0 == strncmp(setting_value->setting_value, "basic_routine_mode_close", ((strlen(setting_value->setting_value) + 1) > 25 ? (strlen(setting_value->setting_value) + 1) : 25))) // basic_routine_mode_close
    {
        *template = BASIC_TEMPLATE_CLOSE_RANGE;
    }
    else if (0 == strncmp(setting_value->setting_value, "basic_routine_mode_mid", ((strlen(setting_value->setting_value) + 1) > 23 ? (strlen(setting_value->setting_value) + 1) : 23))) // basic_routine_mode_mid
    {
        *template = BASIC_TEMPLATE_MID_RANGE;
    }
    else if (0 == strncmp(setting_value->setting_value, "basic_routine_mode_long", ((strlen(setting_value->setting_value) + 1) > 24 ? (strlen(setting_value->setting_value) + 1) : 24))) // basic_routine_mode_long
    {
        *template = BASIC_TEMPLATE_LONG_RANGE;
    }
    else if (0 == strncmp(setting_value->setting_value, "user_defined_mode", ((strlen(setting_value->setting_value) + 1) > 18 ? (strlen(setting_value->setting_value) + 1) : 18))) // user_defined_mode
    {
        *template = CUSTOM_TEMPLATE;
    }
    else
    {
        TRACE_E("Provided predefined setting is unreconized.");
        ret = 1;
    }

    return ret;
}
static int __setting_set_pre_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    cJSON *cj_properties = (cJSON *)arg;
    ld2410_template_t template;
    if (cj_properties && setting)
    {
        s_hilink_predefined_setting_value_t *setting_value = (s_hilink_predefined_setting_value_t *)setting->user_arg;
        if (setting_value)
        {
            CJSON_GET_VALUE_STRING_BY_COPY(cj_properties, ezlopi_value_str, setting_value->setting_value, sizeof(setting_value->setting_value));
            if (EZPI_SUCCESS != EZPI_core_nvs_write_str(setting_value->setting_value, strlen(setting_value->setting_value), "predef"))
            {
                TRACE_E("Failed to write to NVS");
                ret = 1;
            }
            if (!__setting_set_find_predefined_setting_template(setting_value, &template))
            {
                ret = __setting_set_change_setting_template(template, setting->cloud_properties.setting_id);
            }
            else
            {
                ret = 1;
            }
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}
static int __setting_set_user_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    cJSON *cj_properties = (cJSON *)arg;
    s_hilink_userdefined_setting_value_t *setting_val = (s_hilink_userdefined_setting_value_t *)setting->user_arg;
    if (cj_properties && setting && setting_val)
    {
        cJSON *cj_value = cJSON_GetObjectItem(__FUNCTION__, cj_properties, ezlopi_value_str);
        if (cj_value)
        {
            __setting_extract_user_defined_setting(cj_value, setting_val);

            char *value_str = __prepare_user_defined_setting_str(setting_val);
            if (value_str)
            {
                if (EZPI_SUCCESS == EZPI_core_nvs_write_str(value_str, strlen(value_str), "userdef"))
                {
                    ret = 1;
                }
                else
                {
                    ret = __setting_set_change_setting_template(CUSTOM_TEMPLATE, setting->cloud_properties.setting_id); // user_defined_mode
                }
            }
            else
            {
                ret = 1;
            }
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}
static int __setting_set_radar_distance_sensitivity_setting(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    cJSON *cj_properties = (cJSON *)arg;
    s_hilink_radar_distance_sensitivity_value_t *setting_val = (s_hilink_radar_distance_sensitivity_value_t *)setting->user_arg;
    if (cj_properties && setting && setting_val)
    {
        CJSON_GET_VALUE_INT(cj_properties, ezlopi_value_str, setting_val->distance_sensitivity_value);
        if (EZPI_SUCCESS != EZPI_core_nvs_write_int32(setting_val->distance_sensitivity_value, "rdrsens"))
        {
            TRACE_E("Failed to write to NVS");
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}
static int __settings_set(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    if (hilink_presence_sensor_setting_ids[0] == setting->cloud_properties.setting_id)
    {
        ESP_ERROR_CHECK(__setting_set_pre_defined_setting(arg, setting));
    }
    if (hilink_presence_sensor_setting_ids[1] == setting->cloud_properties.setting_id)
    {
        ESP_ERROR_CHECK(__setting_set_user_defined_setting(arg, setting));
    }
    if (hilink_presence_sensor_setting_ids[2] == setting->cloud_properties.setting_id)
    {
        ESP_ERROR_CHECK(__setting_set_radar_distance_sensitivity_setting(arg, setting));
    }

    EZPI_core_device_value_updated_settings_broadcast(setting);

    return ret;
}
// ********************************************* hub.device.setting.value.set related end ********************************************* //

// ********************************************* hub.device.setting.reset related start ********************************************* //
static int __setting_reset_pre_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    cJSON *cj_params = (cJSON *)arg;
    if (cj_params && setting)
    {
        s_hilink_predefined_setting_value_t *setting_val = (s_hilink_predefined_setting_value_t *)setting->user_arg;
        if (setting_val)
        {
            snprintf(setting_val->setting_value, 50, "%s", "basic_routine_mode_mid");
            if (EZPI_SUCCESS == EZPI_core_nvs_write_str(setting_val->setting_value, strlen(setting_val->setting_value), "predef"))
            {
                TRACE_E("Failed to write to NVS");
                ret = 1;
            }
            else
            {
                ret = __setting_set_change_setting_template(BASIC_TEMPLATE_MID_RANGE, setting->cloud_properties.setting_id);
            }
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

static int __setting_reset_user_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    cJSON *cj_params = (cJSON *)arg;
    if (cj_params && setting)
    {
        s_hilink_userdefined_setting_value_t *setting_val = (s_hilink_userdefined_setting_value_t *)setting->user_arg;
        if (setting)
        {
            memset(setting_val, 0, sizeof(s_hilink_userdefined_setting_value_t));
            memcpy(setting_val, &hilink_user_defined_setting_default_value, sizeof(s_hilink_userdefined_setting_value_t));
            char *setting_val_str = __prepare_user_defined_setting_str(setting_val);
            if (setting_val_str)
            {
                if (EZPI_SUCCESS == EZPI_core_nvs_write_str(setting_val_str, strlen(setting_val_str), "userdef"))
                {
                    TRACE_I("Failed to write to nvs.");
                    ret = 1;
                }
                else
                {
                    ret = __setting_set_change_setting_template(CUSTOM_TEMPLATE, setting->cloud_properties.setting_id);
                }
            }
            else
            {
                ret = 1;
            }
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

static int __setting_reset_radar_distance_sensitivity_setting(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    cJSON *cj_params = (cJSON *)arg;
    if (cj_params && setting)
    {
        s_hilink_radar_distance_sensitivity_value_t *setting_val = (s_hilink_radar_distance_sensitivity_value_t *)setting->user_arg;
        if (setting_val)
        {
            setting_val->distance_sensitivity_value = 10;
            if (EZPI_SUCCESS != EZPI_core_nvs_write_int32(setting_val->distance_sensitivity_value, "rdrsens"))
            {
                TRACE_E("Failed to write to NVS");
                ret = 1;
            }
            else
            {
                ret = 0;
            }
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

static int __settings_reset(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    if (hilink_presence_sensor_setting_ids[0] == setting->cloud_properties.setting_id)
    {
        ESP_ERROR_CHECK(__setting_reset_pre_defined_setting(arg, setting));
    }
    if (hilink_presence_sensor_setting_ids[1] == setting->cloud_properties.setting_id)
    {
        ESP_ERROR_CHECK(__setting_reset_user_defined_setting(arg, setting));
    }
    if (hilink_presence_sensor_setting_ids[2] == setting->cloud_properties.setting_id)
    {
        ESP_ERROR_CHECK(__setting_reset_radar_distance_sensitivity_setting(arg, setting));
    }

    EZPI_core_device_value_updated_settings_broadcast(setting);

    return ret;
}
// ********************************************* hub.device.setting.reset related end ********************************************* //

// ********************************************* hub.device.setting.updated related start ********************************************* //
static int __setting_update_pre_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    if (setting)
    {
        cJSON *cj_params = (cJSON *)arg;
        s_hilink_predefined_setting_value_t *setting_value = (s_hilink_predefined_setting_value_t *)setting->user_arg;
        if (cj_params && setting_value)
        {
            cJSON_AddStringToObject(__FUNCTION__, cj_params, ezlopi_value_str, setting_value->setting_value);
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

static int __setting_update_user_defined_setting(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    cJSON *cj_params = (cJSON *)arg;
    s_hilink_userdefined_setting_value_t *setting_value = (s_hilink_userdefined_setting_value_t *)hilink_presence_sensor_user_defined_setting->user_arg;
    if (cj_params && setting && setting_value)
    {
        cJSON *cj_value = cJSON_AddObjectToObject(__FUNCTION__, cj_params, ezlopi_value_str);
        if (cj_value)
        {
            if (setting_value->is_active)
            {
                ESP_ERROR_CHECK(__prepare_user_defined_setting_cjson(cj_value, setting_value));
            }
            else
            {
                cJSON_AddBoolToObject(__FUNCTION__, cj_value, ezlopi_is_active_str, setting_value->is_active);
            }
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

static int __setting_update_radar_distance_sensitivity_setting(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    cJSON *cj_params = (cJSON *)arg;
    s_hilink_radar_distance_sensitivity_value_t *setting_value = (s_hilink_radar_distance_sensitivity_value_t *)setting->user_arg;
    if (cj_params && setting && setting_value)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cj_params, ezlopi_value_str, setting_value->distance_sensitivity_value);
    }
    else
    {
        ret = 1;
    }

    return ret;
}

static int __settings_update(void *arg, l_ezlopi_device_settings_v3_t *setting)
{
    int ret = 0;

    if (hilink_presence_sensor_setting_ids[0] == setting->cloud_properties.setting_id)
    {
        ESP_ERROR_CHECK(__setting_update_pre_defined_setting(arg, setting));
    }
    if (hilink_presence_sensor_setting_ids[1] == setting->cloud_properties.setting_id)
    {
        ESP_ERROR_CHECK(__setting_update_user_defined_setting(arg, setting));
    }
    if (hilink_presence_sensor_setting_ids[2] == setting->cloud_properties.setting_id)
    {
        ESP_ERROR_CHECK(__setting_update_radar_distance_sensitivity_setting(arg, setting));
    }

    return ret;
}
// ********************************************* hub.device.setting.updated related end ********************************************* //

static inline cJSON *__setting_add_text_and_lang_tag(const char *const object_text, const char *const object_lang_tag)
{

    cJSON *cj_object = cJSON_CreateObject(__FUNCTION__);
    if (cj_object)
    {
        cJSON_AddStringToObject(__FUNCTION__, cj_object, ezlopi_text_str, object_text);
        cJSON_AddStringToObject(__FUNCTION__, cj_object, ezlopi_lang_tag_str, object_lang_tag);
    }
    else
    {
        ezlopi_free(__FUNCTION__, cj_object);
        cj_object = NULL;
    }

    return cj_object;
}
static inline void __setting_extract_user_defined_setting(cJSON *cj_value, s_hilink_userdefined_setting_value_t *user_defined_setting_val)
{
    CJSON_GET_VALUE_FLOAT(cj_value, ezlopi_min_move_distance_str, user_defined_setting_val->min_move_distance);
    CJSON_GET_VALUE_FLOAT(cj_value, ezlopi_max_move_distance_str, user_defined_setting_val->max_move_distance);
    CJSON_GET_VALUE_FLOAT(cj_value, ezlopi_min_still_distance_str, user_defined_setting_val->min_still_distance);
    CJSON_GET_VALUE_FLOAT(cj_value, ezlopi_max_still_distance_str, user_defined_setting_val->max_still_distance);
    CJSON_GET_VALUE_UINT8(cj_value, ezlopi_timeout_str, user_defined_setting_val->timeout);
    CJSON_GET_VALUE_BOOL(cj_value, ezlopi_is_active_str, user_defined_setting_val->is_active);
}
static inline int __prepare_user_defined_setting_cjson(cJSON *cj_value, s_hilink_userdefined_setting_value_t *setting_val)
{
    int ret = 0;
    if (cj_value && setting_val)
    {
        cJSON_AddNumberToObject(__FUNCTION__, cj_value, ezlopi_min_move_distance_str, setting_val->min_move_distance);
        cJSON_AddNumberToObject(__FUNCTION__, cj_value, ezlopi_max_move_distance_str, setting_val->max_move_distance);
        cJSON_AddNumberToObject(__FUNCTION__, cj_value, ezlopi_min_still_distance_str, setting_val->min_still_distance);
        cJSON_AddNumberToObject(__FUNCTION__, cj_value, ezlopi_max_still_distance_str, setting_val->max_still_distance);
        cJSON_AddNumberToObject(__FUNCTION__, cj_value, ezlopi_timeout_str, setting_val->timeout);
        cJSON_AddBoolToObject(__FUNCTION__, cj_value, ezlopi_is_active_str, setting_val->is_active);
    }
    else
    {
        ret = 1;
    }
    return ret;
}
static inline char *__prepare_user_defined_setting_str(s_hilink_userdefined_setting_value_t *setting_val)
{
    char *ret = NULL;

    cJSON *cj_setting = cJSON_CreateObject(__FUNCTION__);
    if (cj_setting && setting_val)
    {
        ESP_ERROR_CHECK(__prepare_user_defined_setting_cjson(cj_setting, setting_val));
        ret = cJSON_PrintBuffered(__FUNCTION__, cj_setting, 1024, false);
        TRACE_D("length of 'ret': %d", strlen(ret));
    }

    return ret;
}
// Sensor's distance parameters are in the multiple of 0.75 which are encoded as enum in the driver.
// Starting from 0.75cm ranges goes up to 6.0cm, there are total of 8 different distance ranges.
static inline distance_t __setting_user_defined_setting_get_enum(float val)
{
    distance_t dist = DISTANCE_0CM;
    if (val == 0.75)
    {
        dist = DISTANCE_75CM;
    }
    else if (val == 1.5)
    {
        dist = DISTANCE_150CM;
    }
    else if (val == 2.25)
    {
        dist = DISTANCE_225CM;
    }
    else if (val == 3.0)
    {
        dist = DISTANCE_300CM;
    }
    else if (val == 3.75)
    {
        dist = DISTANCE_375CM;
    }
    else if (val == 4.5)
    {
        dist = DISTANCE_450CM;
    }
    else if (val == 5.25)
    {
        dist = DISTANCE_525CM;
    }
    else if (val == 6.0)
    {
        dist = DISTANCE_600CM;
    }
    else
    {
        dist = DISTANCE_0CM;
    }
    return dist;
}

/*******************************************************************************
 *                          End of File
 *******************************************************************************/