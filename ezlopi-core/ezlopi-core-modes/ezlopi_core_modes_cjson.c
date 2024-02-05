#include <cJSON.h>

#include "ezlopi_core_modes.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_modes_cjson.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_cloud_constants.h"

static void __cjson_add_alarmed(cJSON *cj_alarmed, s_alarmed_t *alarmed);
static void __cjson_add_mode_to_array(cJSON *cj_modes_arr, s_house_modes_t *mode);
static void __cjson_add_entry_delay(cJSON *cj_result, s_entry_delay_t *entry_delay);
static void __cjson_add_number_as_hex_string(cJSON *cj_dest, const char *obj_name, uint32_t number);
static void __cjson_add_protect_buttons(cJSON *cj_protect_buttons_arr, s_protect_buttons_t *l_protect_buttons);

//////////////////////

int ezlopi_core_modes_cjson_get_modes(cJSON *cj_dest)
{
    uint32_t ret = 0;
    s_ezlopi_modes_t *_modes = ezlopi_core_modes_get_custom_modes();
    if (_modes)
    {
        __cjson_add_number_as_hex_string(cj_dest, ezlopi_current_str, _modes->current_mode_id);
        __cjson_add_number_as_hex_string(cj_dest, ezlopi_switchTo_str, _modes->switch_to_mode_id);

        cJSON_AddNumberToObject(cj_dest, ezlopi_timeIsLeftToSwitch_str, _modes->time_is_left_to_switch_sec);
        cJSON_AddNumberToObject(cj_dest, ezlopi_switchToDelay_str, _modes->switch_to_delay_sec);
        cJSON_AddNumberToObject(cj_dest, ezlopi_alarmDelay_str, _modes->alarm_delay_sec);

        __cjson_add_entry_delay(cj_dest, &_modes->entry_delay);

        cJSON *cj_modes_array = cJSON_AddArrayToObject(cj_dest, ezlopi_modes_str);
        if (cj_modes_array)
        {
            __cjson_add_mode_to_array(cj_modes_array, _modes->mode_home);
            __cjson_add_mode_to_array(cj_modes_array, _modes->mode_away);
            __cjson_add_mode_to_array(cj_modes_array, _modes->mode_night);
            __cjson_add_mode_to_array(cj_modes_array, _modes->mode_vacation);
        }

        cJSON_AddItemReferenceToObject(cj_dest, ezlopi_devices_str, _modes->cj_devices);
        cJSON_AddItemReferenceToObject(cj_dest, ezlopi_alarams_str, _modes->cj_alarms);
        cJSON_AddItemReferenceToObject(cj_dest, ezlopi_cameras_str, _modes->cj_cameras);

        cJSON *cj_protect_buttons_arr = cJSON_AddArrayToObject(cj_dest, ezlopi_protectButtons_str);
        if (cj_protect_buttons_arr)
        {
            __cjson_add_protect_buttons(cj_protect_buttons_arr, _modes->l_protect_buttons);
        }

        cJSON *cj_alarmed = cJSON_AddObjectToObject(cj_dest, ezlopi_alarmed_str);
        if (cj_alarmed)
        {
            __cjson_add_alarmed(cj_alarmed, &_modes->alarmed);
        }
    }

    return ret;
}

