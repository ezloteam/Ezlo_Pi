#include <string.h>
#include "stdint.h"
#include "settings.h"
#include "trace.h"

#include "cJSON.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_cloud_methods_str.h"
#include "ezlopi_devices_list.h"
#include "web_provisioning.h"

void ezlopi_device_settings_list_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_settings_array = cJSON_AddArrayToObject(cj_result, "settings");
        if (cj_settings_array)
        {
            l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
            while (curr_device)
            {
                l_ezlopi_device_settings_v3_t *curr_setting = curr_device->settings;
                while (curr_setting)
                {
                    cJSON *cj_properties = cJSON_CreateObject();
                    if (cj_properties)
                    {
                        char tmp_string[64];
                        snprintf(tmp_string, sizeof(tmp_string), "%08x", curr_setting->cloud_properties.setting_id);
                        cJSON_AddStringToObject(cj_properties, "_id", tmp_string);
                        snprintf(tmp_string, sizeof(tmp_string), "%08x", curr_device->cloud_properties.device_id);
                        cJSON_AddStringToObject(cj_properties, "deviceId", tmp_string);
                        cJSON_AddStringToObject(cj_properties, "status", "synced");
                        curr_setting->func(EZLOPI_SETTINGS_ACTION_GET_SETTING, curr_setting, cj_properties, curr_setting->user_arg);
                        if (!cJSON_AddItemToArray(cj_settings_array, cj_properties))
                        {
                            cJSON_Delete(cj_properties);
                        }
                    }
                    curr_setting = curr_setting->next;
                }
                curr_device = curr_device->next;
            }
        }
    }
}
void ezlopi_device_settings_value_set_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        char *setting_id_str = 0;
        CJSON_GET_VALUE_STRING(cj_params, ezlopi__id_str, setting_id_str);
        uint32_t setting_id = strtoul(setting_id_str, NULL, 16);

        l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
        uint32_t found_setting = 0;
        while (curr_device)
        {
            l_ezlopi_device_settings_v3_t *curr_setting = curr_device->settings;
            while (curr_setting)
            {
                if (setting_id == curr_setting->cloud_properties.setting_id)
                {
                    curr_setting->func(EZLOPI_SETTINGS_ACTION_SET_SETTING, curr_setting, cj_params, curr_setting->user_arg);
                    found_setting = 1;
                    break;
                }
                curr_setting = curr_setting->next;
            }
            if (found_setting)
            {
                break;
            }
            curr_device = curr_device->next;
        }
    }
}
void ezlopi_device_settings_reset_v3(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        l_ezlopi_device_t *curr_device = ezlopi_device_get_head();
        uint32_t found_setting = 0;
        while (curr_device)
        {
            if (cJSON_HasObjectItem(cj_params, ezlopi_device_id_str))
            {
                char *device_id_str = 0;
                CJSON_GET_VALUE_STRING(cj_params, ezlopi_device_id_str, device_id_str);
                int device_id = strtol(device_id_str, NULL, 16);
                TRACE_E("device_id: %X", device_id);
                if (device_id == curr_device->cloud_properties.device_id)
                {
                    l_ezlopi_device_settings_v3_t *curr_setting = curr_device->settings;
                    while (curr_setting)
                    {
                        curr_setting->func(EZLOPI_SETTINGS_ACTION_RESET_SETTING, curr_setting, cj_params, curr_setting->user_arg);
                        curr_setting = curr_setting->next;
                    }
                }
            }
            else if (cJSON_HasObjectItem(cj_params, ezlopi__id_str))
            {

                char *setting_id_str = 0;
                CJSON_GET_VALUE_STRING(cj_params, ezlopi__id_str, setting_id_str);
                uint32_t setting_id = strtol(setting_id_str, NULL, 16);
                TRACE_E("setting_id: %X", setting_id);
                l_ezlopi_device_settings_v3_t *curr_setting = curr_device->settings;
                while (curr_setting)
                {
                    if (setting_id == curr_setting->cloud_properties.setting_id)
                    {
                        curr_setting->func(EZLOPI_SETTINGS_ACTION_RESET_SETTING, curr_setting, cj_params, curr_setting->user_arg);
                    }
                    curr_setting = curr_setting->next;
                }
            }
            else
            {
                TRACE_E("ID not found !");
            }
            curr_device = curr_device->next;
        }
    }
}

