#include <time.h>
#include "ezlopi_util_trace.h"

#include "ezlopi_core_modes.h"
#include "ezlopi_core_sntp.h"
#include "ezlopi_core_ota.h"
#include "ezlopi_core_http.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_event_group.h"
#include "ezlopi_core_scenes_operators.h"
#include "ezlopi_core_websocket_client.h"
#include "ezlopi_core_scenes_when_methods.h"
#include "ezlopi_core_scenes_status_changed.h"
#include "ezlopi_core_scenes_when_methods_helper_functions.h"

#include "ezlopi_cloud_constants.h"

int ezlopi_scene_when_is_item_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W(" is_item_state ");
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block)
    {
        uint32_t item_id = 0;
        l_fields_v2_t* value_field = NULL;
        #warning "Warning: armed check remains [Krishna]";

        l_fields_v2_t* curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 4))
            {
                item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
            }
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, 4))
            {
                value_field = curr_field;
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field)
        {
            l_ezlopi_device_t* curr_device = ezlopi_device_get_head();
            while (curr_device)
            {
                l_ezlopi_item_t* curr_item = curr_device->items;
                while (curr_item)
                {
                    if (item_id == curr_item->cloud_properties.item_id)
                    {
                        cJSON* cj_tmp_value = cJSON_CreateObject();
                        if (cj_tmp_value)
                        {
                            curr_item->func(EZLOPI_ACTION_GET_EZLOPI_VALUE, curr_item, (void*)cj_tmp_value, NULL);
                            cJSON* cj_value = cJSON_GetObjectItem(cj_tmp_value, ezlopi_value_str);
                            if (cj_value)
                            {
                                switch (cj_value->type)
                                {
                                case cJSON_True:
                                {
                                    if (true == value_field->field_value.u_value.value_bool)
                                    {
                                        ret = 1;
                                    }
                                    break;
                                }
                                case cJSON_False:
                                {
                                    if (false == value_field->field_value.u_value.value_bool)
                                    {
                                        ret = 1;
                                    }
                                    break;
                                }
                                case cJSON_Number:
                                {
                                    if (cj_value->valuedouble == value_field->field_value.u_value.value_double)
                                    {
                                        ret = 1;
                                    }
                                    break;
                                }
                                case cJSON_String:
                                {
                                    uint32_t cmp_size = (strlen(cj_value->valuestring) > strlen(value_field->field_value.u_value.value_string)) ? strlen(cj_value->valuestring) : strlen(value_field->field_value.u_value.value_string);
                                    if (0 == strncmp(cj_value->valuestring, value_field->field_value.u_value.value_string, cmp_size))
                                    {
                                        ret = 1;
                                    }
                                    break;
                                }
                                default:
                                {
                                    TRACE_E("Value type mis-matched!");
                                }
                                }
                            }

                            cJSON_Delete(cj_tmp_value);
                        }
                        break;
                    }
                    curr_item = curr_item->next;
                }

                curr_device = curr_device->next;
            }
        }
    }

    return ret;
}

int ezlopi_scene_when_is_interval(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W(" is_interval ");
    int ret = 0;

    if (scene_node)
    {
        char* end_prt = NULL;
        uint32_t interval = strtoul(scene_node->when_block->fields->field_value.u_value.value_string, &end_prt, 10);
        if (end_prt)
        {
            if (0 == strncmp(end_prt, "m", 1))
            {
                interval *= 60;
            }
            else if (0 == strncmp(end_prt, "h", 1))
            {
                interval *= (60 * 60);
            }
            else if (0 == strncmp(end_prt, "d", 1))
            {
                interval *= (60 * 60 * 24);
            }
        }

        uint32_t last_update = 0;
        if (scene_node->when_block->fields->user_arg)
        {
            last_update = (uint32_t)scene_node->when_block->fields->user_arg;

            if (((xTaskGetTickCount() - last_update) / 1000) > interval)
            {
                scene_node->when_block->fields->user_arg = (void*)xTaskGetTickCount();
                ret = 1;
            }
        }
        else
        {
            ret = 1;
            scene_node->when_block->fields->user_arg = (void*)xTaskGetTickCount();
        }
    }

    return ret;
}

