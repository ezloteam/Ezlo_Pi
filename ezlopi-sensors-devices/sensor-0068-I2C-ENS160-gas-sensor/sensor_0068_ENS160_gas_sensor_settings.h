

#ifndef _SENSOR_ENS160_GAS_SENSOR_SETTINGS_H_
#define _SENSOR_ENS160_GAS_SENSOR_SETTINGS_H_

#include "cjext.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_devices.h"


#define SENSOR_ENS160_GAS_SENSOR_SETTING_AMBIENT_TEMPERATURE_LABEL_TEXT "Set ENS160 gas sensor ambient temperature"
#define SENSOR_ENS160_GAS_SENSOR_SETTING_AMBIENT_TEMPERATURE_DESCRIPTION_TEXT "Setter for ENS160 gas sensor ambient temperature value. Sensor can operate on different surroundings depending on the ambeint temperature"
#define SENSOR_ENS160_GAS_SENSOR_SETTING_RELATIVE_HUMIDITY_LABEL_TEXT "Set ENS160 gas sensor relative humidity"
#define SENSOR_ENS160_GAS_SENSOR_SETTING_RELATIVE_HUMIDITY_DESCRIPTION_TEXT "Setter for ENS160 gas sensor relative humidity value. Sensor can operate on different surroundings depending on the relative humidity"

#define SENSOR_ENS160_GAS_SENSOR_SETTING_AMBIENT_TEMPERATURE_LABEL_LANG_TAG "ens160_gas_sensor_ambient_temperature_setting_label"
#define SENSOR_ENS160_GAS_SENSOR_SETTING_AMBIENT_TEMPERATURE_DESCRIPTION_LANG_TAG "ens160_gas_sensor_ambient_temperature_setting_description"
#define SENSOR_ENS160_GAS_SENSOR_SETTING_RELATIVE_HUMIDITY_LABEL_LANG_TAG "ens160_gas_sensor_relative_humidity_setting_label"
#define SENSOR_ENS160_GAS_SENSOR_SETTING_RELATIVE_HUMIDITY_DESCRIPTION_LANG_TAG "ens160_gas_sensor_relative_humidity_setting_description"

typedef struct s_sensor_ens160_gas_sensor_setting_ambient_temperature
{
    float ambient_temperature;
    void* user_arg;
}s_sensor_ens160_gas_sensor_setting_ambient_temperature_t;

typedef struct s_sensor_ens160_gas_sensor_setting_relative_humidity
{
    float relative_humidity;
    void* user_arg;
}s_sensor_ens160_gas_sensor_setting_relative_humidity_t;

int sensor_0068_gas_sensor_settings_initialize(l_ezlopi_device_t* devices, void* user_arg);
float get_ambient_temperature_setting();
float get_relative_humidity_setting();
bool has_setting_changed();
void set_setting_changed_to_false();

static inline cJSON* __setting_add_text_and_lang_tag(const char* const object_text, const char* const object_lang_tag)
{

    cJSON* cj_object = cJSON_CreateObject(__FUNCTION__);
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


#endif // _SENSOR_ENS160_GAS_SENSOR_SETTINGS_H_


