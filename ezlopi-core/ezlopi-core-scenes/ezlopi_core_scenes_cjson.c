#include "../../build/config/sdkconfig.h"

#ifdef CONFIG_EZPI_SERV_ENABLE_MESHBOTS

#include <string.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_scenes_cjson.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_scenes_value.h"

#include "ezlopi_cloud_constants.h"

static void __cjson_add_fields(cJSON *cj_block, l_fields_v2_t *fields);
static void __cjson_add_string(cJSON *root, const char *key, const char *value);
static void __cjson_add_action_delay(cJSON *cj_then_block, s_action_delay_v2_t *action_delay);
static void __cjson_add_action_block_options(cJSON *cj_block_array, l_action_block_v2_t *then_block);

cJSON *ezlopi_scene_cjson_get_field(l_fields_v2_t *field_node)
{
    cJSON *cj_field = NULL;
    if (field_node)
    {
        cj_field = cJSON_CreateObject(__FUNCTION__);
        {
            const char *value_type_str = ezlopi_scene_get_scene_value_type_name(field_node->value_type);
            if (value_type_str)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_field, ezlopi_type_str, value_type_str);
                cJSON_AddStringToObject(__FUNCTION__, cj_field, ezlopi_name_str, field_node->name);
            }

            if (field_node->scale)
            {
                cJSON_AddStringToObject(__FUNCTION__, cj_field, ezlopi_scale_str, field_node->scale);
            }

            switch (field_node->value_type)
            {
            case EZLOPI_VALUE_TYPE_INT:
            case EZLOPI_VALUE_TYPE_FLOAT:
            case EZLOPI_VALUE_TYPE_TEMPERATURE:
            case EZLOPI_VALUE_TYPE_HUMIDITY:
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
            case EZLOPI_VALUE_TYPE_KILO_VOLT_AMPERE_HOUR:
            case EZLOPI_VALUE_TYPE_REACTIVE_POWER_INSTANT:
            case EZLOPI_VALUE_TYPE_AMOUNT_OF_USEFUL_ENERGY:
            case EZLOPI_VALUE_TYPE_REACTIVE_POWER_CONSUMPTION:
            {
                cJSON_AddNumberToObject(__FUNCTION__, cj_field, ezlopi_value_str, field_node->field_value.u_value.value_double);
                break;
            }
            case EZLOPI_VALUE_TYPE_BOOL:
            {
                if (0 == field_node->field_value.u_value.value_double)
                {
                    cJSON_AddFalseToObject(__FUNCTION__, cj_field, ezlopi_value_str);
                }
                else
                {
                    cJSON_AddTrueToObject(__FUNCTION__, cj_field, ezlopi_value_str);
                }
                break;
            }
            case EZLOPI_VALUE_TYPE_ITEM:
            case EZLOPI_VALUE_TYPE_DEVICE:
            case EZLOPI_VALUE_TYPE_STRING:
            case EZLOPI_VALUE_TYPE_INTERVAL:
            case EZLOPI_VALUE_TYPE_SCENEID:
            case EZLOPI_VALUE_TYPE_EXPRESSION:
            case EZLOPI_VALUE_TYPE_HOUSE_MODE_ID:
            {
                __cjson_add_string(cj_field, ezlopi_value_str, field_node->field_value.u_value.value_string);
                break;
            }
            case EZLOPI_VALUE_TYPE_BLOCK:
            {
                TRACE_E(" extracting : __ EZLOPI_VALUE_TYPE_BLOCK __");
                l_when_block_v2_t *curr_when_block = field_node->field_value.u_value.when_block; // cj_value?
                if (curr_when_block)
                {
                    cJSON *cj_when_block = NULL;
                    cj_when_block = ezlopi_scenes_cjson_create_when_block(curr_when_block);
                    if (cj_when_block)
                    {
                        TRACE_S(" adding : __ EZLOPI_VALUE_TYPE_BLOCK __");
                        CJSON_TRACE(ezlopi_value_str, cj_when_block);
                        cJSON_AddItemReferenceToObject(__FUNCTION__, cj_field, ezlopi_value_str, cj_when_block);
                    }
                }
                break;
            }
            case EZLOPI_VALUE_TYPE_BLOCKS:
            {
                cJSON *value_block_array = cJSON_AddArrayToObject(__FUNCTION__, cj_field, ezlopi_value_str);
                if (value_block_array)
                {
                    l_when_block_v2_t *curr_when_block = field_node->field_value.u_value.when_block;
                    while (curr_when_block)
                    {
                        cJSON *cj_when_block = ezlopi_scenes_cjson_create_when_block(curr_when_block);
                        if (cj_when_block)
                        {
                            if (!cJSON_AddItemToArray(value_block_array, cj_when_block))
                            {
                                cJSON_Delete(__FUNCTION__, cj_when_block);
                            }
                        }
                        curr_when_block = curr_when_block->next;
                    }

                    CJSON_TRACE(ezlopi_value_str, value_block_array);
                }
                break;
            }
            case EZLOPI_VALUE_TYPE_ENUM:
            case EZLOPI_VALUE_TYPE_TOKEN:
            {
                if (field_node->field_value.e_type == VALUE_TYPE_STRING)
                {
                    __cjson_add_string(cj_field, ezlopi_value_str, field_node->field_value.u_value.value_string);
                }
                else if (field_node->field_value.e_type == VALUE_TYPE_CJSON)
                {
                    cJSON_AddItemToObject(__FUNCTION__, cj_field, ezlopi_value_str, cJSON_Duplicate(__FUNCTION__, field_node->field_value.u_value.cj_value, 1));
                }
                break;
            }
            case EZLOPI_VALUE_TYPE_CREDENTIAL:
            case EZLOPI_VALUE_TYPE_DICTIONARY:
            case EZLOPI_VALUE_TYPE_WEEKLY_INTERVAL:
            case EZLOPI_VALUE_TYPE_DAILY_INTERVAL:
            case EZLOPI_VALUE_TYPE_ARRAY:
            case EZLOPI_VALUE_TYPE_24_HOURS_TIME:
            case EZLOPI_VALUE_TYPE_24_HOURS_TIME_ARRAY:
            case EZLOPI_VALUE_TYPE_INT_ARRAY:
            case EZLOPI_VALUE_TYPE_HMS_INTERVAL:
            case EZLOPI_VALUE_TYPE_RGB:
            case EZLOPI_VALUE_TYPE_OBJECT:
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_field, ezlopi_value_str, cJSON_Duplicate(__FUNCTION__, field_node->field_value.u_value.cj_value, 1));
                break;
            }
            case EZLOPI_VALUE_TYPE_CAMERA_STREAM:
            case EZLOPI_VALUE_TYPE_USER_CODE:
            case EZLOPI_VALUE_TYPE_BUTTON_STATE:
            case EZLOPI_VALUE_TYPE_USER_LOCK_OPERATION:
            case EZLOPI_VALUE_TYPE_USER_CODE_ACTION:
            case EZLOPI_VALUE_TYPE_SOUND_INFO:
            case EZLOPI_VALUE_TYPE_CAMERA_HOTZONE:
            case EZLOPI_VALUE_TYPE_HOTZONE_MATCH:
            case EZLOPI_VALUE_TYPE_GEOFENCE:
            case EZLOPI_VALUE_TYPE_NONE:
            case EZLOPI_VALUE_TYPE_MAX:
            {
                TRACE_W("Value type not implemented!, curr-type: %d", field_node->value_type);
                break;
            }
            default:
            {
#if (1 == ENABLE_TRACE)
                const char *value_type_name = ezlopi_scene_get_scene_value_type_name(field_node->value_type);
                TRACE_E("Value type not matched!, curr-type[%d]: %s ", field_node->value_type, value_type_name ? value_type_name : ezlopi_null_str);
#endif
                break;
            }
            }
        }
    }

    return cj_field;
}