int ezlopi_scene_when_is_item_state_changed(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_item_state_changed' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_button_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_button_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_sun_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W(" is_SunState ");
    // TRACE_W("Warning: need sunrise and sunset timing from actual api");

    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block && scene_node)
    {
        time_t rawtime = 0;
        time(&rawtime);
        struct tm* info;
        info = localtime(&rawtime);

        TRACE_W("%d:%d:%d ", info->tm_hour, info->tm_min, info->tm_sec);

        // list of function for extracting field parameter
        const s_issunstate_method_t __issunstate_field[] = {
            {.field_name = "sunrise", .field_func = issunstate_get_suntime},
            {.field_name = "sunset", .field_func = issunstate_get_suntime},
            {.field_name = "time", .field_func = issunstate_get_offs_tmval},
            {.field_name = "weekdays", .field_func = issunstate_eval_weekdays},
            {.field_name = "days", .field_func = issunstate_eval_days},
            {.field_name = "range", .field_func = issunstate_eval_range},
            {.field_name = NULL, .field_func = NULL},
        };

        // Condition checker
        uint8_t flag_check = 0;
        l_fields_v2_t* curr_field = when_block->fields;
        while (NULL != curr_field)
        {
            for (uint8_t i = 0; i < ((sizeof(__issunstate_field) / sizeof(__issunstate_field[i]))); i++)
            {
                if (0 == strncmp(__issunstate_field[i].field_name, curr_field->name, strlen(__issunstate_field[i].field_name) + 1))
                {
                    flag_check |= (__issunstate_field[i].field_func)(scene_node, curr_field, info, ((0 == i) ? 1 : (1 == i) ? 2
                        : 0));
                    break;
                }
            }
            curr_field = curr_field->next;
        }
        // Now check the flag results
        ret = issunstate_check_flag_result(scene_node, info, flag_check);
    }
    return ret;
}

int ezlopi_scene_when_is_date(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W(" isDate ");
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block && scene_node)
    {
        time_t rawtime = 0;
        time(&rawtime);
        struct tm* info;
        info = localtime(&rawtime);

        TRACE_W("%d:%d:%d ", info->tm_hour, info->tm_min, info->tm_sec);

        if (2 == info->tm_sec) // nth sec mark
        {
            // list of field function to extract the respective parameters
            const s_isdate_method_t __isdate_func[] = {
                {.field_name = "type", .field_func = isdate_type_check},
                {.field_name = "time", .field_func = isdate_tm_check},
                {.field_name = "weekdays", .field_func = isdate_weekdays_check},
                {.field_name = "days", .field_func = isdate_mdays_check},
                {.field_name = "weeks", .field_func = isdate_year_weeks_check},
                {.field_name = NULL, .field_func = NULL},
            };
            uint8_t flag_check = 0;
            e_isdate_modes_t mode_type = ISDATE_UNDEFINED_MODE;
            l_fields_v2_t* curr_field = when_block->fields;
            while (NULL != curr_field)
            {
                for (uint8_t i = 0; i < ((sizeof(__isdate_func) / sizeof(__isdate_func[i]))); i++)
                {
                    if (0 == strncmp(__isdate_func[i].field_name, curr_field->name, strlen(__isdate_func[i].field_name) + 1))
                    {
                        flag_check |= (__isdate_func[i].field_func)(&mode_type, info, curr_field); // bit0 - bit3
                        break;
                    }
                }
                curr_field = curr_field->next;
            }
            ret = isdate_check_flag_result(mode_type, flag_check);
            // Output Filter based on date+time of activation
            TRACE_S("mode[%d], isDate:- FLAG_STATUS: %#x", mode_type, flag_check);
        }
    }
    return ret;
}