cJSON *ezlopi_cloud_settings_updated_from_devices_v3(l_ezlopi_device_t *device, l_ezlopi_device_settings_v3_t *setting)
{
    cJSON *cjson_response = cJSON_CreateObject();
    if (cjson_response)
    {
        if (NULL != setting)
        {
            cJSON_AddStringToObject(cjson_response, ezlopi_msg_subclass_str, method_hub_device_setting_updated);
            cJSON_AddNumberToObject(cjson_response, ezlopi_msg_id_str, web_provisioning_get_message_count());
            cJSON_AddStringToObject(cjson_response, ezlopi_id_str, ezlopi_ui_broadcast_str);

            cJSON *cj_result = cJSON_AddObjectToObject(cjson_response, ezlopi_result_str);
            if (cj_result)
            {
                char tmp_string[64];
                snprintf(tmp_string, sizeof(tmp_string), "%08x", setting->cloud_properties.setting_id);
                cJSON_AddStringToObject(cj_result, "_id", tmp_string);
                setting->func(EZLOPI_SETTINGS_ACTION_UPDATE_SETTING, setting, cj_result, setting->user_arg);
            }
        }
        else
        {
            cJSON_Delete(cjson_response);
            cjson_response = NULL;
        }
    }

    return cjson_response;
}

void ezlopi_device_settings_list(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_settings_array = cJSON_AddArrayToObject(cj_result, "settings");
        if (cj_settings_array)
        {
            l_ezlopi_device_settings_t *registered_settings = ezlopi_devices_settings_get_list();

            ezlopi_device_settings_print_settings(registered_settings);

            while (NULL != registered_settings)
            {
                if (NULL != registered_settings->properties)
                {
                    cJSON *cj_properties = cJSON_CreateObject();
                    if (cj_properties)
                    {
                        char tmp_string[64];
                        snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_settings->properties->id);
                        cJSON_AddStringToObject(cj_properties, "_id", tmp_string);
                        snprintf(tmp_string, sizeof(tmp_string), "%08x", registered_settings->properties->device_id);
                        cJSON_AddStringToObject(cj_properties, "deviceId", tmp_string);
                        cJSON_AddStringToObject(cj_properties, "label", registered_settings->properties->label);
                        cJSON_AddStringToObject(cj_properties, "description", registered_settings->properties->description);
                        cJSON_AddStringToObject(cj_properties, "status", "synced");
                        cJSON_AddStringToObject(cj_properties, "valueType", registered_settings->properties->value_type);

                        registered_settings->properties->__settings_call(EZLOPI_SETTINGS_ACTION_GET_SETTING, registered_settings->properties, cj_properties, NULL);
                        if (!cJSON_AddItemToArray(cj_settings_array, cj_properties))
                        {
                            cJSON_Delete(cj_properties);
                        }
                    }
                }

                registered_settings = registered_settings->next;
            }
        }
    }
}

void ezlopi_device_settings_value_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        char *settings_id_str = 0;
        CJSON_GET_VALUE_STRING(cj_params, ezlopi__id_str, settings_id_str);
        uint32_t settings_id = strtoul(settings_id_str, NULL, 16);

        l_ezlopi_device_settings_t *registered_settings = ezlopi_devices_settings_get_list();

        while (NULL != registered_settings)
        {
            if (registered_settings->properties)
            {
                if (settings_id == registered_settings->properties->id)
                {
                    _ezlopi_device_settings_value_set(settings_id, cj_params);
                }
            }
            registered_settings = registered_settings->next;
        }
    }
}

void ezlopi_device_settings_reset(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));
    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {

        l_ezlopi_device_settings_t *registered_settings = ezlopi_devices_settings_get_list();
        while (NULL != registered_settings)
        {
            if (registered_settings->properties)
            {

                if (cJSON_HasObjectItem(cj_params, ezlopi__id_str))
                {

                    char *settings_id_str = 0;
                    CJSON_GET_VALUE_STRING(cj_params, ezlopi__id_str, settings_id_str);
                    int settings_id = strtol(settings_id_str, NULL, 16);
                    // TRACE_I("settings_id_str: %X", settings_id);

                    if (settings_id == registered_settings->properties->id)
                    {
                        _ezlopi_device_settings_reset_settings_id(settings_id);
                    }
                }
                else if (cJSON_HasObjectItem(cj_params, ezlopi_device_id_str))
                {
                    char *device_id_str = 0;
                    CJSON_GET_VALUE_STRING(cj_params, ezlopi_device_id_str, device_id_str);
                    int device_id = strtol(device_id_str, NULL, 16);
                    // TRACE_I("device_id_str: %X", device_id);

                    if (device_id == registered_settings->properties->device_id)
                    {
                        _ezlopi_device_settings_reset_device_id(device_id);
                    }
                }
            }

            registered_settings = registered_settings->next;
        }
    }
}