s_ezlopi_modes_t *ezlopi_core_modes_cjson_parse_modes(cJSON *cj_modes)
{
    s_ezlopi_modes_t *parsed_mode = NULL;
    if (cj_modes)
    {
        parsed_mode = (s_ezlopi_modes_t *)malloc(sizeof(s_ezlopi_modes_t));
        if (parsed_mode)
        {
            CJSON_GET_ID(parsed_mode->current_mode_id, cJSON_GetObjectItem(cj_modes, ezlopi_current_str));
            CJSON_GET_ID(parsed_mode->switch_to_mode_id, cJSON_GetObjectItem(cj_modes, ezlopi_switchTo_str));
            CJSON_GET_VALUE_DOUBLE(cj_modes, ezlopi_timeIsLeftToSwitch_str, parsed_mode->time_is_left_to_switch_sec);
            CJSON_GET_VALUE_DOUBLE(cj_modes, ezlopi_switchToDelay_str, parsed_mode->switch_to_delay_sec);
            CJSON_GET_VALUE_DOUBLE(cj_modes, ezlopi_alarmDelay_str, parsed_mode->alarm_delay_sec);

            cJSON *cj_entry_delay = cJSON_GetObjectItem(cj_modes, ezlopi_entryDelay_str);
            if (cj_entry_delay)
            {
                CJSON_GET_VALUE_DOUBLE(cj_modes, ezlopi_normal_str, parsed_mode->entry_delay.normal_delay_sec);
                CJSON_GET_VALUE_DOUBLE(cj_modes, ezlopi_short_str, parsed_mode->entry_delay.short_delay_sec);
                CJSON_GET_VALUE_DOUBLE(cj_modes, ezlopi_extended_str, parsed_mode->entry_delay.extended_delay_sec);
                CJSON_GET_VALUE_DOUBLE(cj_modes, ezlopi_instant_str, parsed_mode->entry_delay.normal_delay_sec);
            }

            cJSON *cj_house_modes = cJSON_GetObjectItem(cj_modes, ezlopi_modes_str);
            if (cj_house_modes)
            {
                uint32_t mode_idx = 0;
                cJSON *cj_house_mod = NULL;

                while (NULL != (cj_house_mod = cJSON_GetArrayItem(cj_house_modes, mode_idx)))
                {
                    uint32_t _mode_id = 0;
                    uint32_t _init_id = ezlopi_cloud_get_modes_initial_id();
                    CJSON_GET_ID(_mode_id, cJSON_GetObjectItem(cj_house_modes, ezlopi__id_str));

                    if ((_mode_id & 0xffffff00) == _init_id)
                    {
                        _mode_id &= 0x000000ff;
                        s_house_modes_t *cur_house_mode = NULL;

                        switch (_mode_id)
                        {
                        case EZLOPI_HOUSE_MODE_REF_ID_HOME:
                        {
                            cur_house_mode = parsed_mode->mode_home;
                            parsed_mode->mode_home->_id = _mode_id;
                            parsed_mode->mode_home->name = ezlopi_Home_str;
                            break;
                        }
                        case EZLOPI_HOUSE_MODE_REF_ID_AWAY:
                        {
                            cur_house_mode = parsed_mode->mode_away;
                            parsed_mode->mode_home->_id = _mode_id;
                            parsed_mode->mode_home->name = ezlopi_Away_str;
                            break;
                        }
                        case EZLOPI_HOUSE_MODE_REF_ID_NIGHT:
                        {
                            cur_house_mode = parsed_mode->mode_night;
                            parsed_mode->mode_home->_id = _mode_id;
                            parsed_mode->mode_home->name = ezlopi_Night_str;
                            break;
                        }
                        case EZLOPI_HOUSE_MODE_REF_ID_VACATION:
                        {
                            cur_house_mode = parsed_mode->mode_vacation;
                            parsed_mode->mode_home->_id = _mode_id;
                            parsed_mode->mode_home->name = ezlopi_Vacation_str;
                            break;
                        }
                        default:
                        {
                            TRACE_E("Undefined house-mode-id!");
                            break;
                        }
                        }

                        {
                            cJSON *cj_description = cJSON_GetObjectItem(cj_house_mod, ezlopi_description_str);
                            if (cj_description && cj_description->valuestring)
                            {
                                uint32_t desc_len = strlen(cj_description->valuestring) + 1;
                                cur_house_mode->description = (char *)malloc(desc_len);
                                if (cur_house_mode->description)
                                {
                                    snprintf(cur_house_mode->description, desc_len, "%s", cj_description->valuestring);
                                }
                            }
                        }
                        {
                            cJSON *cj_notification = cJSON_GetObjectItem(cj_house_mod, ezlopi_notifications_str);
                            if (cj_notification)
                            {
                                cur_house_mode->cj_notifications = cJSON_Duplicate(cj_notification, cJSON_True);
                            }
                        }

                        CJSON_GET_VALUE_BOOL(cj_house_mod, ezlopi_disarmedDefault_str, cur_house_mode->disarmed_default);
                        {
                            cJSON *cj_disarmed_devices = cJSON_GetObjectItem(cj_house_mod, ezlopi_notifications_str);
                            if (cj_disarmed_devices)
                            {
                                cur_house_mode->cj_disarmed_devices = cJSON_Duplicate(cj_disarmed_devices, cJSON_True);
                            }
                        }
                        {
                            cJSON *cj_alarms_off_devices = cJSON_GetObjectItem(cj_house_mod, ezlopi_alarmsOffDevices_str);
                            if (cj_alarms_off_devices)
                            {
                                cur_house_mode->cj_alarms_off_devices = cJSON_Duplicate(cj_alarms_off_devices, cJSON_True);
                            }
                        }
                        {
                            cJSON *cj_cameras_off_devices = cJSON_GetObjectItem(cj_house_mod, ezlopi_camerasOffDevices_str);
                            if (cj_cameras_off_devices)
                            {
                                cur_house_mode->cj_cameras_off_devices = cJSON_Duplicate(cj_cameras_off_devices, cJSON_True);
                            }
                        }

                        CJSON_GET_VALUE_BOOL(cj_house_mod, ezlopi_protect_str, cur_house_mode->protect);
                        CJSON_GET_VALUE_BOOL(cj_house_mod, ezlopi_armed_str, cur_house_mode->armed);
                    }

                    mode_idx++;
                }
            }

            {
                cJSON *cj_devices = cJSON_GetObjectItem(cj_modes, ezlopi_devices_str);
                if (cj_devices)
                {
                    parsed_mode->cj_devices = cJSON_Duplicate(cj_devices, cJSON_True);
                }
            }

            {
                cJSON *cj_alarms = cJSON_GetObjectItem(cj_modes, ezlopi_alarms_str);
                if (cj_alarms)
                {
                    parsed_mode->cj_alarms = cJSON_Duplicate(cj_alarms, cJSON_True);
                }
            }

            {
                cJSON *cj_cameras = cJSON_GetObjectItem(cj_modes, ezlopi_cameras_str);
                if (cj_cameras)
                {
                    parsed_mode->cj_cameras = cJSON_Duplicate(cj_cameras, cJSON_True);
                }
            }

            {
                cJSON * cj_protect_buttons = cJSON_GetObjectItem(cj_modes, ezlopi_protectButtons_str);
                if (cj_protect_buttons)
                {
                    uint32_t idx = 0;
                    cJSON * cj_button = NULL;
                    
                    while (NULL != (cj_button = cJSON_GetArrayItem(cj_protect_buttons, idx)))
                    {
                        parsed_mode->l_protect_buttons = 

                        idx++;
                    }
                }
            }
        }
    }

    return parsed_mode;
}