int ezlopi_scene_when_is_once(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W(" isOnce ");
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block && scene_node)
    {
        time_t rawtime = 0;
        time(&rawtime);
        struct tm* info;
        info = localtime(&rawtime);

        TRACE_W("%d:%d:%d ", info->tm_hour, info->tm_min, info->tm_sec);

        if (4 == info->tm_sec) // nth sec mark
        {
            // list of funciton to check validity of each field values
            const s_isonce_method_t __isonce_method[] = {
                {.field_name = "time", .field_func = isonce_tm_check},
                {.field_name = "day", .field_func = isonce_day_check},
                {.field_name = "month", .field_func = isonce_month_check},
                {.field_name = "year", .field_func = isonce_year_check},
                {.field_name = NULL, .field_func = NULL},
            };
            uint8_t flag_check = 0;
            l_fields_v2_t* curr_field = when_block->fields;
            while (curr_field)
            {
                for (uint8_t i = 0; i < ((sizeof(__isonce_method) / sizeof(__isonce_method[i]))); i++)
                {
                    if (0 == strncmp(__isonce_method[i].field_name, curr_field->name, strlen(__isonce_method[i].field_name) + 1))
                    {
                        flag_check |= (__isonce_method[i].field_func)(curr_field, info);
                        break;
                    }
                }
                curr_field = curr_field->next;
            }

            // Output Filter based on date & time
            ret = isonce_check_flag_result(scene_node, flag_check);
            TRACE_S("isOnce :- FLAG_STATUS: 0x0%x", flag_check);
        }
    }
    return ret;
}

int ezlopi_scene_when_is_date_range(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W(" isDate_range ");
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block && scene_node)
    {
        time_t rawtime = 0;
        time(&rawtime);
        struct tm* info;
        info = localtime(&rawtime);

        TRACE_W("%d:%d:%d ", info->tm_hour, info->tm_min, info->tm_sec);

        if (6 == info->tm_sec) // nth sec mark
        {
            // Default values : start and end times.
            struct tm start = {
                .tm_hour = 0,
                .tm_min = 0,
            };
            struct tm end = {
                .tm_hour = 23,
                .tm_min = 59,
            };

            // field function pointers
            const s_isdate_range_method_t _isdate_range_fields[] = {
                {.field_name = "startTime", .field_func = isdate_range_get_tm},
                {.field_name = "startDay", .field_func = isdate_range_get_startday},
                {.field_name = "startMonth", .field_func = isdate_range_get_startmonth},
                {.field_name = "startYear", .field_func = isdate_range_get_startyear},
                {.field_name = "endTime", .field_func = isdate_range_get_tm},
                {.field_name = "endDay", .field_func = isdate_range_get_endday},
                {.field_name = "endMonth", .field_func = isdate_range_get_endmonth},
                {.field_name = "endYear", .field_func = isdate_range_get_endyear},
                {.field_name = NULL, .field_func = NULL},
            };
            l_fields_v2_t* curr_field = when_block->fields;
            while (curr_field)
            {
                for (int i = 0; i < ((sizeof(_isdate_range_fields) / sizeof(_isdate_range_fields[i]))); i++)
                {
                    if (0 == strncmp(_isdate_range_fields[i].field_name, curr_field->name, strlen(_isdate_range_fields[i].field_name) + 1))
                    {
                        (_isdate_range_fields[i].field_func)(curr_field, ((i < 4) ? &start : &end));
                        break;
                    }
                }
                curr_field = curr_field->next;
            }

            // Check for time,day,month and year validity
            uint8_t(*isdate_range_check_flags[])(struct tm* start, struct tm* end, struct tm* info) = {
                isdate_range_check_tm,
                isdate_range_check_day,
                isdate_range_check_month,
                isdate_range_check_year,
            };
            uint8_t flag_check = 0;
            for (uint8_t i = 0; i < ISDATE_RANGE_MAX; i++)
            {
                flag_check |= isdate_range_check_flags[i](&start, &end, info);
            }

            ret = isdate_range_check_flag_result(flag_check);
            TRACE_S("isdate_range flag_check [0x0%x]", flag_check);
        }
    }
    return ret;
}