#if 0 // Hub settings is disable as it is not used, instead hub.device.settings is used.
void ezlopi_settings_list(cJSON *cj_request, cJSON *cj_response)
{

    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_result = cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
    if (cj_result)
    {
        cJSON *cj_settings = cJSON_AddArrayToObject(cj_result, "settings");
        if (cj_settings)
        {

            // Retrieve the settings list
            s_ezlopi_hub_settings_t *settings_list = ezlopi_settings_get_settings_list();

            // Calculate the number of settings
            uint16_t num_settings = ezlopi_settings_get_settings_count();

            // Add each setting to the "settings" array
            for (uint16_t i = 0; i < num_settings; i++)
            {
                cJSON *cj_setting = cJSON_CreateObject();
                cJSON_AddItemToArray(cj_settings, cj_setting);

                // Add setting properties to the setting object
                cJSON_AddStringToObject(cj_setting, "name", settings_list[i].name);

                // Determine the value type and add the corresponding value
                switch (settings_list[i].value_type)
                {
                case EZLOPI_SETTINGS_TYPE_TOKEN:
                    cJSON_AddStringToObject(cj_setting, "value", settings_list[i].value.token_value);
                    cJSON_AddStringToObject(cj_setting, "valueType", "token");

                    // Create the "enum" array
                    cJSON *cj_enum = cJSON_CreateArray();
                    cJSON_AddItemToObject(cj_setting, "enum", cj_enum);

                    // Add enum values to the "enum" array
                    for (uint16_t j = 0; j < EZLOPI_SETTINGS_MAX_ENUM_VALUES; j++)
                    {
                        const char *enum_value = settings_list[i].enum_values[j];
                        if (enum_value != NULL)
                        {
                            cJSON_AddItemToArray(cj_enum, cJSON_CreateString(enum_value));
                        }
                        else
                        {
                            break;
                        }
                    }
                    break;
                case EZLOPI_SETTINGS_TYPE_BOOL:
                    cJSON_AddBoolToObject(cj_setting, "value", settings_list[i].value.bool_value);
                    cJSON_AddStringToObject(cj_setting, "valueType", "bool");
                    break;
                case EZLOPI_SETTINGS_TYPE_INT:
                    cJSON_AddNumberToObject(cj_setting, "value", settings_list[i].value.int_value);
                    cJSON_AddStringToObject(cj_setting, "valueType", "int");
                    break;
                default:
                    break;
                }
            }
        }
    }
}

void ezlopi_settings_value_set(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON *cj_params = cJSON_GetObjectItem(cj_request, "params");
    if (cj_params)
    {
        char *settings_name = NULL;

        CJSON_GET_VALUE_STRING(cj_params, ezlopi_name_str, settings_name);
        cJSON *key_settings_value = cJSON_GetObjectItem(cj_params, ezlopi_value_str);

        if (cJSON_IsBool(key_settings_value))
        {
            bool settings_val_bool;
            cJSON_bool _settings_val_bool = cJSON_IsTrue(key_settings_value);
            settings_val_bool = _settings_val_bool ? true : false;
            ezlopi_settings_modify_setting(settings_name, &settings_val_bool);
        }
        else if (cJSON_IsNumber(key_settings_value))
        {
            int settings_val_int = (int)cJSON_GetNumberValue(key_settings_value);
            ezlopi_settings_modify_setting(settings_name, &settings_val_int);
        }
        else if (cJSON_IsString(key_settings_value))
        {
            char *settings_val_str = cJSON_GetStringValue(key_settings_value);
            TRACE_W("settings_val_str: %s", settings_val_str);
            ezlopi_settings_modify_setting(settings_name, settings_val_str);
        }
        else
        {
        }
    }
}

void ezlopi_settings_value_set_response(cJSON *cj_request, cJSON *cj_response)
{
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_id_str, cJSON_GetObjectItem(cj_request, ezlopi_id_str));
    cJSON_AddItemReferenceToObject(cj_response, ezlopi_key_method_str, cJSON_GetObjectItem(cj_request, ezlopi_key_method_str));

    cJSON_AddNullToObject(cj_response, ezlopi_error_str);

    cJSON_AddObjectToObject(cj_response, ezlopi_result_str);
}
#endif