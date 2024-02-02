#include "ezlopi_cloud_constants.h"

#include "ezlopi_core_nvs.h"
#include "ezlopi_core_modes.h"

static s_house_modes_t *sg_current_mode = NULL;
static s_ezlopi_modes_t *sg_custom_modes = NULL;

static void __cjson_add_alarmed(cJSON *cj_alarmed, s_alarmed_t *alarmed);
static void __cjson_add_mode_to_array(cJSON *cj_modes_arr, s_house_modes_t *mode);
static void __cjson_add_entry_delay(cJSON *cj_result, s_entry_delay_t *entry_delay);
static void __cjson_add_number_as_hex_string(cJSON *cj_dest, const char *obj_name, uint32_t number);
static void __cjson_add_protect_buttons(cJSON *cj_protect_buttons_arr, s_protect_buttons_t *l_protect_buttons);

int ezlopi_core_modes_add_to_result(cJSON *cj_result)
{
    int ret = 0;
    if (sg_custom_modes)
    {
        __cjson_add_number_as_hex_string(cj_result, ezlopi_current_str, sg_custom_modes->current_mode_id);
        __cjson_add_number_as_hex_string(cj_result, ezlopi_switchTo_str, sg_custom_modes->switch_to_mode_id);

        cJSON_AddNumberToObject(cj_result, ezlopi_timeIsLeftToSwitch_str, sg_custom_modes->time_is_left_to_switch_sec);
        cJSON_AddNumberToObject(cj_result, ezlopi_switchToDelay_str, sg_custom_modes->switch_to_delay_sec);
        cJSON_AddNumberToObject(cj_result, ezlopi_alarmDelay_str, sg_custom_modes->alarm_delay_sec);

        __cjson_add_entry_delay(cj_result, &sg_custom_modes->entry_delay);

        cJSON *cj_modes_array = cJSON_AddArrayToObject(cj_result, ezlopi_modes_str);
        if (cj_modes_array)
        {
            __cjson_add_mode_to_array(cj_modes_array, sg_custom_modes->mode_home);
            __cjson_add_mode_to_array(cj_modes_array, sg_custom_modes->mode_away);
            __cjson_add_mode_to_array(cj_modes_array, sg_custom_modes->mode_night);
            __cjson_add_mode_to_array(cj_modes_array, sg_custom_modes->mode_vacation);
        }

        cJSON_AddItemReferenceToObject(cj_result, ezlopi_devices_str, sg_custom_modes->cj_devices);
        cJSON_AddItemReferenceToObject(cj_result, ezlopi_alarams_str, sg_custom_modes->cj_alarms);
        cJSON_AddItemReferenceToObject(cj_result, ezlopi_cameras_str, sg_custom_modes->cj_cameras);

        cJSON *cj_protect_buttons_arr = cJSON_AddArrayToObject(cj_result, ezlopi_protectButtons_str);
        if (cj_protect_buttons_arr)
        {
            __cjson_add_protect_buttons(cj_protect_buttons_arr, sg_custom_modes->l_protect_buttons);
        }

        cJSON *cj_alarmed = cJSON_AddObjectToObject(cj_result, ezlopi_alarmed_str);
        if (cj_alarmed)
        {
            __cjson_add_alarmed(cj_alarmed, &sg_custom_modes->alarmed);
        }
    }

    return ret;
}

void ezlopi_core_modes_init(void)
{
    ezlopi_core_default_init();
    if (NULL == sg_custom_modes)
    {
        sg_custom_modes = ezlopi_core_default_mode_get();
    }
}

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
        cJSON_AddNumberToObject(cj_alarmed, ezlopi_timeIsLeft_str, sg_custom_modes->time_is_left_to_switch_sec);
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