static void ezlopi_scenes_cjson_add_user_notifications(cJSON *root, l_user_notification_v2_t *user_notifications)
{
    if (root)
    {
        cJSON *cj_user_notifications_array = cJSON_AddArrayToObject(__FUNCTION__, root, ezlopi_user_notifications_str);
        if (cj_user_notifications_array)
        {
            while (user_notifications)
            {
                cJSON *array_item = cJSON_CreateString(__FUNCTION__, user_notifications->user_id);
                if (array_item)
                {
                    if (!cJSON_AddItemToArray(cj_user_notifications_array, array_item))
                    {
                        cJSON_Delete(__FUNCTION__, array_item);
                    }
                }
                user_notifications = user_notifications->next;
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_house_modes(cJSON *root, l_house_modes_v2_t *house_modes)
{
    if (root)
    {
        cJSON *cj_user_notifications_array = cJSON_AddArrayToObject(__FUNCTION__, root, ezlopi_house_modes_str);
        if (cj_user_notifications_array)
        {
            while (house_modes)
            {
                cJSON *array_item = cJSON_CreateString(__FUNCTION__, house_modes->house_mode);
                if (array_item)
                {
                    if (!cJSON_AddItemToArray(cj_user_notifications_array, array_item))
                    {
                        cJSON_Delete(__FUNCTION__, array_item);
                    }
                }
                house_modes = house_modes->next;
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_when_block_options(cJSON *cj_when_block, l_when_block_v2_t *when_block)
{
    if (cj_when_block && when_block)
    {
        cJSON *cj_block_options = cJSON_AddObjectToObject(__FUNCTION__, cj_when_block, ezlopi_blockOptions_str);
        if (cj_block_options)
        {
            cJSON *cj_method = cJSON_AddObjectToObject(__FUNCTION__, cj_block_options, ezlopi_method_str);
            if (cj_method)
            {
                if ((when_block->block_options.method.type > EZLOPI_SCENE_METHOD_TYPE_NONE) &&
                    (when_block->block_options.method.type < EZLOPI_SCENE_METHOD_TYPE_MAX))
                {
                    const char *method_type_name = ezlopi_scene_get_scene_method_name(when_block->block_options.method.type);
                    __cjson_add_string(cj_method, ezlopi_name_str, method_type_name ? method_type_name : ezlopi__str);
                }
                else
                {
                    TRACE_E("Method type error");
                }

                cJSON *cj_args = cJSON_AddObjectToObject(__FUNCTION__, cj_method, ezlopi_args_str);
                if (cj_args)
                {
                    l_fields_v2_t *curr_field = when_block->fields;
                    while (curr_field)
                    {
                        cJSON_AddStringToObject(__FUNCTION__, cj_args, curr_field->name, curr_field->name);
                        curr_field = curr_field->next;
                    }
                }
            }
        }
    }
}

static void ezlopi_scenes_cjson_add_when_group_info(cJSON *cj_when_block, l_when_block_v2_t *when_block)
{
    if (cj_when_block && when_block)
    {
        if ((when_block->when_grp) && (0 < strlen(when_block->when_grp->grp_blockName)))
        {
            __cjson_add_string(cj_when_block, ezlopi_blockName_str, when_block->when_grp->grp_blockName); // group_blockName
            char tmp_str[16] = {0};
            snprintf(tmp_str, sizeof(tmp_str), "%08x", when_block->when_grp->grp_id);
            cJSON_AddBoolToObject(__FUNCTION__, cj_when_block, ezlopi_is_group_str, true); // is_group flag
            __cjson_add_string(cj_when_block, ezlopi_group_id_str, tmp_str);               // group_id
        }
    }
}

static void ezlopi_scenes_cjson_add_when_block_info(cJSON *cj_when_block, l_when_block_v2_t *when_block)
{
    if (cj_when_block && when_block)
    {
        if (0 < when_block->blockId)
        {
            cJSON_AddBoolToObject(__FUNCTION__, cj_when_block, ezlopi_block_enable_str, when_block->block_enable);
            char tmp_str[16] = {0};
            snprintf(tmp_str, sizeof(tmp_str), "%08x", when_block->blockId);
            __cjson_add_string(cj_when_block, ezlopi_blockId_str, tmp_str);
        }
    }
}

static void __cjson_add_action_block_options(cJSON *cj_block_array, l_action_block_v2_t *action_block)
{
    if (cj_block_array && action_block)
    {

        cJSON *cj_block_options = cJSON_AddObjectToObject(__FUNCTION__, cj_block_array, ezlopi_blockOptions_str);
        if (cj_block_options)
        {

            cJSON *cj_method = cJSON_AddObjectToObject(__FUNCTION__, cj_block_options, ezlopi_method_str);
            if (cj_method)
            {
                if ((action_block->block_options.method.type > EZLOPI_SCENE_METHOD_TYPE_NONE) &&
                    (action_block->block_options.method.type < EZLOPI_SCENE_METHOD_TYPE_MAX))
                {
                    const char *method_type_name = ezlopi_scene_get_scene_method_name(action_block->block_options.method.type);
                    __cjson_add_string(cj_method, ezlopi_name_str, method_type_name ? method_type_name : ezlopi__str);
                }
                else
                {
                    TRACE_E("Method type error");
                }

                cJSON *cj_args = cJSON_AddObjectToObject(__FUNCTION__, cj_method, ezlopi_args_str);
                if (cj_args)
                {
                    l_fields_v2_t *curr_field = action_block->fields;
                    while (curr_field)
                    {
                        cJSON_AddStringToObject(__FUNCTION__, cj_args, curr_field->name, curr_field->name);
                        curr_field = curr_field->next;
                    }
                }
            }
        }
    }
}

static void __cjson_add_action_delay(cJSON *cj_action_block, s_action_delay_v2_t *action_delay)
{
    if (cj_action_block && action_delay)
    {
        if (action_delay->days || action_delay->hours || action_delay->minutes || action_delay->seconds)
        {
            cJSON *cj_action_delay = cJSON_AddObjectToObject(__FUNCTION__, cj_action_block, ezlopi_delay_str);
            if (cj_action_delay)
            {
                cJSON_AddNumberToObject(__FUNCTION__, cj_action_delay, "days", action_delay->days);
                cJSON_AddNumberToObject(__FUNCTION__, cj_action_delay, "hours", action_delay->hours);
                cJSON_AddNumberToObject(__FUNCTION__, cj_action_delay, "minutes", action_delay->minutes);
                cJSON_AddNumberToObject(__FUNCTION__, cj_action_delay, "seconds", action_delay->seconds);
            }
        }
    }
}

static void __cjson_add_fields(cJSON *cj_block, l_fields_v2_t *fields)
{
    if (cj_block)
    {
        cJSON *cj_fields_array = cJSON_AddArrayToObject(__FUNCTION__, cj_block, ezlopi_fields_str);
        if (cj_fields_array)
        {
            l_fields_v2_t *curr_field = fields;
            while (curr_field)
            {
                cJSON *cj_field = cJSON_CreateObject(__FUNCTION__);
                if (cj_field)
                {
                    __cjson_add_string(cj_field, ezlopi_name_str, curr_field->name);
                    const char *value_type_name = ezlopi_scene_get_scene_value_type_name(curr_field->value_type);
                    __cjson_add_string(cj_field, ezlopi_type_str, value_type_name ? value_type_name : ezlopi__str);

                    switch (curr_field->value_type)
                    {
                    case EZLOPI_VALUE_TYPE_NONE:
                    {
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_INT:
                    case EZLOPI_VALUE_TYPE_FLOAT:
                    case EZLOPI_VALUE_TYPE_TEMPERATURE:
                    case EZLOPI_VALUE_TYPE_HUMIDITY:
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
                    case EZLOPI_VALUE_TYPE_KILO_VOLT_AMPERE_HOUR:
                    case EZLOPI_VALUE_TYPE_REACTIVE_POWER_INSTANT:
                    case EZLOPI_VALUE_TYPE_AMOUNT_OF_USEFUL_ENERGY:
                    case EZLOPI_VALUE_TYPE_REACTIVE_POWER_CONSUMPTION:
                    {
                        cJSON_AddNumberToObject(__FUNCTION__, cj_field, ezlopi_value_str, curr_field->field_value.u_value.value_double);
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_BOOL:
                    {
                        if (0 == curr_field->field_value.u_value.value_double)
                        {
                            cJSON_AddFalseToObject(__FUNCTION__, cj_field, ezlopi_value_str);
                        }
                        else
                        {
                            cJSON_AddTrueToObject(__FUNCTION__, cj_field, ezlopi_value_str);
                        }
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_STRING:
                    case EZLOPI_VALUE_TYPE_ITEM:
                    case EZLOPI_VALUE_TYPE_DEVICE:
                    case EZLOPI_VALUE_TYPE_INTERVAL:
                    case EZLOPI_VALUE_TYPE_EXPRESSION:
                    {
                        __cjson_add_string(cj_field, ezlopi_value_str, curr_field->field_value.u_value.value_string);
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_BLOCK:
                    {
                        l_when_block_v2_t *curr_when_block = curr_field->field_value.u_value.when_block; // cj_value?
                        if (curr_when_block)
                        {
                            cJSON *cj_when_block = NULL;
                            cj_when_block = ezlopi_scenes_cjson_create_when_block(curr_when_block);
                            if (cj_when_block)
                            {
                                // TRACE_S(" adding : __ EZLOPI_VALUE_TYPE_BLOCK __");
                                CJSON_TRACE(ezlopi_value_str, cj_when_block);
                                cJSON_AddItemReferenceToObject(__FUNCTION__, cj_field, ezlopi_value_str, cj_when_block);
                            }
                        }
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_BLOCKS:
                    {
                        TRACE_D(" adding : __ EZLOPI_VALUE_TYPE_BLOCKS __");
                        cJSON *value_block_array = cJSON_AddArrayToObject(__FUNCTION__, cj_field, ezlopi_value_str);
                        if (value_block_array)
                        {
                            l_when_block_v2_t *curr_when_block = curr_field->field_value.u_value.when_block; // cj_value?
                            while (curr_when_block)
                            {
                                // TRACE_S("Here!! found when - block");
                                cJSON *cj_when_block = NULL;
                                cj_when_block = ezlopi_scenes_cjson_create_when_block(curr_when_block);
                                if (cj_when_block)
                                {
                                    // TRACE_S("---->> adding when-block");
                                    if (!cJSON_AddItemToArray(value_block_array, cj_when_block))
                                    {
                                        cJSON_Delete(__FUNCTION__, cj_when_block);
                                    }
                                }
                                curr_when_block = curr_when_block->next;
                            }
                            // CJSON_TRACE(ezlopi_value_str, value_block_array);
                        }
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_HOUSE_MODE_ID:
                    {
                        // char id_str[32];
                        // snprintf(id_str, sizeof(id_str), "%u", (uint32_t)curr_field->field_value.u_value.value_double);
                        // TRACE_E("house_mode_id :  %s", id_str);
                        // __cjson_add_string(cj_field, ezlopi_value_str, id_str);
                        __cjson_add_string(cj_field, ezlopi_value_str, curr_field->field_value.u_value.value_string);
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_ARRAY:
                    case EZLOPI_VALUE_TYPE_24_HOURS_TIME:
                    case EZLOPI_VALUE_TYPE_24_HOURS_TIME_ARRAY:
                    case EZLOPI_VALUE_TYPE_INT_ARRAY:
                    case EZLOPI_VALUE_TYPE_HMS_INTERVAL:
                    case EZLOPI_VALUE_TYPE_HOUSE_MODE_ID_ARRAY:
                    case EZLOPI_VALUE_TYPE_RGB:
                    case EZLOPI_VALUE_TYPE_OBJECT:
                    {
                        // "adding reference === duplicating the object";
                        cJSON_AddItemReferenceToObject(__FUNCTION__, cj_field, ezlopi_value_str, curr_field->field_value.u_value.cj_value);
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_CREDENTIAL:
                    case EZLOPI_VALUE_TYPE_DICTIONARY:
                    case EZLOPI_VALUE_TYPE_WEEKLY_INTERVAL:
                    case EZLOPI_VALUE_TYPE_DAILY_INTERVAL:
                    case EZLOPI_VALUE_TYPE_ENUM:
                    case EZLOPI_VALUE_TYPE_TOKEN:
                    {
                        if (VALUE_TYPE_STRING == curr_field->field_value.e_type)
                        {
                            __cjson_add_string(cj_field, ezlopi_value_str, curr_field->field_value.u_value.value_string);
                        }
                        else if (VALUE_TYPE_CJSON == curr_field->field_value.e_type)
                        {
                            cJSON_AddItemReferenceToObject(__FUNCTION__, cj_field, ezlopi_value_str, curr_field->field_value.u_value.cj_value);
                        }
                        else
                        {
                            TRACE_W("Value type not Implemented!, value_type: %d", curr_field->value_type);
                        }
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_SCENEID:
                    {
                        __cjson_add_string(cj_field, ezlopi_value_str, curr_field->field_value.u_value.value_string);
                        break;
                    }
                    case EZLOPI_VALUE_TYPE_CAMERA_STREAM:
                    case EZLOPI_VALUE_TYPE_USER_CODE:
                    case EZLOPI_VALUE_TYPE_BUTTON_STATE:
                    case EZLOPI_VALUE_TYPE_USER_LOCK_OPERATION:
                    case EZLOPI_VALUE_TYPE_USER_CODE_ACTION:
                    case EZLOPI_VALUE_TYPE_SOUND_INFO:
                    case EZLOPI_VALUE_TYPE_CAMERA_HOTZONE:
                    case EZLOPI_VALUE_TYPE_HOTZONE_MATCH:
                    case EZLOPI_VALUE_TYPE_GEOFENCE:
                    case EZLOPI_VALUE_TYPE_MAX:
                    {
                        TRACE_W("Value type not implemented!, curr-type: %d", curr_field->value_type);
                        break;
                    }
                    default:
                    {
#if (1 == ENABLE_TRACE)
                        const char *value_type_name = ezlopi_scene_get_scene_value_type_name(curr_field->value_type);
                        TRACE_E("Value type not matched!, curr-type[%d]: %s ", curr_field->value_type, value_type_name ? value_type_name : ezlopi_null_str);
#endif
                        break;
                    }
                    }

                    if (!cJSON_AddItemToArray(cj_fields_array, cj_field))
                    {
                        cJSON_Delete(__FUNCTION__, cj_field);
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
        cj_action_block = cJSON_CreateObject(__FUNCTION__);
        if (cj_action_block)
        {
            __cjson_add_action_block_options(cj_action_block, action_block);
            __cjson_add_string(cj_action_block, ezlopi_blockType_str, block_type_str);
            __cjson_add_action_delay(cj_action_block, &action_block->delay);
            __cjson_add_fields(cj_action_block, action_block->fields);
        }
    }

    return cj_action_block;
}

void ezlopi_scenes_cjson_add_action_blocks(cJSON *root, l_action_block_v2_t *action_blocks, const char *block_type_str)
{
    if (root && action_blocks)
    {
        cJSON *cj_then_block_array = cJSON_AddArrayToObject(__FUNCTION__, root, block_type_str);

        if (cj_then_block_array)
        {
            while (action_blocks)
            {
                cJSON *cj_then_block = ezlopi_scenes_cjson_create_action_block(action_blocks, (char *)block_type_str);
                if (cj_then_block)
                {
                    if (!cJSON_AddItemToArray(cj_then_block_array, cj_then_block))
                    {
                        cJSON_Delete(__FUNCTION__, cj_then_block);
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
        cj_when_block = cJSON_CreateObject(__FUNCTION__);
        if (cj_when_block)
        {
            ezlopi_scenes_cjson_add_when_group_info(cj_when_block, when_block); // when-group info
            ezlopi_scenes_cjson_add_when_block_options(cj_when_block, when_block);
            __cjson_add_string(cj_when_block, ezlopi_blockType_str, ezlopi_when_str);
            __cjson_add_fields(cj_when_block, when_block->fields);
            ezlopi_scenes_cjson_add_when_block_info(cj_when_block, when_block); // when-block info
        }
    }

    return cj_when_block;
}

void ezlopi_scenes_cjson_add_when_blocks(cJSON *root, l_when_block_v2_t *when_block_node)
{
    if (root)
    {
        cJSON *cj_when_block_array = cJSON_AddArrayToObject(__FUNCTION__, root, ezlopi_when_str);
        if (cj_when_block_array)
        {
            while (when_block_node)
            {
                cJSON *cj_when_block = ezlopi_scenes_cjson_create_when_block(when_block_node);
                if (cj_when_block)
                {
                    if (!cJSON_AddItemToArray(cj_when_block_array, cj_when_block))
                    {
                        cJSON_Delete(__FUNCTION__, cj_when_block);
                    }
                }

                when_block_node = when_block_node->next;
            }
        }
    }
}

cJSON *ezlopi_scenes_create_cjson_scene(l_scenes_list_v2_t *scene)
{
    cJSON *cj_scene = NULL;
    if (scene)
    {
        cj_scene = cJSON_CreateObject(__FUNCTION__);
        if (cj_scene)
        {
            char tmp_str[16] = {0};
            snprintf(tmp_str, sizeof(tmp_str), "%08x", scene->_id);
            cJSON_AddStringToObject(__FUNCTION__, cj_scene, ezlopi__id_str, tmp_str);
            cJSON_AddBoolToObject(__FUNCTION__, cj_scene, ezlopi_enabled_str, scene->enabled);

            snprintf(tmp_str, sizeof(tmp_str), "%08x", scene->group_id);
            cJSON_AddStringToObject(__FUNCTION__, cj_scene, ezlopi_group_id_str, tmp_str);
            cJSON_AddBoolToObject(__FUNCTION__, cj_scene, ezlopi_is_group_str, scene->is_group);

            __cjson_add_string(cj_scene, ezlopi_name_str, scene->name);
            __cjson_add_string(cj_scene, ezlopi_parent_id_str, scene->parent_id);
            if (scene->meta)
            {
                cJSON_AddItemToObject(__FUNCTION__, cj_scene, ezlopi_meta_str, cJSON_Duplicate(__FUNCTION__, scene->meta, 1));
            }
            ezlopi_scenes_cjson_add_user_notifications(cj_scene, scene->user_notifications);
            ezlopi_scenes_cjson_add_house_modes(cj_scene, scene->house_modes);

            ezlopi_scenes_cjson_add_when_blocks(cj_scene, scene->when_block);
            ezlopi_scenes_cjson_add_action_blocks(cj_scene, scene->then_block, ezlopi_then_str);
            ezlopi_scenes_cjson_add_action_blocks(cj_scene, scene->else_block, ezlopi_else_str);
        }
    }

    return cj_scene;
}

cJSON *ezlopi_scenes_create_cjson_scene_list(l_scenes_list_v2_t *scenes_list)
{
    cJSON *cj_scenes_array = cJSON_CreateArray(__FUNCTION__);
    if (cj_scenes_array)
    {
        while (scenes_list)
        {
            cJSON *cj_scene = ezlopi_scenes_create_cjson_scene(scenes_list);
            if (cj_scene)
            {
                if (!cJSON_AddItemToArray(cj_scenes_array, cj_scene))
                {
                    cJSON_Delete(__FUNCTION__, cj_scene);
                }
            }
            scenes_list = scenes_list->next;
        }
    }

    return cj_scenes_array;
}

static void __cjson_add_string(cJSON *root, const char *key, const char *value)
{
    if (root && key && value)
    {
        if (isprint(value[0]))
        {
            cJSON_AddStringToObject(__FUNCTION__, root, key, value);
        }
        else
        {
            cJSON_AddNullToObject(__FUNCTION__, root, key);
        }
    }
}
#endif // CONFIG_EZPI_SERV_ENABLE_MESHBOTS