
#ifndef _HILINK_PRESENCE_SENSOR_SETTINGS_H_
#define _HILINK_PRESENCE_SENSOR_SETTINGS_H_

#include "ezlopi_util_trace.h"
#include "cjext.h"
#include "ezlopi_core_devices.h"

#define HILINK_PRESENCE_SENSOR_SETTING_PREDEFINED_LABEL_TEXT "Pre-defined operation mode"
#define HILINK_PRESENCE_SENSOR_SETTING_PREDEFINED_DESCRIPTION_TEXT "Pre-Defined Operation mode, there will be different operation mode for different use cases"
#define HILINK_PRESENCE_SENSOR_SETTING_USER_DEFINED_LABEL_TEXT "User defined mode"
#define HILINK_PRESENCE_SENSOR_SETTING_USER_DEFINED_DESCRIPTION_TEXT "User defined operation mode, where user can set several distance parameters for setting a custom operation mode"
#define HILINK_PRESENCE_SENSOR_SETTING_RADAR_DISTANCE_SENSITIVITY_LABEL_TEXT "Radar Distance Sensitivity"
#define HILINK_PRESENCE_SENSOR_SETTING_RADAR_DISTANCE_SENSITIVITY_DESCRIPTION_TEXT "Presence Sensor Pro device will send update to its cloud when the distance change by an object is more than the sensitivity value set."

#define SLEEP_MODE_CLOSE_TEXT "Sleep mode close range"
#define SLEEP_MODE_MID_TEXT "Sleep mode mid range"
#define SLEEP_MODE_LONG_TEXT "Sleep mode long range"
#define MOVEMENT_MODE_CLOSE_TEXT "Movement mode close range"
#define MOVEMENT_MODE_MID_TEXT "Movement mode mid range"
#define MOVEMENT_MODE_LONG_TEXT "Movement mode long range"
#define BASIC_ROUTINE_MODE_CLOSE_TEXT "Basic mode close range"
#define BASIC_ROUTINE_MODE_MID_TEXT "Basic mode mid range"
#define BASIC_ROUTINE_MODE_LONG_TEXT "Basic mode long range"
#define USER_DEFINED_MODE_TEXT "User defined mode, when selected means this setting is not used, instead \"User defined mode\" setting is in use"

#define HILINK_PRESENCE_SENSOR_SETTING_PREDEFINED_LABEL_LANG_TAG "ezlopi_presence_predefined_operation_mode_label"
#define HILINK_PRESENCE_SENSOR_SETTING_PREDEFINED_DESCRIPTION_LANG_TAG "ezlopi_presence_predefined_operation_mode_description"
#define HILINK_PRESENCE_SENSOR_SETTING_USER_DEFINED_LABEL_LANG_TAG "ezlopi_presence_user_defined_mode_label"
#define HILINK_PRESENCE_SENSOR_SETTING_USER_DEFINED_DESCRIPTION_LANG_TAG "ezlopi_presence_user_defined_mode_description"
#define HILINK_PRESENCE_SENSOR_SETTING_RADAR_DISTANCE_SENSITIVITY_LABEL_LANG_TAG "ezlopi_presence_radar_distance_sensitivity_setting_label"
#define HILINK_PRESENCE_SENSOR_SETTING_RADAR_DISTANCE_SENSITIVITY_DESCRIPTION_LANG_TAG "ezlopi_presence_radar_distance_sensitivity_setting_description"

#define SLEEP_MODE_CLOSE_LANG_TAG "ezlopi_presence_sleep_close"
#define SLEEP_MODE_MID_LANG_TAG "ezlopi_presence_sleep_mid"
#define SLEEP_MODE_LONG_LANG_TAG "ezlopi_presence_sleep_long"
#define MOVEMENT_MODE_CLOSE_LANG_TAG "ezlopi_presence_movement_close"
#define MOVEMENT_MODE_MID_LANG_TAG "ezlopi_presence_movement_mid"
#define MOVEMENT_MODE_LONG_LANG_TAG "ezlopi_presence_movement_long"
#define BASIC_ROUTINE_MODE_CLOSE_LANG_TAG "ezlopi_presence_basic_close"
#define BASIC_ROUTINE_MODE_MID_LANG_TAG "ezlopi_presence_basic_mid"
#define BASIC_ROUTINE_MODE_LONG_LANG_TAG "ezlopi_presence_basic_long"
#define USER_DEFINED_MODE_LANG_TAG "ezlopi_presence_user_defined_mode"

