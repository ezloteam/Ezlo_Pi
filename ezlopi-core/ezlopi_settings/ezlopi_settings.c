#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "cJSON.h"
#include "trace.h"
#include "ezlopi_nvs.h"
#include "ezlopi_cloud_keywords.h"
#include "ezlopi_settings.h"

static l_ezlopi_device_settings_t *configured_settings_head = NULL;
static l_ezlopi_device_settings_t *ezlopi_device_settings_list_create(s_ezlopi_device_settings_properties_t *properties, void *user_arg);

l_ezlopi_device_settings_t *ezlopi_devices_settings_get_list(void)
{
    return configured_settings_head;
}

int ezlopi_device_setting_add(s_ezlopi_device_settings_properties_t *properties, void *user_arg)
{
    int ret = 0;
    if (configured_settings_head)
    {
        l_ezlopi_device_settings_t *current_settings = configured_settings_head;

        while (NULL != current_settings->next)
        {
            current_settings = current_settings->next;
        }

        current_settings->next = ezlopi_device_settings_list_create(properties, user_arg);
        if (current_settings->next)
        {
            ret = 1;
        }
    }
    else
    {
        configured_settings_head = ezlopi_device_settings_list_create(properties, user_arg);
        if (configured_settings_head)
        {
            ret = 1;
        }
    }

    return ret;
}

static l_ezlopi_device_settings_t *ezlopi_device_settings_list_create(s_ezlopi_device_settings_properties_t *properties, void *user_arg)
{
    l_ezlopi_device_settings_t *settings_device_list = (l_ezlopi_device_settings_t *)malloc(sizeof(l_ezlopi_device_settings_t));
    if (settings_device_list)
    {
        memset(settings_device_list, 0, sizeof(l_ezlopi_device_settings_t));
        settings_device_list->properties = properties;
        settings_device_list->next = NULL;

        if (user_arg)
        {
            settings_device_list->user_arg = user_arg;
        }
    }
    else
    {
    }
    return settings_device_list;
}

#if 0
l_ezlopi_device_settings_t *ezlopi_device_add_settings_to_device(l_ezlopi_device_t *device, f_settings_call_t settings_callback)
{
    l_ezlopi_device_settings_t *new_setting = NULL;
    if (device)
    {
        new_setting = malloc(sizeof(l_ezlopi_device_settings_t));
        if (new_setting)
        {
            memset(new_item, 0, sizeof(l_ezlopi_device_settings_t));
            new_setting.;

            if (NULL == device->items)
            {
                device->items = new_item;
            }
            else
            {
                l_ezlopi_item_t *curr_item = device->items;
                while (curr_item->next)
                {
                    curr_item = curr_item->next;
                }

                curr_item->next = new_item;
            }
        }
    }

    return new_item;
}
#endif