int ezlopi_scene_when_is_user_lock_operation(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_user_lock_operation' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_house_mode_changed_to(l_scenes_list_v2_t* scene_node, void* arg)
{
    //TRACE_W(" isHouse_mode ");
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;

    if (when_block)
    {
        l_fields_v2_t* house_mode_id_array = NULL;
        l_fields_v2_t* curr_field = when_block->fields;

        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, ezlopi_houseMode_str, strlen(ezlopi_houseMode_str)))
            {
                house_mode_id_array = curr_field;
            }

            curr_field = curr_field->next;
        }

        uint32_t idx = 0;
        cJSON* cj_house_mdoe_id = NULL;

        while (NULL == (cj_house_mdoe_id = cJSON_GetArrayItem(house_mode_id_array->field_value.u_value.cj_value, idx++)))
        {
            if (cj_house_mdoe_id->valuestring)
            {
                uint32_t house_mode_id = strtoul(cj_house_mdoe_id->valuestring, NULL, 16);
                s_ezlopi_modes_t* modes = ezlopi_core_modes_get_custom_modes();
                if ((modes->current_mode_id == house_mode_id) && ((uint32_t)house_mode_id_array->user_arg != modes->current_mode_id))
                {
                    ret = 1;
                    house_mode_id_array->user_arg = (void*)house_mode_id;
                    TRACE_E("house-mode-changed-to: %d", house_mode_id);
                }
            }
        }
    }

    return ret;
}

int ezlopi_scene_when_is_house_mode_changed_from(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_house_mode_changed_from' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_device_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W(" isDevice_state. ");
    // TRACE_W("Warning: when-method 'is_device_state' not implemented!");
    int ret = 0;

    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block && scene_node)
    {
        uint32_t device_id = 0;
        bool value_armed = false;
        bool value_reachable = false;

        l_fields_v2_t* curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "device", 7))
            {
                if (EZLOPI_VALUE_TYPE_DEVICE == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    device_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                }
            }
            else if (0 == strncmp(curr_field->name, "armed", 6))
            {
                if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                {
                    value_armed = curr_field->field_value.u_value.value_bool;
                }
            }
            else if (0 == strncmp(curr_field->name, "reachable", 10))
            {
                if (EZLOPI_VALUE_TYPE_BOOL == curr_field->value_type)
                {
                    value_reachable = curr_field->field_value.u_value.value_bool;
                }
            }
            curr_field = curr_field->next;
        }
        if (device_id)
        {
            l_ezlopi_device_t* curr_device = ezlopi_device_get_head();
            while (curr_device)
            {
                if (device_id == curr_device->cloud_properties.device_id)
                {
                    s_ezlopi_cloud_controller_t* controller_info = ezlopi_device_get_controller_information();
                    if (controller_info)
                    {
                        #warning "we need to change from 'controller' to 'device_id' specific";
                        ret = ((value_armed == controller_info->armed) ? 1 : 0);
                        ret = ((value_reachable == controller_info->service_notification) ? 1 : 0);
                    }
                }

                curr_device = curr_device->next;
            }
        }
        #warning "need to check device_group condition";
    }

    return ret;
}

int ezlopi_scene_when_is_network_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_network_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_scene_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W(" isScene_state ");
    // TRACE_W("Warning: when-method 'is_scene_state' not implemented!");
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block && scene_node)
    {
        uint32_t scene_id = 0;
        l_fields_v2_t* value_field = NULL;

        l_fields_v2_t* curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "scene", 6))
            {
                if (EZLOPI_VALUE_TYPE_SCENEID == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    scene_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                    // TRACE_E("scene_id : %d", scene_id);
                }
            }
            else if (0 == strncmp(curr_field->name, "state", 6))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    value_field = curr_field;
                }
            }
            curr_field = curr_field->next;
        }

        if (scene_id && value_field)
        {

            l_scenes_list_v2_t* curr_scene = ezlopi_scenes_get_scenes_head_v2();
            while (curr_scene)
            {
                if (curr_scene->_id == scene_id)
                {
                    if (0 == strncmp("any_result", value_field->field_value.u_value.value_string, 11))
                    {
                        ret = 1;
                    }
                    else if (0 == strncmp("scene_enabled", value_field->field_value.u_value.value_string, 14))
                    {
                        ret = (true == curr_scene->enabled) ? 1 : 0;
                    }
                    else if (0 == strncmp("scene_disabled", value_field->field_value.u_value.value_string, 14))
                    {
                        ret = (false == curr_scene->enabled) ? 1 : 0;
                    }
                    else if (0 == strncmp("finished", value_field->field_value.u_value.value_string, 9))
                    {
                        ret = (EZLOPI_SCENE_STATUS_STOP == curr_scene->status) ? 1 : 0;
                    }
                    else if (0 == strncmp("partially_finished", value_field->field_value.u_value.value_string, 19))
                    {
                        ret = (EZLOPI_SCENE_STATUS_RUNNING == curr_scene->status) ? 1 : 0;
                    }
                    else if (0 == strncmp("stopped", value_field->field_value.u_value.value_string, 8))
                    {
                        ret = (EZLOPI_SCENE_STATUS_STOPPED == curr_scene->status) ? 1 : 0;
                    }
                    #warning "need to add 'FAILED' status for scene";
                    // else if (0 == strncmp("failed", value_field->field_value.u_value.value_string, 7))
                    // {
                    //     ret = (false == curr_scene->enabled)? 1:0;
                    // }
                    // TRACE_E("scene_state : %s , ret = %d", ezlopi_scenes_status_to_string(curr_scene->status), ret);
                    break;
                }
                curr_scene = curr_scene->next;
            }
        }
    }

    return ret;
}

