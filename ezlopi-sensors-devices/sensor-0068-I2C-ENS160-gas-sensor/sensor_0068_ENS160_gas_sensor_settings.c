
#include <stdlib.h>

#include "esp_err.h"

#include "ezlopi_core_cloud.h"
#include "ezlopi_core_nvs.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_cloud_settings.h"

#include "ezlopi_cloud_constants.h"

#include "ens160.h"
#include "sensor_0068_ENS160_gas_sensor_settings.h"

static const char* nvs_key_ens160_gas_sensor_ambient_temperature_setting = "enstemp";
static const char* nvs_key_ens160_gas_sensor_relative_humidity_setting = "enshmd";

static uint32_t ens160_gas_sensor_settings_ids[2] = { 0, 0 };

static bool setting_changed = false;

l_ezlopi_device_settings_v3_t* ens160_gas_sensor_ambient_temperature_sensor_setting;
l_ezlopi_device_settings_v3_t* ens160_gas_sensor_relative_humidity_sensor_setting;

static int __settings_callback(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3* setting, void* arg, void* user_arg);
static int __settings_get(void* arg, l_ezlopi_device_settings_v3_t* setting);
static int __settings_set(void* arg, l_ezlopi_device_settings_v3_t* setting);
static int __settings_reset(void* arg, l_ezlopi_device_settings_v3_t* setting);
static int __settings_update(void* arg, l_ezlopi_device_settings_v3_t* setting);


// ********************************************* Setting initialization related start ********************************************* //