////////////////////////////////

static void __cjson_add_number_as_hex_string(cJSON *cj_dest, const char *obj_name, uint32_t number)
{
    if (cj_dest && obj_name)
    {
        char tmp_str[32];
        snprintf(tmp_str, sizeof(tmp_str), "%08x", number);
        cJSON_AddStringToObject(cj_dest, obj_name, tmp_str);
    }
}

static void __cjson_add_entry_delay(cJSON *cj_result, s_entry_delay_t *entry_delay)
{
    cJSON *cj_entry_delay = cJSON_AddObjectToObject(cj_result, ezlopi_entryDelay_str);
    if (cj_entry_delay)
    {
        cJSON_AddNumberToObject(cj_entry_delay, ezlopi_normal_str, entry_delay->normal_delay_sec);
        cJSON_AddNumberToObject(cj_entry_delay, ezlopi_short_str, entry_delay->short_delay_sec);
        cJSON_AddNumberToObject(cj_entry_delay, ezlopi_extended_str, entry_delay->extended_delay_sec);
        cJSON_AddNumberToObject(cj_entry_delay, ezlopi_instant_str, entry_delay->instant_delay_sec);
    }
}

static void __cjson_add_mode_to_array(cJSON *cj_modes_arr, s_house_modes_t *mode)
{
    if (mode)
    {
        cJSON *cj_mode = cJSON_CreateObject();
        if (cj_mode)
        {
            __cjson_add_number_as_hex_string(cj_mode, ezlopi__id_str, mode->_id);

            cJSON_AddStringToObject(cj_mode, ezlopi_name_str, mode->name);
            cJSON_AddStringToObject(cj_mode, ezlopi_description_str, mode->description ? mode->description : "");
            cJSON_AddItemReferenceToObject(cj_mode, ezlopi_notifications_str, mode->cj_notifications);
            cJSON_AddBoolToObject(cj_mode, ezlopi_disarmedDefault_str, mode->disarmed_default);
            cJSON_AddItemReferenceToObject(cj_mode, ezlopi_disarmedDevices_str, mode->cj_disarmed_devices);
            cJSON_AddItemReferenceToObject(cj_mode, ezlopi_alarmsOffDevices_str, mode->cj_alarms_off_devices);
            cJSON_AddItemReferenceToObject(cj_mode, ezlopi_camerasOffDevices_str, mode->cj_cameras_off_devices);
            cJSON_AddBoolToObject(cj_mode, ezlopi_protect_str, mode->protect);
            cJSON_AddBoolToObject(cj_mode, ezlopi_armed_str, mode->armed);

            if (!cJSON_AddItemToArray(cj_modes_arr, cj_mode))
            {
                cJSON_Delete(cj_mode);
            }
        }
    }
}