int ezlopi_scene_when_is_group_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_group_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_cloud_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W(" isCloud_state ");
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block && scene_node)
    {
        l_fields_v2_t* value_field = NULL;
        l_fields_v2_t* curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "state", 6))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type && (NULL != curr_field->field_value.u_value.value_string))
                {
                    value_field = curr_field;
                }
            }
            curr_field = curr_field->next;
        }

        if (value_field)
        {
            if (0 < ezlopi_event_group_wait_for_event(EZLOPI_EVENT_NMA_REG, 100, false))
            {
                ret = (0 == strncmp(value_field->field_value.u_value.value_string, "connected", 10));
                TRACE_S("NMA_CONNECTED, ret = %d", ret);
            }
            else
            {
                ret = (0 == strncmp(value_field->field_value.u_value.value_string, "disconnected", 14));
                TRACE_S("NMA_DISCONNECTED, ret = %d", ret);
            }
        }
    }
    return ret;
}

int ezlopi_scene_when_is_battery_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_battery_state' not implemented!");
    return 0;
}

int ezlopi_scene_when_is_battery_level(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_battery_level' not implemented!");
    return 0;
}

int ezlopi_scene_when_compare_numbers(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W(" Compare_num ");
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;

    if (when_block && scene_node)
    {
        uint32_t item_id = 0;
        l_fields_v2_t* value_field = NULL;
        l_fields_v2_t* comparator_field = NULL;

        l_fields_v2_t* curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, ezlopi_item_str, strlen(ezlopi_item_str)))
            {
                item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
            }
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, strlen(ezlopi_value_str)))
            {
                value_field = curr_field;
            }
            else if (0 == strncmp(curr_field->name, ezlopi_comparator_str, strlen(ezlopi_comparator_str)))
            {
                comparator_field = curr_field;
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field && comparator_field)
        {
            ret = ezlopi_scenes_operators_value_number_operations(item_id, value_field, comparator_field);
        }
    }

    return ret;
}

int ezlopi_scene_when_compare_number_range(l_scenes_list_v2_t* scene_node, void* arg)
{
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;

    if (when_block && scene_node)
    {
        uint32_t item_id = 0;
        l_fields_v2_t* end_vlaue_field = NULL;
        l_fields_v2_t* start_value_field = NULL;

        l_fields_v2_t* curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, ezlopi_item_str, 4))
            {
                item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
            }
            else if (0 == strncmp(curr_field->name, ezlopi_startValue_str, strlen(ezlopi_startValue_str)))
            {
                start_value_field = curr_field;
            }
            else if (0 == strncmp(curr_field->name, ezlopi_endValue_str, strlen(ezlopi_endValue_str)))
            {
                end_vlaue_field = curr_field;
            }

            curr_field = curr_field->next;
        }

        if (item_id && end_vlaue_field && start_value_field)
        {
            double double_item_value = ezlopi_core_scenes_operator_get_item_double_value_current(item_id);
            if ((start_value_field->field_value.u_value.value_double <= double_item_value) &&
                (end_vlaue_field->field_value.u_value.value_double >= double_item_value))
            {
                ret = 1;
            }
        }
        else
        {
            TRACE_E("error args");
        }
    }

    return ret;
}