typedef struct s_hilink_predefined_setting_value
{
    char setting_value[50];
} s_hilink_predefined_setting_value_t;

typedef struct s_hilink_userdefined_setting_value
{
    float min_move_distance;
    float max_move_distance;
    float min_still_distance;
    float max_still_distance;
    uint8_t timeout;
    bool is_active;
} s_hilink_userdefined_setting_value_t;

typedef struct s_hilink_radar_distance_sensitivity_value
{
    int distance_sensitivity_value;
} s_hilink_radar_distance_sensitivity_value_t;

int hilink_presence_sensor_initialize_settings(l_ezlopi_device_t* device);
int hilink_presence_sensor_apply_settings();
bool hilink_presence_sensor_target_in_detectable_range(const uint16_t moving_target_distance);

static inline cJSON* __setting_add_text_and_lang_tag(const char* const object_text, const char* const object_lang_tag)
{

    cJSON* cj_object = cJSON_CreateObject();
    if (cj_object)
    {
        cJSON_AddStringToObject(cj_object, ezlopi_text_str, object_text);
        cJSON_AddStringToObject(cj_object, ezlopi_lang_tag_str, object_lang_tag);
    }
    else
    {
        free(cj_object);
        cj_object = NULL;
    }

    return cj_object;
}

static inline void __setting_extract_user_defined_setting(cJSON* cj_value, s_hilink_userdefined_setting_value_t* user_defined_setting_val)
{
    CJSON_GET_VALUE_DOUBLE(cj_value, ezlopi_min_move_distance_str, user_defined_setting_val->min_move_distance);
    CJSON_GET_VALUE_DOUBLE(cj_value, ezlopi_max_move_distance_str, user_defined_setting_val->max_move_distance);
    CJSON_GET_VALUE_DOUBLE(cj_value, ezlopi_min_still_distance_str, user_defined_setting_val->min_still_distance);
    CJSON_GET_VALUE_DOUBLE(cj_value, ezlopi_max_still_distance_str, user_defined_setting_val->max_still_distance);
    CJSON_GET_VALUE_DOUBLE(cj_value, ezlopi_timeout_str, user_defined_setting_val->timeout);
    CJSON_GET_VALUE_DOUBLE(cj_value, ezlopi_is_active_str, user_defined_setting_val->is_active);
}

static inline int __prepare_user_defined_setting_cjson(cJSON* cj_value, s_hilink_userdefined_setting_value_t* setting_val)
{
    int ret = 0;
    if (cj_value && setting_val)
    {
        cJSON_AddNumberToObject(cj_value, ezlopi_min_move_distance_str, setting_val->min_move_distance);
        cJSON_AddNumberToObject(cj_value, ezlopi_max_move_distance_str, setting_val->max_move_distance);
        cJSON_AddNumberToObject(cj_value, ezlopi_min_still_distance_str, setting_val->min_still_distance);
        cJSON_AddNumberToObject(cj_value, ezlopi_max_still_distance_str, setting_val->max_still_distance);
        cJSON_AddNumberToObject(cj_value, ezlopi_timeout_str, setting_val->timeout);
        cJSON_AddBoolToObject(cj_value, ezlopi_is_active_str, setting_val->is_active);
    }
    else
    {
        ret = 1;
    }
    return ret;
}

static inline char* __prepare_user_defined_setting_str(s_hilink_userdefined_setting_value_t* setting_val)
{
    char* ret = NULL;

    cJSON* cj_setting = cJSON_CreateObject();
    if (cj_setting && setting_val)
    {
        ESP_ERROR_CHECK(__prepare_user_defined_setting_cjson(cj_setting, setting_val));
        ret = cJSON_PrintBuffered(cj_setting, 1024, false);
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

#endif // _HILINK_PRESENCE_SENSOR_SETTINGS_H_