void _ezlopi_device_settings_value_set(uint32_t id, void *args)
{
    int ret = 0;
    cJSON *cjson_params = (cJSON *)args;
    if (NULL != cjson_params)
    {

        l_ezlopi_device_settings_t *settings_current = configured_settings_head;

        while (NULL != settings_current)
        {
            if (NULL != settings_current->properties)
            {
                if (id == settings_current->properties->id)
                {
                    if (strcmp(settings_current->properties->value_type, "action") == 0)
                    {
                    }
                    else if (strcmp(settings_current->properties->value_type, "bool") == 0)
                    {

                        cJSON *value = cJSON_GetObjectItem(cjson_params, ezlopi_value_str);
                        if (value)
                        {
                            if (cJSON_IsTrue(value))
                            {
                                settings_current->properties->value.bool_value = true;
                            }
                            else
                            {
                                settings_current->properties->value.bool_value = false;
                            }
                            if (settings_current->properties->value_nonvolatile == true)
                            {
                                if (1 == ezlopi_nvs_write_bool(settings_current->properties->value.bool_value, settings_current->properties->nvs_alias))
                                {
                                    settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, settings_current->properties, NULL, NULL);
                                }
                                else
                                {
                                    TRACE_E("Error writing settings to NVS");
                                }
                            }
                            else
                            {
                                settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, settings_current->properties, NULL, NULL);
                            }
                        }
                        else
                        {
                            TRACE_E("Error parsing JSON, settings update failed !");
                        }
                    }
                    else if (strcmp(settings_current->properties->value_type, "int") == 0)
                    {
                        cJSON *value = cJSON_GetObjectItem(cjson_params, ezlopi_value_str);
                        if (value)
                        {
                            settings_current->properties->value.int_value = value->valueint;
                            if (settings_current->properties->value_nonvolatile == true)
                            {
                                if (1 == ezlopi_nvs_write_int32(settings_current->properties->value.int_value, settings_current->properties->nvs_alias))
                                {
                                    settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, settings_current->properties, NULL, NULL);
                                }
                                else
                                {
                                    TRACE_E("Error writing settings to NVS");
                                }
                            }
                            else
                            {
                                settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, settings_current->properties, NULL, NULL);
                            }
                        }
                        else
                        {
                            TRACE_E("Error parsing JSON, settings update failed !");
                        }
                    }
                    else if (strcmp(settings_current->properties->value_type, "token") == 0)
                    {
                        cJSON *o_item = cJSON_GetObjectItem(cjson_params, ezlopi_value_str);
                        if (o_item && o_item->valuestring)
                        {
                            settings_current->properties->value.token_value = malloc(strlen(o_item->valuestring) + 1);
                            if (settings_current->properties->value.token_value)
                            {
                                strncpy(settings_current->properties->value.token_value, o_item->valuestring, strlen(o_item->valuestring) + 1);
                                if (settings_current->properties->value_nonvolatile == true)
                                {
                                    if (1 == ezlopi_nvs_write_str(settings_current->properties->value.token_value, strlen(settings_current->properties->value.token_value), settings_current->properties->nvs_alias))
                                    {
                                        settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, settings_current->properties, NULL, NULL);
                                    }
                                    else
                                    {
                                        TRACE_E("Error writing settings to NVS");
                                    }
                                }
                                else
                                {
                                    settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, settings_current->properties, NULL, NULL);
                                }
                            }
                            else
                            {
                                TRACE_E("Error memory allocation, settings update failed !");
                            }
                        }
                        else
                        {
                            TRACE_E("Error parsing JSON, settings update failed !");
                        }
                    }
                    else if (strcmp(settings_current->properties->value_type, "string") == 0)
                    {
                        cJSON *o_item = cJSON_GetObjectItem(cjson_params, ezlopi_value_str);
                        if (o_item && o_item->valuestring)
                        {
                            settings_current->properties->value.string_value = malloc(strlen(o_item->valuestring) + 1);
                            if (settings_current->properties->value.string_value)
                            {
                                strncpy(settings_current->properties->value.string_value, o_item->valuestring, strlen(o_item->valuestring) + 1);
                                if (settings_current->properties->value_nonvolatile == true)
                                {
                                    if (1 == ezlopi_nvs_write_str(settings_current->properties->value.string_value, strlen(settings_current->properties->value.string_value), settings_current->properties->nvs_alias))
                                    {
                                        settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, settings_current->properties, NULL, NULL);
                                    }
                                    else
                                    {
                                        TRACE_E("Error writing settings to NVS");
                                    }
                                }
                                else
                                {
                                    settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, settings_current->properties, NULL, NULL);
                                }
                            }
                            else
                            {
                                TRACE_E("Error memory allocation, settings update failed !");
                            }
                        }
                        else
                        {
                            TRACE_E("Error parsing JSON, settings update failed !");
                        }
                    }
                    else if (strcmp(settings_current->properties->value_type, "rgb") == 0)
                    {
                    }
                    else if (strcmp(settings_current->properties->value_type, "scalable") == 0)
                    {
                        cJSON *cJSON_value_root = cJSON_GetObjectItem(cjson_params, ezlopi_value_str);
                        if (cJSON_value_root)
                        {
                            cJSON *cJSON_value = cJSON_GetObjectItem(cJSON_value_root, ezlopi_value_str);
                            if (cJSON_value)
                            {
                                TRACE_D("scaled value : %f", (float)cJSON_value->valuedouble);
                                if (settings_current->properties->value.scalable_value)
                                {
                                    settings_current->properties->value.scalable_value->value = (float)cJSON_value->valuedouble;
                                }

                                if (settings_current->properties->value_nonvolatile == true)
                                {
                                    if (1 == ezlopi_nvs_write_float32(settings_current->properties->value.scalable_value->value, settings_current->properties->nvs_alias))
                                    {
                                        settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, settings_current->properties, NULL, NULL);
                                    }
                                    else
                                    {
                                        TRACE_E("Error writing settings to NVS");
                                    }
                                }
                                else
                                {
                                    settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, settings_current->properties, NULL, NULL);
                                }
                            }
                        }
                        else
                        {
                            TRACE_E("Error : Failed parsince JSON!");
                        }
                    }
                }
                else
                {
                }
            }
            settings_current = settings_current->next;
        }
    }
}