static int __settings_sensor_0068_gas_sensor_setting_initialize_ambient_temperature_setting(l_ezlopi_device_t* device, void* user_arg)
{
    int ret = 0;

    if (device)
    {
        ens160_gas_sensor_settings_ids[0] = ezlopi_cloud_generate_settings_id();
        ens160_gas_sensor_ambient_temperature_sensor_setting = ezlopi_device_add_settings_to_device_v3(device, __settings_callback);
        if (ens160_gas_sensor_ambient_temperature_sensor_setting)
        {
            ens160_gas_sensor_ambient_temperature_sensor_setting->cloud_properties.setting_id = ens160_gas_sensor_settings_ids[0];
            s_sensor_ens160_gas_sensor_setting_ambient_temperature_t* ambient_temperature = (s_sensor_ens160_gas_sensor_setting_ambient_temperature_t*)malloc(sizeof(s_sensor_ens160_gas_sensor_setting_ambient_temperature_t));
            if (ambient_temperature)
            {
                memset(ambient_temperature, 0, sizeof(s_sensor_ens160_gas_sensor_setting_ambient_temperature_t));
                float read_value = 0;
                uint8_t error = ezlopi_nvs_read_float32(&read_value, nvs_key_ens160_gas_sensor_ambient_temperature_setting);
                if (1 == error)
                {
                    TRACE_D("Setting already exist");
                    ambient_temperature->ambient_temperature = read_value;
                }
                else
                {
                    TRACE_W("Not found saved setting for ambient temperature.");
                    ambient_temperature->ambient_temperature = 25;
                    if (!ezlopi_nvs_write_float32(ambient_temperature->ambient_temperature, nvs_key_ens160_gas_sensor_ambient_temperature_setting))
                    {
                        TRACE_E("Failed to write to NVS");
                        ret = 1;
                    }
                }
                ambient_temperature->user_arg = user_arg;
                ens160_gas_sensor_ambient_temperature_sensor_setting->user_arg = ambient_temperature;
            }
            else
            {
                free(ens160_gas_sensor_ambient_temperature_sensor_setting);
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

static int __settings_sensor_0068_gas_sensor_setting_initialize_relative_humidity_setting(l_ezlopi_device_t* device, void* user_arg)
{
    int ret = 0;

    if (device)
    {
        ens160_gas_sensor_settings_ids[1] = ezlopi_cloud_generate_settings_id();
        ens160_gas_sensor_relative_humidity_sensor_setting = ezlopi_device_add_settings_to_device_v3(device, __settings_callback);
        if (ens160_gas_sensor_relative_humidity_sensor_setting)
        {
            ens160_gas_sensor_relative_humidity_sensor_setting->cloud_properties.setting_id = ens160_gas_sensor_settings_ids[1];
            s_sensor_ens160_gas_sensor_setting_relative_humidity_t* relative_humidity = (s_sensor_ens160_gas_sensor_setting_relative_humidity_t*)malloc(sizeof(s_sensor_ens160_gas_sensor_setting_relative_humidity_t));
            if (relative_humidity)
            {
                memset(relative_humidity, 0, sizeof(s_sensor_ens160_gas_sensor_setting_relative_humidity_t));
                float read_value = 0;
                uint8_t error = ezlopi_nvs_read_float32(&read_value, nvs_key_ens160_gas_sensor_relative_humidity_setting);
                if (1 == error)
                {
                    TRACE_D("Setting already exist");
                    relative_humidity->relative_humidity = read_value;
                }
                else
                {
                    TRACE_W("Not found saved setting for ambient temperature.");
                    relative_humidity->relative_humidity = 50;
                    if (!ezlopi_nvs_write_float32(relative_humidity->relative_humidity, nvs_key_ens160_gas_sensor_relative_humidity_setting))
                    {
                        TRACE_E("Failed to write to NVS");
                        ret = 1;
                    }
                }
                relative_humidity->user_arg = user_arg;
                ens160_gas_sensor_relative_humidity_sensor_setting->user_arg = relative_humidity;
            }
            else
            {
                free(ens160_gas_sensor_relative_humidity_sensor_setting);
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

int sensor_0068_gas_sensor_settings_initialize(l_ezlopi_device_t* devices, void* user_arg)
{
    int ret = 0;

    ESP_ERROR_CHECK(__settings_sensor_0068_gas_sensor_setting_initialize_ambient_temperature_setting(devices, user_arg));
    ESP_ERROR_CHECK(__settings_sensor_0068_gas_sensor_setting_initialize_relative_humidity_setting(devices, user_arg));

    return ret;
}

static int __settings_callback(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3* setting, void* arg, void* user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_SETTINGS_ACTION_GET_SETTING:
    {
        TRACE_D("EZLOPI_SETTINGS_ACTION_GET_SETTING");
        __settings_get(arg, setting);
        break;
    }
    case EZLOPI_SETTINGS_ACTION_SET_SETTING:
    {
        TRACE_D("EZLOPI_SETTINGS_ACTION_SET_SETTING");
        __settings_set(arg, setting);
        break;
    }
    case EZLOPI_SETTINGS_ACTION_RESET_SETTING:
    {
        TRACE_D("EZLOPI_SETTINGS_ACTION_RESET_SETTING");
        __settings_reset(arg, setting);
        break;
    }
    case EZLOPI_SETTINGS_ACTION_UPDATE_SETTING:
    {
        TRACE_D("EZLOPI_SETTINGS_ACTION_UPDATE_SETTING");
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

// ********************************************* Setting initialization related end ********************************************* //

// ********************************************* Setting getter related start ********************************************* //

static int __settings_get_ens160_gas_sensor_ambient_temperature_get(void* arg, l_ezlopi_device_settings_v3_t* setting)
{
    int ret = 0;

    cJSON* cj_properties = (cJSON*)arg;
    s_sensor_ens160_gas_sensor_setting_ambient_temperature_t* ambient_temperature = (s_sensor_ens160_gas_sensor_setting_ambient_temperature_t*)setting->user_arg;
    if (cj_properties && ambient_temperature)
    {
        cJSON_AddItemToObject(cj_properties, "label", __setting_add_text_and_lang_tag(SENSOR_ENS160_GAS_SENSOR_SETTING_AMBIENT_TEMPERATURE_LABEL_TEXT, SENSOR_ENS160_GAS_SENSOR_SETTING_AMBIENT_TEMPERATURE_LABEL_LANG_TAG));
        cJSON_AddItemToObject(cj_properties, "description", __setting_add_text_and_lang_tag(SENSOR_ENS160_GAS_SENSOR_SETTING_AMBIENT_TEMPERATURE_DESCRIPTION_TEXT, SENSOR_ENS160_GAS_SENSOR_SETTING_AMBIENT_TEMPERATURE_DESCRIPTION_LANG_TAG));
        cJSON_AddStringToObject(cj_properties, "status", "synced");
        cJSON_AddStringToObject(cj_properties, "valueType", value_type_temperature);

        cJSON* value = cJSON_AddObjectToObject(cj_properties, "value");
        cJSON* valueDefault = cJSON_AddObjectToObject(cj_properties, "valueDefault");
        if (value && valueDefault)
        {
            cJSON_AddNumberToObject(value, "value", ambient_temperature->ambient_temperature);
            cJSON_AddStringToObject(value, "scale", scales_celsius);

            cJSON_AddNumberToObject(valueDefault, "value", 25);
            cJSON_AddStringToObject(valueDefault, "scale", scales_celsius);
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

static int __settings_get_ens160_gas_sensor_relative_humidity_get(void* arg, l_ezlopi_device_settings_v3_t* setting)
{
    int ret = 0;

    cJSON* cj_properties = (cJSON*)arg;
    s_sensor_ens160_gas_sensor_setting_relative_humidity_t* relative_humidity = (s_sensor_ens160_gas_sensor_setting_relative_humidity_t*)setting->user_arg;
    if (cj_properties && relative_humidity)
    {
        cJSON_AddItemToObject(cj_properties, "label", __setting_add_text_and_lang_tag(SENSOR_ENS160_GAS_SENSOR_SETTING_RELATIVE_HUMIDITY_LABEL_TEXT, SENSOR_ENS160_GAS_SENSOR_SETTING_RELATIVE_HUMIDITY_LABEL_LANG_TAG));
        cJSON_AddItemToObject(cj_properties, "description", __setting_add_text_and_lang_tag(SENSOR_ENS160_GAS_SENSOR_SETTING_RELATIVE_HUMIDITY_DESCRIPTION_TEXT, SENSOR_ENS160_GAS_SENSOR_SETTING_RELATIVE_HUMIDITY_DESCRIPTION_LANG_TAG));
        cJSON_AddStringToObject(cj_properties, "status", "synced");
        cJSON_AddStringToObject(cj_properties, "valueType", value_type_humidity);

        cJSON* value = cJSON_AddObjectToObject(cj_properties, "value");
        cJSON* valueDefault = cJSON_AddObjectToObject(cj_properties, "valueDefault");
        if (value && valueDefault)
        {
            cJSON_AddNumberToObject(value, "value", relative_humidity->relative_humidity);
            cJSON_AddStringToObject(value, "scale", scales_percent);

            cJSON_AddNumberToObject(valueDefault, "value", 50);
            cJSON_AddStringToObject(valueDefault, "scale", scales_percent);
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

static int __settings_get(void* arg, l_ezlopi_device_settings_v3_t* setting)
{
    int ret = ret = 0;
    if (setting->cloud_properties.setting_id == ens160_gas_sensor_settings_ids[0])
    {
        ESP_ERROR_CHECK(__settings_get_ens160_gas_sensor_ambient_temperature_get(arg, setting));
    }
    if (setting->cloud_properties.setting_id == ens160_gas_sensor_settings_ids[1])
    {
        ESP_ERROR_CHECK(__settings_get_ens160_gas_sensor_relative_humidity_get(arg, setting));
    }
    return ret;
}
// ********************************************* Setting getter related end ********************************************* //

// ********************************************* Setting setter related start ********************************************* //

static int __settings_set_ens160_gas_sensor_ambient_temperature_get(void* arg, l_ezlopi_device_settings_v3_t* setting)
{
    int ret = 0;

    cJSON* cj_properties = (cJSON*)arg;
    s_sensor_ens160_gas_sensor_setting_ambient_temperature_t* ambient_temperature = (s_sensor_ens160_gas_sensor_setting_ambient_temperature_t*)setting->user_arg;
    if (cj_properties && ambient_temperature)
    {
        CJSON_GET_VALUE_DOUBLE(cj_properties, "value", ambient_temperature->ambient_temperature);
        if (!ezlopi_nvs_write_float32(ambient_temperature->ambient_temperature, nvs_key_ens160_gas_sensor_ambient_temperature_setting))
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

static int __settings_set_ens160_gas_sensor_relative_humidity_get(void* arg, l_ezlopi_device_settings_v3_t* setting)
{
    int ret = 0;

    cJSON* cj_properties = (cJSON*)arg;
    s_sensor_ens160_gas_sensor_setting_relative_humidity_t* relative_humidity = (s_sensor_ens160_gas_sensor_setting_relative_humidity_t*)setting->user_arg;
    if (cj_properties && relative_humidity)
    {
        CJSON_GET_VALUE_DOUBLE(cj_properties, "value", relative_humidity->relative_humidity);
        if (!ezlopi_nvs_write_float32(relative_humidity->relative_humidity, nvs_key_ens160_gas_sensor_relative_humidity_setting))
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

static int __settings_set(void* arg, l_ezlopi_device_settings_v3_t* setting)
{

    int ret = ret = 0;
    if (setting->cloud_properties.setting_id == ens160_gas_sensor_settings_ids[0])
    {
        ESP_ERROR_CHECK(__settings_set_ens160_gas_sensor_ambient_temperature_get(arg, setting));
    }
    if (setting->cloud_properties.setting_id == ens160_gas_sensor_settings_ids[1])
    {
        ESP_ERROR_CHECK(__settings_set_ens160_gas_sensor_relative_humidity_get(arg, setting));
    }
    setting_changed = true;
    ezlopi_cloud_settings_updated_from_devices_v3(NULL, setting);
    return ret;

}

// ********************************************* Setting setter related end ********************************************* //



// ********************************************* Setting updater related start ********************************************* //


static int __settings_update_ens160_gas_sensor_ambient_temperature_get(void* arg, l_ezlopi_device_settings_v3_t* setting)
{
    int ret = 0;

    cJSON* cj_properties = (cJSON*)arg;
    s_sensor_ens160_gas_sensor_setting_ambient_temperature_t* ambient_temperature = (s_sensor_ens160_gas_sensor_setting_ambient_temperature_t*)setting->user_arg;
    if (cj_properties && ambient_temperature)
    {
        cJSON_AddNumberToObject(cj_properties, "value", ambient_temperature->ambient_temperature);
        cJSON_AddStringToObject(cj_properties, "scale", scales_celsius);
    }
    else
    {
        ret = 1;
    }
    return ret;
}

static int __settings_update_ens160_gas_sensor_relative_humidity_get(void* arg, l_ezlopi_device_settings_v3_t* setting)
{
    int ret = 0;

    cJSON* cj_properties = (cJSON*)arg;
    s_sensor_ens160_gas_sensor_setting_relative_humidity_t* relative_humidity = (s_sensor_ens160_gas_sensor_setting_relative_humidity_t*)setting->user_arg;
    if (cj_properties && relative_humidity)
    {
        cJSON_AddNumberToObject(cj_properties, "value", relative_humidity->relative_humidity);
        cJSON_AddStringToObject(cj_properties, "scale", scales_percent);
    }
    else
    {
        ret = 1;
    }

    return ret;
}

static int __settings_update(void* arg, l_ezlopi_device_settings_v3_t* setting)
{
    int ret = ret = 0;
    if (setting->cloud_properties.setting_id == ens160_gas_sensor_settings_ids[0])
    {
        ESP_ERROR_CHECK(__settings_update_ens160_gas_sensor_ambient_temperature_get(arg, setting));
    }
    if (setting->cloud_properties.setting_id == ens160_gas_sensor_settings_ids[1])
    {
        ESP_ERROR_CHECK(__settings_update_ens160_gas_sensor_relative_humidity_get(arg, setting));
    }
    return ret;
}

// ********************************************* Setting updater related end ********************************************* //


// ********************************************* Setting resetter related start ********************************************* //

static int __settings_reset_ens160_gas_sensor_ambient_temperature_get(void* arg, l_ezlopi_device_settings_v3_t* setting)
{
    int ret = 0;

    cJSON* cj_properties = (cJSON*)arg;
    s_sensor_ens160_gas_sensor_setting_ambient_temperature_t* ambient_temperature = (s_sensor_ens160_gas_sensor_setting_ambient_temperature_t*)setting->user_arg;
    if (cj_properties && ambient_temperature)
    {
        ambient_temperature->ambient_temperature = 25;
        if (!ezlopi_nvs_write_float32(ambient_temperature->ambient_temperature, nvs_key_ens160_gas_sensor_ambient_temperature_setting))
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

static int __settings_reset_ens160_gas_sensor_relative_humidity_get(void* arg, l_ezlopi_device_settings_v3_t* setting)
{
    int ret = 0;

    cJSON* cj_properties = (cJSON*)arg;
    s_sensor_ens160_gas_sensor_setting_relative_humidity_t* relative_humidity = (s_sensor_ens160_gas_sensor_setting_relative_humidity_t*)setting->user_arg;
    if (cj_properties && relative_humidity)
    {
        relative_humidity->relative_humidity = 50;
        if (!ezlopi_nvs_write_float32(relative_humidity->relative_humidity, nvs_key_ens160_gas_sensor_relative_humidity_setting))
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

static int __settings_reset(void* arg, l_ezlopi_device_settings_v3_t* setting)
{

    int ret = ret = 0;
    if (setting->cloud_properties.setting_id == ens160_gas_sensor_settings_ids[0])
    {
        ESP_ERROR_CHECK(__settings_reset_ens160_gas_sensor_ambient_temperature_get(arg, setting));
    }
    if (setting->cloud_properties.setting_id == ens160_gas_sensor_settings_ids[1])
    {
        ESP_ERROR_CHECK(__settings_reset_ens160_gas_sensor_relative_humidity_get(arg, setting));
    }
    setting_changed = true;
    ezlopi_cloud_settings_updated_from_devices_v3(NULL, setting);
    return ret;

}

// ********************************************* Setting resetter related end ********************************************* //


float get_ambient_temperature_setting()
{
    float ret = 0.0f;
    s_sensor_ens160_gas_sensor_setting_ambient_temperature_t* ambient_temperature = (s_sensor_ens160_gas_sensor_setting_ambient_temperature_t*)ens160_gas_sensor_ambient_temperature_sensor_setting;
    if (ambient_temperature)
    {
        ret = ambient_temperature->ambient_temperature;
    }
    return ret;
}
float get_relative_humidity_setting()
{
    float ret = 0.0f;
    s_sensor_ens160_gas_sensor_setting_relative_humidity_t* relative_humidity = (s_sensor_ens160_gas_sensor_setting_relative_humidity_t*)ens160_gas_sensor_relative_humidity_sensor_setting;
    if (relative_humidity)
    {
        ret = relative_humidity->relative_humidity;
    }
    return ret;
}

bool has_setting_changed()
{
    return setting_changed;
}

void set_setting_changed_to_false()
{
    setting_changed = false;
}
