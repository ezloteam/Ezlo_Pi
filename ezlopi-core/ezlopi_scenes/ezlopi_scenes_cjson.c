#include "string.h"
#include "cJSON.h"

#include "trace.h"
#include "ezlopi_scenes_v2.h"

static void ezlopi_scenes_cjson_add_action_block_options(cJSON *cj_block_array, l_action_block_v2_t *then_block);
static void ezlopi_scenes_cjson_add_action_delay(cJSON *cj_then_block, s_action_delay_v2_t *action_delay);
static void ezlopi_scenes_cjson_add_fields(cJSON *cj_block, l_fields_v2_t *fields);

static void ezlopi_scenes_cjson_add_string(cJSON *root, char *key, const char *value)
{
    if (root && key && value)
    {
        if (isprint(value[0]))
        {
            cJSON_AddStringToObject(root, key, value);
        }
        else
        {
            cJSON_AddNullToObject(root, key);
        }
    }
}

static void ezlopi_scenes_cjson_add_user_notifications(cJSON *root, l_user_notification_v2_t *user_notifications)
{
    if (root && user_notifications)
    {
        cJSON *cj_user_notifications_array = cJSON_AddArrayToObject(root, "user_notifications");
        if (cj_user_notifications_array)
        {
            while (user_notifications)
            {
                cJSON *array_item = cJSON_CreateString(user_notifications->user_id);
                if (array_item)
                {
                    if (!cJSON_AddItemToArray(cj_user_notifications_array, array_item))
                    {
                        cJSON_Delete(array_item);
                    }
                }
                user_notifications = user_notifications->next;
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_house_modes(cJSON *root, l_house_modes_v2_t *house_modes)
{
    if (root && house_modes)
    {
        cJSON *cj_user_notifications_array = cJSON_AddArrayToObject(root, "house_modes");
        if (cj_user_notifications_array)
        {
            while (house_modes)
            {
                cJSON *array_item = cJSON_CreateString(house_modes->house_mode);
                if (array_item)
                {
                    if (!cJSON_AddItemToArray(cj_user_notifications_array, array_item))
                    {
                        cJSON_Delete(array_item);
                    }
                }
                house_modes = house_modes->next;
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_when_block_options(cJSON *cj_block_array, l_when_block_v2_t *when_block)
{
    if (cj_block_array && when_block)
    {
        cJSON *cj_block_options = cJSON_AddObjectToObject(cj_block_array, "blockOptions");
        if (cj_block_options)
        {
            cJSON *cj_method = cJSON_AddObjectToObject(cj_block_options, "method");
            if (cj_method)
            {
                if ((when_block->block_options.method.type > EZLOPI_SCENE_METHOD_TYPE_NONE) &&
                    (when_block->block_options.method.type < EZLOPI_SCENE_METHOD_TYPE_MAX))
                {
                    const char *method_type_name = ezlopi_scene_get_scene_method_name(when_block->block_options.method.type);
                    ezlopi_scenes_cjson_add_string(cj_method, "name", method_type_name ? method_type_name : "");
                }
                else
                {
                    TRACE_E("Method type error");
                }

                cJSON *cj_args = cJSON_AddObjectToObject(cj_method, "args");
                if (cj_args)
                {
                    l_fields_v2_t *curr_field = when_block->fields;
                    while (curr_field)
                    {
                        cJSON_AddStringToObject(cj_args, curr_field->name, curr_field->name);
                        curr_field = curr_field->next;
                    }
                }
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_action_block_options(cJSON *cj_block_array, l_action_block_v2_t *action_block)
{
    if (cj_block_array && action_block)
    {

        cJSON *cj_block_options = cJSON_AddObjectToObject(cj_block_array, "blockOptions");
        if (cj_block_options)
        {

            cJSON *cj_method = cJSON_AddObjectToObject(cj_block_options, "method");
            if (cj_method)
            {
                if ((action_block->block_options.method.type > EZLOPI_SCENE_METHOD_TYPE_NONE) &&
                    (action_block->block_options.method.type < EZLOPI_SCENE_METHOD_TYPE_MAX))
                {
                    const char *method_type_name = ezlopi_scene_get_scene_method_name(action_block->block_options.method.type);
                    ezlopi_scenes_cjson_add_string(cj_method, "name", method_type_name ? method_type_name : "");
                }
                else
                {
                    TRACE_E("Method type error");
                }

                cJSON *cj_args = cJSON_AddObjectToObject(cj_method, "args");
                if (cj_args)
                {
                    l_fields_v2_t *curr_field = action_block->fields;
                    while (curr_field)
                    {
                        cJSON_AddStringToObject(cj_args, curr_field->name, curr_field->name);
                        curr_field = curr_field->next;
                    }
                }
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_action_delay(cJSON *cj_action_block, s_action_delay_v2_t *action_delay)
{
    if (cj_action_block && action_delay)
    {
        if (action_delay->days || action_delay->hours || action_delay->minutes || action_delay->seconds)
        {
            cJSON *cj_action_delay = cJSON_AddObjectToObject(cj_action_block, "delay");
            if (cj_action_delay)
            {
                cJSON_AddNumberToObject(cj_action_delay, "days", action_delay->days);
                cJSON_AddNumberToObject(cj_action_delay, "hours", action_delay->hours);
                cJSON_AddNumberToObject(cj_action_delay, "minutes", action_delay->minutes);
                cJSON_AddNumberToObject(cj_action_delay, "seconds", action_delay->seconds);
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_fields(cJSON *cj_block, l_fields_v2_t *fields)
{
    if (cj_block)
    {
        cJSON *cj_fields_array = cJSON_AddArrayToObject(cj_block, "fields");
        if (cj_fields_array)
        {
            l_fields_v2_t *curr_field = fields;
            while (curr_field)
            {
                cJSON *cj_field = cJSON_CreateObject();
                if (cj_field)
                {
                    ezlopi_scenes_cjson_add_string(cj_field, "name", curr_field->name);
                    const char *value_type_name = ezlopi_scene_get_scene_value_type_name_v2(curr_field->value_type);
                    ezlopi_scenes_cjson_add_string(cj_field, "type", value_type_name ? value_type_name : "");

                    switch (curr_field->value_type)
                    {
                    case EZLOPI_VALUE_TYPE_NONE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_INT:
                    {
                        cJSON_AddNumberToObject(cj_field, "value", curr_field->value.value_double);
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_BOOL:
                    {
                        if (0 == curr_field->value.value_double)
                        {
                            cJSON_AddFalseToObject(cj_field, "value");
                        }
                        else
                        {
                            cJSON_AddTrueToObject(cj_field, "value");
                        }
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_FLOAT:
                    {
                        cJSON_AddNumberToObject(cj_field, "value", curr_field->value.value_double);
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_STRING:
                    {
                        ezlopi_scenes_cjson_add_string(cj_field, "value", curr_field->value.value_string);
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_ITEM:
                    {
                        ezlopi_scenes_cjson_add_string(cj_field, "value", curr_field->value.value_string);
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_INTERVAL:
                    {
                        ezlopi_scenes_cjson_add_string(cj_field, "value", curr_field->value.value_string);
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_BLOCKS:
                    {
                        cJSON *vlaue_block_array = cJSON_AddArrayToObject(cj_field, "value");
                        if (vlaue_block_array)
                        {
                            l_when_block_v2_t *curr_when_block = curr_field->value.when_block;
                            while (curr_when_block)
                            {
                                cJSON *cj_when_block = NULL;
                                // ezlopi_scenes_cjson_create_when_block(curr_when_block);
                                if (cj_when_block)
                                {
                                    if (!cJSON_AddItemToArray(vlaue_block_array, cj_when_block))
                                    {
                                        cJSON_Delete(cj_when_block);
                                    }
                                }
                                curr_when_block = curr_when_block->next;
                            }

                            char *str_vlaue = cJSON_Print(vlaue_block_array);
                            if (str_vlaue)
                            {
                                TRACE_I("value: %s", str_vlaue);
                                free(str_vlaue);
                            }
                        }
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_DICTIONARY:
                    case EZLOPI_VALUE_TYPE_ARRAY:
                    case EZLOPI_VALUE_TYPE_RGB:
                    case EZLOPI_VALUE_TYPE_CAMERA_STREAM:
                    case EZLOPI_VALUE_TYPE_USER_CODE:
                    case EZLOPI_VALUE_TYPE_WEEKLY_INTERVAL:
                    case EZLOPI_VALUE_TYPE_DAILY_INTERVAL:
                    case EZLOPI_VALUE_TYPE_TOKEN:
                    case EZLOPI_VALUE_TYPE_BUTTON_STATE:
                    case EZLOPI_VALUE_TYPE_USER_LOCK_OPERATION:
                    case EZLOPI_VALUE_TYPE_USER_CODE_ACTION:
                    case EZLOPI_VALUE_TYPE_SOUND_INFO:
                    case EZLOPI_VALUE_TYPE_CAMERA_HOTZONE:
                    case EZLOPI_VALUE_TYPE_HOTZONE_MATCH:
                    case EZLOPI_VALUE_TYPE_GEOFENCE:
                    case EZLOPI_VALUE_TYPE_ILLUMINANCE:
                    case EZLOPI_VALUE_TYPE_PRESSURE:
                    case EZLOPI_VALUE_TYPE_SUBSTANCE_AMOUNT:
                    case EZLOPI_VALUE_TYPE_POWER:
                    case EZLOPI_VALUE_TYPE_VELOCITY:
                    case EZLOPI_VALUE_TYPE_ACCELERATION:
                    case EZLOPI_VALUE_TYPE_DIRECTION:
                    case EZLOPI_VALUE_TYPE_GENERAL_PURPOSE:
                    case EZLOPI_VALUE_TYPE_ACIDITY:
                    case EZLOPI_VALUE_TYPE_ELECTRIC_POTENTIAL:
                    case EZLOPI_VALUE_TYPE_ELECTRIC_CURRENT:
                    case EZLOPI_VALUE_TYPE_FORCE:
                    case EZLOPI_VALUE_TYPE_IRRADIANCE:
                    case EZLOPI_VALUE_TYPE_PRECIPITATION:
                    case EZLOPI_VALUE_TYPE_LENGTH:
                    case EZLOPI_VALUE_TYPE_MASS:
                    case EZLOPI_VALUE_TYPE_VOLUME_FLOW:
                    case EZLOPI_VALUE_TYPE_VOLUME:
                    case EZLOPI_VALUE_TYPE_ANGLE:
                    case EZLOPI_VALUE_TYPE_FREQUENCY:
                    case EZLOPI_VALUE_TYPE_SEISMIC_INTENSITY:
                    case EZLOPI_VALUE_TYPE_SEISMIC_MAGNITUDE:
                    case EZLOPI_VALUE_TYPE_ULTRAVIOLET:
                    case EZLOPI_VALUE_TYPE_ELECTRICAL_RESISTANCE:
                    case EZLOPI_VALUE_TYPE_ELECTRICAL_CONDUCTIVITY:
                    case EZLOPI_VALUE_TYPE_LOUDNESS:
                    case EZLOPI_VALUE_TYPE_MOISTURE:
                    case EZLOPI_VALUE_TYPE_TIME:
                    case EZLOPI_VALUE_TYPE_RADON_CONCENTRATION:
                    case EZLOPI_VALUE_TYPE_BLOOD_PRESSURE:
                    case EZLOPI_VALUE_TYPE_ENERGY:
                    case EZLOPI_VALUE_TYPE_RF_SIGNAL_STRENGTH:
                    case EZLOPI_VALUE_TYPE_TEMPERATURE:
                    case EZLOPI_VALUE_TYPE_HUMIDITY:
                    case EZLOPI_VALUE_TYPE_KILO_VOLT_AMPERE_HOUR:
                    case EZLOPI_VALUE_TYPE_REACTIVE_POWER_INSTANT:
                    case EZLOPI_VALUE_TYPE_AMOUNT_OF_USEFUL_ENERGY:
                    case EZLOPI_VALUE_TYPE_REACTIVE_POWER_CONSUMPTION:
                    case EZLOPI_VALUE_TYPE_DEVICE:
                    case EZLOPI_VALUE_TYPE_EXPRESSION:
                    case EZLOPI_VALUE_TYPE_24_HOURS_TIME:
                    case EZLOPI_VALUE_TYPE_24_HOURS_TIME_ARRAY:
                    case EZLOPI_VALUE_TYPE_INT_ARRAY:
                    case EZLOPI_VALUE_TYPE_HMS_INTERVAL:
                    case EZLOPI_VALUE_TYPE_MAX:
                    {
                        TRACE_W("Value type not implemented!, curr-type: %d", curr_field->value_type);
                        break;
                    }
                    default:
                    {
                        const char *value_type_name = ezlopi_scene_get_scene_value_type_name_v2(curr_field->value_type);
                        TRACE_E("Value type not matched!, curr-type[%d]: %s ", curr_field->value_type, value_type_name ? value_type_name : "null");
                        break;
                    }
                    }
#if 0
                    case SCENE_VALUE_TYPE_BOOL:
                    {
                        ezlopi_scenes_cjson_add_string(cj_field, "type", "bool");
                        if (0 == curr_field->value.value_double)
                        {
                            cJSON_AddFalseToObject(cj_field, "value");
                        }
                        else
                        {
                            cJSON_AddTrueToObject(cj_field, "value");
                        }
                        break;
                    }
                    case SCENE_VALUE_TYPE_INT:
                    {
                        ezlopi_scenes_cjson_add_string(cj_field, "type", "int");
                        cJSON_AddNumberToObject(cj_field, "value", curr_field->value.value_double);
                        break;
                    }
                    case SCENE_VALUE_TYPE_ITEM:
                    {
                        ezlopi_scenes_cjson_add_string(cj_field, "type", "item");
                        ezlopi_scenes_cjson_add_string(cj_field, "value", curr_field->value.value_string);
                        break;
                    }
                    default:
                    {
                        break;
                    }

#endif

                    if (!cJSON_AddItemToArray(cj_fields_array, cj_field))
                    {
                        cJSON_Delete(cj_field);
                    }
                }

                curr_field = curr_field->next;
            }
        }
    }
}

cJSON *ezlopi_scenes_cjson_create_action_block(l_action_block_v2_t *action_block, char *block_type_str)
{
    cJSON *cj_action_block = NULL;
    if (action_block)
    {
        cj_action_block = cJSON_CreateObject();
        if (cj_action_block)
        {
            ezlopi_scenes_cjson_add_action_block_options(cj_action_block, action_block);
            ezlopi_scenes_cjson_add_string(cj_action_block, "blockType", block_type_str);
            ezlopi_scenes_cjson_add_action_delay(cj_action_block, &action_block->delay);
            ezlopi_scenes_cjson_add_fields(cj_action_block, action_block->fields);
        }
    }

    return cj_action_block;
}

void ezlopi_scenes_cjson_add_action_blocks(cJSON *root, l_action_block_v2_t *action_blocks, char *block_type_str)
{
    if (root && action_blocks)
    {
        cJSON *cj_then_block_array = cJSON_AddArrayToObject(root, block_type_str);

        if (cj_then_block_array)
        {
            while (action_blocks)
            {
                cJSON *cj_then_block = NULL;
                ezlopi_scenes_cjson_create_action_block(action_blocks, block_type_str);
                if (cj_then_block)
                {
                    if (!cJSON_AddItemToArray(cj_then_block_array, cj_then_block))
                    {
                        cJSON_Delete(cj_then_block);
                    }
                }

                action_blocks = action_blocks->next;
            }
        }
    }
}

cJSON *ezlopi_scenes_cjson_create_when_block(l_when_block_v2_t *when_block)
{
    cJSON *cj_when_block = NULL;
    if (when_block)
    {
        cj_when_block = cJSON_CreateObject();
        if (cj_when_block)
        {
            ezlopi_scenes_cjson_add_when_block_options(cj_when_block, when_block);
            ezlopi_scenes_cjson_add_string(cj_when_block, "blockType", "when");
            ezlopi_scenes_cjson_add_fields(cj_when_block, when_block->fields);
        }
    }

    return cj_when_block;
}

void ezlopi_scenes_cjson_add_when_blocks(cJSON *root, l_when_block_v2_t *when_blocks)
{
    if (root && when_blocks)
    {
        cJSON *cj_when_block_array = cJSON_AddArrayToObject(root, "when");
        if (cj_when_block_array)
        {
            while (when_blocks)
            {
                cJSON *cj_when_block = NULL;
                // ezlopi_scenes_cjson_create_when_block(when_blocks);
                if (cj_when_block)
                {
                    if (!cJSON_AddItemToArray(cj_when_block_array, cj_when_block))
                    {
                        cJSON_Delete(cj_when_block);
                    }
                }

                when_blocks = when_blocks->next;
            }
        }
    }
}

cJSON *ezlopi_scenes_create_cjson_scene(l_scenes_list_v2_t *scene)
{
    cJSON *cj_scene = NULL;
    if (scene)
    {
        cj_scene = cJSON_CreateObject();
        if (cj_scene)
        {
            char tmp_str[16] = {0};
            snprintf(tmp_str, sizeof(tmp_str), "%08x", scene->_id);
            // cJSON_AddNumberToObject(cj_scene, "_id", scene->_id);
            cJSON_AddStringToObject(cj_scene, "_id", tmp_str);
            cJSON_AddBoolToObject(cj_scene, "enabled", scene->enabled);
            ezlopi_scenes_cjson_add_string(cj_scene, "group_id", scene->group_id);
            cJSON_AddBoolToObject(cj_scene, "is_group", scene->is_group);
            ezlopi_scenes_cjson_add_string(cj_scene, "name", scene->name);
            ezlopi_scenes_cjson_add_string(cj_scene, "parent_id", scene->parent_id);

            ezlopi_scenes_cjson_add_user_notifications(cj_scene, scene->user_notifications);
            ezlopi_scenes_cjson_add_house_modes(cj_scene, scene->house_modes);

            ezlopi_scenes_cjson_add_action_blocks(cj_scene, scene->then_block, "then");
            ezlopi_scenes_cjson_add_when_blocks(cj_scene, scene->when_block);
            ezlopi_scenes_cjson_add_action_blocks(cj_scene, scene->else_block, "else");
        }
    }

    return cj_scene;
}

cJSON *ezlopi_scenes_create_cjson_scene_list(l_scenes_list_v2_t *scenes_list)
{
    cJSON *cj_scenes_array = cJSON_CreateArray();
    if (cj_scenes_array)
    {
        while (scenes_list)
        {
            cJSON *cj_scene = ezlopi_scenes_create_cjson_scene(scenes_list);
            if (cj_scene)
            {
                if (!cJSON_AddItemToArray(cj_scenes_array, cj_scene))
                {
                    cJSON_Delete(cj_scene);
                }
            }
            scenes_list = scenes_list->next;
        }
    }

    return cj_scenes_array;
}

char *ezlopi_scenes_create_json_string(l_scenes_list_v2_t *scenes_list)
{
    char *scenes_list_str = NULL;

    cJSON *cj_scenes_array = ezlopi_scenes_create_cjson_scene_list(scenes_list);
    if (cj_scenes_array)
    {
        scenes_list_str = cJSON_Print(cj_scenes_array);
        if (scenes_list_str)
        {
            cJSON_Minify(scenes_list_str);
        }
        cJSON_Delete(cj_scenes_array);
    }
    return scenes_list_str;
}