void _ezlopi_device_settings_reset_settings_id(uint32_t id)
{
    l_ezlopi_device_settings_t *configured_settings_current = configured_settings_head;
    while (NULL != configured_settings_current)
    {
        if (NULL != configured_settings_current->properties)
        {
            if (id == configured_settings_current->properties->id)
            {
                if (strcmp(configured_settings_current->properties->value_type, "action") == 0)
                {
                }
                else if (strcmp(configured_settings_current->properties->value_type, "bool") == 0)
                {

                    configured_settings_current->properties->value.bool_value = configured_settings_current->properties->value_defaut.bool_value;
                    if (configured_settings_current->properties->value_nonvolatile == true)
                    {
                        if (1 == ezlopi_nvs_write_bool(configured_settings_current->properties->value.bool_value, configured_settings_current->properties->nvs_alias))
                        {
                            configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                        }
                        else
                        {
                            TRACE_E("Error writing settings to NVS");
                        }
                    }
                    else
                    {
                        configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                    }
                }
                else if (strcmp(configured_settings_current->properties->value_type, "int") == 0)
                {
                    configured_settings_current->properties->value.int_value = configured_settings_current->properties->value_defaut.int_value;
                    if (configured_settings_current->properties->value_nonvolatile == true)
                    {
                        if (1 == ezlopi_nvs_write_int32(configured_settings_current->properties->value.int_value, configured_settings_current->properties->nvs_alias))
                        {
                            configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                        }
                        else
                        {
                            TRACE_E("Error writing settings to NVS");
                        }
                    }
                    else
                    {
                        configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                    }
                }
                else if (strcmp(configured_settings_current->properties->value_type, "string") == 0)
                {
                    configured_settings_current->properties->value.string_value = malloc(strlen(configured_settings_current->properties->value_defaut.string_value) + 1);
                    if (configured_settings_current->properties->value.string_value)
                    {
                        strncpy(configured_settings_current->properties->value.string_value, configured_settings_current->properties->value_defaut.string_value, strlen(configured_settings_current->properties->value_defaut.string_value) + 1);
                        if (configured_settings_current->properties->value_nonvolatile == true)
                        {
                            if (1 == ezlopi_nvs_write_str(configured_settings_current->properties->value.string_value, strlen(configured_settings_current->properties->value.string_value), configured_settings_current->properties->nvs_alias))
                            {
                                configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                            }
                            else
                            {
                                TRACE_E("Error writing settings to NVS");
                            }
                        }
                        else
                        {
                            configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                        }
                    }
                    else
                    {
                        TRACE_E("Error :  memory allocation, settings update failed !");
                    }
                }
                else if (strcmp(configured_settings_current->properties->value_type, "token") == 0)
                {
                    configured_settings_current->properties->value.token_value = malloc(strlen(configured_settings_current->properties->value_defaut.token_value) + 1);
                    if (configured_settings_current->properties->value.token_value)
                    {
                        strncpy(configured_settings_current->properties->value.token_value, configured_settings_current->properties->value_defaut.token_value, strlen(configured_settings_current->properties->value_defaut.token_value) + 1);
                        if (configured_settings_current->properties->value_nonvolatile == true)
                        {
                            if (1 == ezlopi_nvs_write_str(configured_settings_current->properties->value.token_value, strlen(configured_settings_current->properties->value.token_value), configured_settings_current->properties->nvs_alias))
                            {
                                configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                            }
                            else
                            {
                                TRACE_E("Error writing settings to NVS");
                            }
                        }
                        else
                        {
                            configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                        }
                    }
                    else
                    {
                        TRACE_E("Error :  memory allocation, settings update failed !");
                    }
                }
                else if (strcmp(configured_settings_current->properties->value_type, "rgb") == 0)
                {
                }
                else if (strcmp(configured_settings_current->properties->value_type, "scalable") == 0)
                {
                    configured_settings_current->properties->value.scalable_value->value = configured_settings_current->properties->value_defaut.scalable_value->value;
                    if (configured_settings_current->properties->value_nonvolatile == true)
                    {
                        if (1 == ezlopi_nvs_write_float32(configured_settings_current->properties->value.scalable_value->value, configured_settings_current->properties->nvs_alias))
                        {
                            configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                        }
                        else
                        {
                            TRACE_E("Error writing settings to NVS");
                        }
                    }
                    else
                    {
                        configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                    }
                }
                else
                {
                    TRACE_E("Error : settings update failed !");
                }
            }
        }
        configured_settings_current = configured_settings_current->next;
    }
}