static void __cjson_add_protect_buttons(cJSON *cj_protect_buttons_arr, s_protect_buttons_t *l_protect_buttons)
{
    while (l_protect_buttons)
    {
        cJSON *cj_protect_button = cJSON_CreateObject();
        if (cj_protect_button)
        {
            __cjson_add_number_as_hex_string(cj_protect_button, ezlopi_id_str, l_protect_buttons->device_id);
            cJSON_AddStringToObject(cj_protect_button, ezlopi_service_str, l_protect_buttons->service_name);

            if (!cJSON_AddItemToArray(cj_protect_buttons_arr, cj_protect_button))
            {
                cJSON_Delete(cj_protect_button);
            }
        }

        l_protect_buttons = l_protect_buttons->next;
    }
}

static void __cjson_add_alarmed(cJSON *cj_alarmed, s_alarmed_t *alarmed)
{
    if (alarmed)
    {
        cJSON_AddNumberToObject(cj_alarmed, ezlopi_entryDelay_str, alarmed->entry_delay_sec);
        cJSON_AddNumberToObject(cj_alarmed, ezlopi_timeIsLeft_str, alarmed->time_is_left_sec);
        cJSON_AddStringToObject(cj_alarmed, ezlopi_type_str, alarmed->type);

        cJSON *cj_sources_arr = cJSON_AddArrayToObject(cj_alarmed, ezlopi_sources_str);
        if (cj_sources_arr)
        {
            s_sources_t *source_node = alarmed->sources;
            while (source_node)
            {
                cJSON *cj_source = cJSON_CreateObject();
                if (cj_source)
                {
                    __cjson_add_number_as_hex_string(cj_source, ezlopi_deviceId_str, source_node->device_id);
                    cJSON_AddNumberToObject(cj_source, ezlopi_timestamp_str, source_node->time_stamp);
                    cJSON_AddNumberToObject(cj_source, ezlopi_delay_str, source_node->delay);
                }

                source_node = source_node->next;
            }
        }
    }
}

///////////////////////////////
static
