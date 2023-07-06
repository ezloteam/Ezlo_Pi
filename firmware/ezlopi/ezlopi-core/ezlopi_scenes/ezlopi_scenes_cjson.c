#include "string.h"
#include "cJSON.h"

#include "trace.h"
#include "ezlopi_scenes.h"

static const char *ezlopi_scenes_methods_name[] = {
#define EZLOPI_SCENE(method_type, name, func) name,
#include "ezlopi_scenes_method_types.h"
#undef EZLOPI_SCENE
};

static void ezlopi_scenes_cjson_add_string(cJSON *root, char *key, char *value)
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

static void ezlopi_scenes_cjson_add_user_notifications(cJSON *root, l_user_notification_t *user_notifications)
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

static void ezlopi_scenes_cjson_add_house_modes(cJSON *root, l_house_modes_t *house_modes)
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

static void ezlopi_scenes_cjson_add_when_block_options(cJSON *cj_block_array, l_when_block_t *when_block)
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
                    TRACE_E("Method: %s", ezlopi_scenes_methods_name[when_block->block_options.method.type]);
                    ezlopi_scenes_cjson_add_string(cj_method, "name", ezlopi_scenes_methods_name[when_block->block_options.method.type]);
                }
                else
                {
                    TRACE_E("Method type error");
                }

                cJSON *cj_args = cJSON_AddObjectToObject(cj_method, "args");
                if (cj_args)
                {
                    l_fields_t *curr_field = when_block->fields;
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

static void ezlopi_scenes_cjson_add_then_block_options(cJSON *cj_block_array, l_then_block_t *then_block)
{
    if (cj_block_array && then_block)
    {

        cJSON *cj_block_options = cJSON_AddObjectToObject(cj_block_array, "blockOptions");
        if (cj_block_options)
        {

            cJSON *cj_method = cJSON_AddObjectToObject(cj_block_options, "method");
            if (cj_method)
            {
                if ((then_block->block_options.method.type > EZLOPI_SCENE_METHOD_TYPE_NONE) &&
                    (then_block->block_options.method.type < EZLOPI_SCENE_METHOD_TYPE_MAX))
                {
                    TRACE_E("Method: %s", ezlopi_scenes_methods_name[then_block->block_options.method.type]);
                    ezlopi_scenes_cjson_add_string(cj_method, "name", ezlopi_scenes_methods_name[then_block->block_options.method.type]);
                }
                else
                {
                    TRACE_E("Method type error");
                }

                cJSON *cj_args = cJSON_AddObjectToObject(cj_method, "args");
                if (cj_args)
                {
                    l_fields_t *curr_field = then_block->fields;
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

static void ezlopi_scenes_cjson_add_action_delay(cJSON *cj_then_block, s_action_delay_t *action_delay)
{
    if (cj_then_block && action_delay)
    {
        if (action_delay->days || action_delay->hours || action_delay->minutes || action_delay->seconds)
        {
            cJSON *cj_action_delay = cJSON_AddObjectToObject(cj_then_block, "delay");
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

static void ezlopi_scenes_cjson_add_fields(cJSON *cj_block, l_fields_t *fields)
{
    if (cj_block)
    {
        cJSON *cj_fields_array = cJSON_AddArrayToObject(cj_block, "fields");
        if (cj_fields_array)
        {
            l_fields_t *curr_field = fields;
            while (curr_field)
            {
                cJSON *cj_field = cJSON_CreateObject();
                if (cj_field)
                {
                    ezlopi_scenes_cjson_add_string(cj_field, "name", curr_field->name);
                    switch (curr_field->value_type)
                    {
                    case EZLOPI_VALUE_TYPE_NONE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_INT:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_BOOL:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_FLOAT:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_STRING:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_DICTIONARY:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_ARRAY:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_RGB:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_CAMERA_STREAM:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_USER_CODE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_WEEKLY_INTERVAL:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_DAILY_INTERVAL:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_TOKEN:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_BUTTON_STATE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_USER_LOCK_OPERATION:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_USER_CODE_ACTION:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_SOUND_INFO:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_CAMERA_HOTZONE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_HOTZONE_MATCH:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_GEOFENCE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_ILLUMINANCE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_PRESSURE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_SUBSTANCE_AMOUNT:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_POWER:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_VELOCITY:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_ACCELERATION:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_DIRECTION:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_GENERAL_PURPOSE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_ACIDITY:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_ELECTRIC_POTENTIAL:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_ELECTRIC_CURRENT:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_FORCE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_IRRADIANCE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_PRECIPITATION:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_LENGTH:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_MASS:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_VOLUME_FLOW:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_VOLUME:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_ANGLE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_FREQUENCY:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_SEISMIC_INTENSITY:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_SEISMIC_MAGNITUDE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_ULTRAVIOLET:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_ELECTRICAL_RESISTANCE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_ELECTRICAL_CONDUCTIVITY:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_LOUDNESS:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_MOISTURE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_TIME:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_RADON_CONCENTRATION:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_BLOOD_PRESSURE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_ENERGY:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_RF_SIGNAL_STRENGTH:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_TEMPERATURE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_HUMIDITY:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_KILO_VOLT_AMPERE_HOUR:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_REACTIVE_POWER_INSTANT:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_AMOUNT_OF_USEFUL_ENERGY:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_REACTIVE_POWER_CONSUMPTION:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_DEVICE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_ITEM:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_EXPRESSION:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_24_HOURS_TIME:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_24_HOURS_TIME_ARRAY:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_INT_ARRAY:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_INTERVAL:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_HMS_INTERVAL:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_MAX:
                    {
                        break;
                    }
                    default:
                    {
                        TRACE_E("Value type not matched!");
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

cJSON *ezlopi_scenes_cjson_create_then_block(l_then_block_t *then_block)
{
    cJSON *cj_then_block = NULL;
    if (then_block)
    {
        cj_then_block = cJSON_CreateObject();
        if (cj_then_block)
        {
            ezlopi_scenes_cjson_add_then_block_options(cj_then_block, then_block);
            ezlopi_scenes_cjson_add_string(cj_then_block, "blockType", "then");
            ezlopi_scenes_cjson_add_action_delay(cj_then_block, &then_block->delay);
            ezlopi_scenes_cjson_add_fields(cj_then_block, then_block->fields);
        }
    }

    return cj_then_block;
}

void ezlopi_scenes_cjson_add_then_blocks(cJSON *root, l_then_block_t *then_blocks)
{
    if (root && then_blocks)
    {
        cJSON *cj_then_block_array = cJSON_AddArrayToObject(root, "then");
        if (cj_then_block_array)
        {
            while (then_blocks)
            {
                cJSON *cj_then_block = ezlopi_scenes_cjson_create_then_block(then_blocks);
                if (cj_then_block)
                {
                    if (!cJSON_AddItemToArray(cj_then_block_array, cj_then_block))
                    {
                        cJSON_Delete(cj_then_block);
                    }
                }

                then_blocks = then_blocks->next;
            }
        }
    }
}

cJSON *ezlopi_scenes_cjson_create_when_block(l_when_block_t *when_block)
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

void ezlopi_scenes_cjson_add_when_blocks(cJSON *root, l_when_block_t *when_blocks)
{
    if (root && when_blocks)
    {
        cJSON *cj_when_block_array = cJSON_AddArrayToObject(root, "when");
        if (cj_when_block_array)
        {
            while (when_blocks)
            {
                cJSON *cj_when_block = ezlopi_scenes_cjson_create_when_block(when_blocks);
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

cJSON *ezlopi_scenes_create_cjson_scene(l_scenes_list_t *scene)
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

            ezlopi_scenes_cjson_add_then_blocks(cj_scene, scene->then);

            ezlopi_scenes_cjson_add_when_blocks(cj_scene, scene->when);
        }
    }

    return cj_scene;
}

cJSON *ezlopi_scenes_create_cjson_scene_list(l_scenes_list_t *scenes_list)
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

char *ezlopi_scenes_create_json_string(l_scenes_list_t *scenes_list)
{
    char *scenes_list_str = NULL;

    cJSON *cj_scenes_array = ezlopi_scenes_create_cjson_scene_list(scenes_list);
    if (cj_scenes_array)
    {

        char *scenes_list_str = cJSON_Print(cj_scenes_array);
        if (scenes_list_str)
        {

            TRACE_D("scenes_list_str: \r\n%s", scenes_list_str);
            cJSON_Minify(scenes_list);
        }

        cJSON_Delete(cj_scenes_array);
    }
    return scenes_list_str;
}