void _ezlopi_device_settings_reset_device_id(uint32_t id)
{
    l_ezlopi_device_settings_t *configured_settings_current = configured_settings_head;
    while (NULL != configured_settings_current)
    {
        if (NULL != configured_settings_current->properties)
        {
            if (id == configured_settings_current->properties->device_id)
            {
                if (strcmp(configured_settings_current->properties->value_type, "action") == 0)
                {
                }
                else if (strcmp(configured_settings_current->properties->value_type, "bool") == 0)
                {

                    configured_settings_current->properties->value.bool_value = configured_settings_current->properties->value_defaut.bool_value;
                    if (configured_settings_current->properties->value_nonvolatile == true)
                    {
                        if (1 == ezlopi_nvs_write_bool(configured_settings_current->properties->value.bool_value, configured_settings_current->properties->nvs_alias))
                        {
                            configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                        }
                        else
                        {
                            TRACE_E("Error writing settings to NVS");
                        }
                    }
                    else
                    {
                        configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                    }
                }
                else if (strcmp(configured_settings_current->properties->value_type, "int") == 0)
                {
                    configured_settings_current->properties->value.int_value = configured_settings_current->properties->value_defaut.int_value;
                    if (configured_settings_current->properties->value_nonvolatile == true)
                    {
                        if (1 == ezlopi_nvs_write_int32(configured_settings_current->properties->value.int_value, configured_settings_current->properties->nvs_alias))
                        {
                            configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                        }
                        else
                        {
                            TRACE_E("Error writing settings to NVS");
                        }
                    }
                    else
                    {
                        configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                    }
                }
                else if (strcmp(configured_settings_current->properties->value_type, "string") == 0)
                {
                    configured_settings_current->properties->value.string_value = malloc(strlen(configured_settings_current->properties->value_defaut.string_value) + 1);
                    if (configured_settings_current->properties->value.string_value)
                    {
                        strncpy(configured_settings_current->properties->value.string_value, configured_settings_current->properties->value_defaut.string_value, strlen(configured_settings_current->properties->value_defaut.string_value) + 1);
                        if (configured_settings_current->properties->value_nonvolatile == true)
                        {
                            if (1 == ezlopi_nvs_write_str(configured_settings_current->properties->value.string_value, strlen(configured_settings_current->properties->value.string_value), configured_settings_current->properties->nvs_alias))
                            {
                                configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                            }
                            else
                            {
                                TRACE_E("Error writing settings to NVS");
                            }
                        }
                        else
                        {
                            configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                        }
                    }
                    else
                    {
                        TRACE_E("Error :  memory allocation, settings update failed !");
                    }
                }
                else if (strcmp(configured_settings_current->properties->value_type, "token") == 0)
                {
                    configured_settings_current->properties->value.token_value = malloc(strlen(configured_settings_current->properties->value_defaut.token_value) + 1);
                    if (configured_settings_current->properties->value.token_value)
                    {
                        strncpy(configured_settings_current->properties->value.token_value, configured_settings_current->properties->value_defaut.token_value, strlen(configured_settings_current->properties->value_defaut.token_value) + 1);
                        if (configured_settings_current->properties->value_nonvolatile == true)
                        {
                            if (1 == ezlopi_nvs_write_str(configured_settings_current->properties->value.token_value, strlen(configured_settings_current->properties->value.token_value), configured_settings_current->properties->nvs_alias))
                            {
                                configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                            }
                            else
                            {
                                TRACE_E("Error writing settings to NVS");
                            }
                        }
                        else
                        {
                            configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                        }
                    }
                    else
                    {
                        TRACE_E("Error :  memory allocation, settings update failed !");
                    }
                }
                else if (strcmp(configured_settings_current->properties->value_type, "rgb") == 0)
                {
                }
                else if (strcmp(configured_settings_current->properties->value_type, "scalable") == 0)
                {
                    configured_settings_current->properties->value.scalable_value->value = configured_settings_current->properties->value_defaut.scalable_value->value;
                    if (configured_settings_current->properties->value_nonvolatile == true)
                    {
                        if (1 == ezlopi_nvs_write_float32(configured_settings_current->properties->value.scalable_value->value, configured_settings_current->properties->nvs_alias))
                        {
                            configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                        }
                        else
                        {
                            TRACE_E("Error writing settings to NVS");
                        }
                    }
                    else
                    {
                        configured_settings_current->properties->__settings_call(EZLOPI_SETTINGS_ACTION_SET_SETTING, configured_settings_current->properties, NULL, NULL);
                    }
                }
                else
                {
                    TRACE_E("Error : settings update failed !");
                }
            }
        }
        configured_settings_current = configured_settings_current->next;
    }
}