int ezlopi_scene_when_compare_strings(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W(" Compare_strings ");
    // TRACE_W("Warning: when-method 'compare_strings' not implemented!");
    int ret = 0;
    // #if 0
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block && scene_node)
    {
        uint32_t item_id = 0;
        // l_fields_v2_t *expression_field = NULL;
        l_fields_v2_t* value_field = NULL;
        l_fields_v2_t* comparator_field = NULL;

        l_fields_v2_t* curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 5))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16); // item or expression_id
                }
            }
            else if (0 == strncmp(curr_field->name, "comparator", 11))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    comparator_field = curr_field;
                }
            }
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, 6))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && NULL != curr_field->field_value.u_value.value_string)
                {
                    value_field = curr_field; // this field has string
                }
                // else if (EZLOPI_VALUE_TYPE_EXPRESSION == curr_field->value_type && NULL != curr_field->value.value_string)
                // {
                //     expression_field = curr_field; // this field has expression_name
                // }
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field && comparator_field) // only for item_value 'string comparisions'
        {
            ret = ezlopi_scenes_operators_value_strings_operations(item_id, value_field, comparator_field);
        }
        // else if (item_id && expression_field && comparator_field) // only for expression 'string comparisions'
        // {
        //     // ret = ezlopi_scenes_operators_value_expn_strings_operations(item_id, value_field, comparator_field);
        // }
    }
    // #endif
    return ret;
}

int ezlopi_scene_when_string_operation(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W(" Compare_string_op ");
    // TRACE_W("Warning: when-method 'string_operation' not implemented!");
    int ret = 0;
    // #if 0
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block && scene_node)
    {
        uint32_t item_id = 0;
        // l_fields_v2_t *expression_field = NULL;
        l_fields_v2_t* value_field = NULL;
        l_fields_v2_t* operation_field = NULL;

        l_fields_v2_t* curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 5))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16); // ID extraction [item or expression]
                }
            }
            else if (0 == strncmp(curr_field->name, "operation", 10))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    operation_field = curr_field;
                }
            }
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, 6))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && NULL != curr_field->field_value.u_value.value_string)
                {
                    value_field = curr_field; // this field has string
                }
                else if (EZLOPI_VALUE_TYPE_INT == curr_field->value_type)
                {
                    value_field = curr_field; // this field has double/int value
                }
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field && operation_field) // only for item_value 'string comparisions'
        {
            ret = ezlopi_scenes_operators_value_strings_operations(item_id, value_field, operation_field);
        }
        // else if (item_id && expression_field && operation_field) // only for expression 'string comparisions'
        // {
        //     // ret = ezlopi_scenes_operators_value_expn_strings_operations(item_id, value_field, operation_field);
        // }
    }
    // #endif
    return ret;
}

int ezlopi_scene_when_in_array(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W(" Compare_inArray ");
    // TRACE_W("Warning: when-method 'in_array' not implemented!");
    int ret = 0;
    // #if 0
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block && scene_node)
    {
        uint32_t item_id = 0;
        // l_fields_v2_t *expression_field = NULL;
        l_fields_v2_t* value_field = NULL;
        l_fields_v2_t* operation_field = NULL;

        l_fields_v2_t* curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 5))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16); // ID extraction [item or expression]
                }
            }
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, 6))
            {
                if (EZLOPI_VALUE_TYPE_ARRAY == curr_field->value_type && (cJSON_IsArray(curr_field->field_value.u_value.cj_value)))
                {
                    value_field = curr_field;
                }
            }
            else if (0 == strncmp(curr_field->name, "operation", 10))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    operation_field = curr_field;
                }
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field) // only for item_value 'string comparisions'
        {
            ret = ezlopi_scenes_operators_value_inarr_operations(item_id, value_field, operation_field);
        }
        // else if (item_id && expression_field && operation_field) // only for expression 'string comparisions'
        // {
        // ret = ezlopi_scenes_operators_value_expn_inarr_operations(item_id, value_field, operation_field);
        // }
    }
    // #endif
    return ret;
}