void ezlopi_device_settings_print_settings(l_ezlopi_device_settings_t *head)
{
#if 0
    l_ezlopi_device_settings_t *current = head;

    if (current == NULL)
    {
        TRACE_E("No settings found !");
        return;
    }

    while (current != NULL)
    {
        s_ezlopi_device_settings_properties_t *properties = current->properties;

        TRACE_I("ID: %u", properties->id);
        TRACE_I("Device ID: %u", properties->device_id);
        if (properties->label)
            TRACE_I("Label: %s", properties->label);
        if (properties->description)
            TRACE_I("Description: %s", properties->description);
        if (properties->status)
            TRACE_I("Status: %s", properties->status);
        if (properties->value_type)
            TRACE_I("Value Type: %s", properties->value_type);
        if (properties->nvs_alias)
            TRACE_I("NVS Alias: %s", properties->nvs_alias);

        // Print value based on value type
        if (strcmp(properties->value_type, "string") == 0)
        {
            TRACE_I("String Value: %s", properties->value.string_value);
        }
        else if (strcmp(properties->value_type, "token") == 0)
        {
            TRACE_I("String Value: %s", properties->value.token_value);
        }
        else if (strcmp(properties->value_type, "int") == 0)
        {
            TRACE_I("Integer Value: %d", properties->value.int_value);
        }
        else if (strcmp(properties->value_type, "bool") == 0)
        {
            TRACE_I("Boolean Value: %s", properties->value.bool_value ? "true" : "false");
        }
        else if (strcmp(properties->value_type, "scalable") == 0)
        {
            TRACE_I("Scallable Value: %f", properties->value.scalable_value->value);
        }
        else
        {
            TRACE_I("Unknown value type");
        }

        current = current->next;
    }
#endif
}