int ezlopi_scene_when_compare_values(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W(" Compare_values ");
    // TRACE_W("Warning: when-method 'is_group_state' not implemented!");
    int ret = 0;
    // #if 0
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block && scene_node)
    {
        uint32_t item_id = 0;
        l_fields_v2_t* value_type_field = NULL;
        l_fields_v2_t* value_field = NULL;
        l_fields_v2_t* comparator_field = NULL;
        // l_fields_v2_t *expression_field = NULL;

        l_fields_v2_t* curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 5))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16); // ID extraction [item or expression]
                }
            }

            else if (0 == strncmp(curr_field->name, ezlopi_value_type_str, 11))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    value_type_field = curr_field;
                }
            }
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, 6))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    value_field = curr_field;
                }
            }
            else if (0 == strncmp(curr_field->name, "comparator", 11))
            {
                if (EZLOPI_VALUE_TYPE_STRING == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    comparator_field = curr_field;
                }
            }
            curr_field = curr_field->next;
        }

        if (item_id && value_field && value_type_field) // only for item_value 'string comparisions'
        {
            ret = ezlopi_scenes_operators_value_comparevalues_with_less_operations(item_id, value_field, value_type_field, comparator_field);
        }
        // else if (item_id && expression_field ) // only for expression 'string comparisions'
        // {
        // ret = ezlopi_scenes_operators_expn_comparevalues_with_less_operations(item_id, value_field, comparator_field);
        // }
    }
    // #endif
    return ret;
}

int ezlopi_scene_when_has_atleast_one_dictionary_value(l_scenes_list_v2_t* scene_node, void* arg)
{
    //TRACE_W(" atleast_one_diction_val ");
    TRACE_W("Warning: when-method 'atleast_one_dictionary_value' not implemented!");
    int ret = 0;
#if 0
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (scene_node && when_block)
    {
        uint32_t item_id = 0;
        l_fields_v2_t* value_field = NULL;

        l_fields_v2_t* curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 5))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                }
            }
            else if (0 == strncmp(curr_field->name, ezlopi_value_str, 6))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    value_field = curr_field; // this contains "options [array]" & 'value': to be checked
                }
            }
            curr_field = curr_field->next;
        }

        // now to extract the
        if (item_id && value_field)
        {
            ret = ezlopi_scenes_operators_has_atleastone_dictionary_value_operations(item_id, value_field);
        }
    }
#endif
    return ret;
}

int ezlopi_scene_when_is_firmware_update_state(l_scenes_list_v2_t* scene_node, void* arg)
{
    //TRACE_W(" firmware_update ");
    TRACE_W("Warning: when-method 'firmware_update_state' not implemented!");
    int ret = 0;
#if 0
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (scene_node && when_block)
    {
        uint32_t item_id = 0;
        char* state_value = NULL;

        l_fields_v2_t* curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "state", 6))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    state_value = curr_field->field_value.u_value.value_string; // started / updating / done
                }
            }
            curr_field = curr_field->next;
        }

        // now to extract the
        if (item_id && (NULL != state_value))
        {
            if (0 == strncmp("done", state_value, 5) && (0 == __get_ota_state()))
            {
                ret = 1;
            }
            else if (0 == strncmp("started", state_value, 8) && (1 == __get_ota_state()))
            {
                ret = 1;
            }
            else if (0 == strncmp("updating", state_value, 9) && (2 == __get_ota_state()))
            {
                ret = 1;
            }
        }
    }
#endif
    return ret;
}

int ezlopi_scene_when_is_dictionary_changed(l_scenes_list_v2_t* scene_node, void* arg)
{
    //TRACE_W(" isDictionary_changed ");
    TRACE_W("Warning: when-method 'is_dictionary_changed' not implemented!");
    int ret = 0;
#if 0
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (scene_node && when_block)
    {
        uint32_t item_id = 0;
        l_fields_v2_t* key_field = NULL;
        l_fields_v2_t* operation_field = NULL;

        l_fields_v2_t* curr_field = when_block->fields;
        while (curr_field)
        {
            if (0 == strncmp(curr_field->name, "item", 5))
            {
                if (EZLOPI_VALUE_TYPE_ITEM == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    item_id = strtoul(curr_field->field_value.u_value.value_string, NULL, 16);
                }
            }
            else if (0 == strncmp(curr_field->name, "key", 4))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    key_field = curr_field; // this contains "options [array]" & 'value': to be checked
                }
            }
            else if (0 == strncmp(curr_field->name, "operation", 10))
            {
                if (EZLOPI_VALUE_TYPE_TOKEN == curr_field->value_type && (NULL != (curr_field->field_value.u_value.value_string)))
                {
                    operation_field = curr_field; // this contains "options [array]" & 'value': to be checked
                }
            }
            curr_field = curr_field->next;
        }

        if (item_id && key_field && operation_field)
        {
            ret = ezlopi_scenes_operators_is_dictionary_changed_operations(scene_node, item_id, key_field, operation_field);
        }
    }
#endif
    return ret;
}

int ezlopi_scene_when_is_detected_in_hot_zone(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method 'is_detected_in_hot_zone' not implemented!");
    return 0;
}

int ezlopi_scene_when_and(l_scenes_list_v2_t* scene_node, void* arg)
{
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block)
    {
        l_when_block_v2_t* value_when_block = when_block->fields->field_value.u_value.when_block;
        while (value_when_block)
        {
            f_scene_method_v2_t scene_method = ezlopi_scene_get_method_v2(value_when_block->block_options.method.type);
            if (scene_method)
            {
                ret &= scene_method(scene_node, (void*)value_when_block);
                if (!ret)
                {
                    break;
                }
            }

            value_when_block = value_when_block->next;
        }
    }
    TRACE_I("and => %d", ret);
    return ret;
}

int ezlopi_scene_when_not(l_scenes_list_v2_t* scene_node, void* arg)
{
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block)
    {
        ret = 1; // required for the starting 'not'
        l_when_block_v2_t* value_when_block = when_block->fields->field_value.u_value.when_block;
        while (value_when_block)
        {
            f_scene_method_v2_t scene_method = ezlopi_scene_get_method_v2(value_when_block->block_options.method.type);
            if (scene_method)
            {
                // TRACE_E("Method Name : %s", ezlopi_scene_get_scene_method_name(value_when_block->block_options.method.type));
                ret = !(scene_method(scene_node, (void*)value_when_block)); // if all the block-calls are false, then return 1;
            }

            value_when_block = value_when_block->next;
        }
    }

    return ret;
}

int ezlopi_scene_when_or(l_scenes_list_v2_t* scene_node, void* arg)
{
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block)
    {
        l_when_block_v2_t* value_when_block = when_block->fields->field_value.u_value.when_block;
        while (value_when_block)
        {
            f_scene_method_v2_t scene_method = ezlopi_scene_get_method_v2(value_when_block->block_options.method.type);
            if (scene_method)
            {
                ret |= scene_method(scene_node, (void*)value_when_block);
                if (ret)
                {
                    break;
                }
            }

            value_when_block = value_when_block->next;
        }
    }

    TRACE_I("or => %d", ret);
    return ret;
}

int ezlopi_scene_when_xor(l_scenes_list_v2_t* scene_node, void* arg)
{
    int ret = 0;
    l_when_block_v2_t* when_block = (l_when_block_v2_t*)arg;
    if (when_block)
    {
        l_when_block_v2_t* value_when_block = when_block->fields->field_value.u_value.when_block;
        while (value_when_block)
        {
            f_scene_method_v2_t scene_method = ezlopi_scene_get_method_v2(value_when_block->block_options.method.type);
            if (scene_method)
            {
                // iterate through all the '_when_blocks_'
                ret ^= scene_method(scene_node, (void*)value_when_block);
                // return 1 ; if odd no of '_when_block_' conditions are true
            }

            value_when_block = value_when_block->next;
        }
    }

    TRACE_I("xor => %d", ret);
    return ret;
}

int ezlopi_scene_when_function(l_scenes_list_v2_t* scene_node, void* arg)
{
    TRACE_W("Warning: when-method not implemented!");
    return 0;
}