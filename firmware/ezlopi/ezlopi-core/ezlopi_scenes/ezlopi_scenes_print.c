#include "trace.h"
#include "ezlopi_scenes.h"

void ezlopi_print_block_options(s_block_options_t *block_options, l_fields_t *fields)
{
    TRACE_D("\t\t|-- blockOptions:");
    TRACE_D("\t\t\t|-- method");
    TRACE_D("\t\t\t\t|-- name: %s", block_options->method.name);
    TRACE_D("\t\t\t\t|-- args:");

    while (fields)
    {
        TRACE_D("\t\t\t\t\t|-- %s: %s", fields->name, fields->name);
        fields = fields->next;
    }
}

void ezlopi_print_fields(l_fields_t *fields)
{
    TRACE_D("\t\t|-- fields: ");
    int field_count = 0;
    while (fields)
    {
        TRACE_D("\t\t\t|---------- field_count: %d ----------", ++field_count);
        TRACE_D("\t\t\t|-- name: %s", fields->name);

        const char *value_type_name = ezlopi_scene_get_scene_value_type_name(fields->value_type);
        TRACE_D("\t\t\t|-- type: %s", value_type_name ? value_type_name : "");

        switch (fields->value_type)
        {
        case EZLOPI_VALUE_TYPE_INT:
        {
            TRACE_D("\t\t\t|-- value: %f", fields->value.value_double);
            break;
        }
        case EZLOPI_VALUE_TYPE_BOOL:
        {
            TRACE_D("\t\t\t|-- value: [%d]%s", fields->value.value_bool, fields->value.value_bool ? "true" : "false");
            break;
        }
        case EZLOPI_VALUE_TYPE_FLOAT:
        {
            TRACE_D("\t\t\t|-- value: %f", fields->value.value_double);
            break;
        }
        case EZLOPI_VALUE_TYPE_STRING:
        {
            TRACE_D("\t\t\t|-- value: %s", fields->value.value_string);
            break;
        }
        case EZLOPI_VALUE_TYPE_ITEM:
        {
            TRACE_D("\t\t\t|-- value: %s", fields->value.value_string);
            break;
        }
        case EZLOPI_VALUE_TYPE_INTERVAL:
        {
            TRACE_D("\t\t\t|-- value: %s", fields->value.value_string);
            break;
        }
        case EZLOPI_VALUE_TYPE_BLOCKS:
        {
            ezlopi_print_when_blocks(fields->value.when_block);
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
        case EZLOPI_VALUE_TYPE_NONE:
        case EZLOPI_VALUE_TYPE_MAX:
        {
            TRACE_W("Value type not Implemented!, value_type: %d", fields->value_type);
            break;
        }
        default:
        {
            TRACE_E("Value type not matched!");
            break;
        }
        }

#if 0
        if (SCENE_VALUE_TYPE_INT == fields->value_type)
        {
            TRACE_D("\t\t\t|-- type: int");
            TRACE_D("\t\t\t|-- value: %f", fields->value.value_double);
        }
        else if (SCENE_VALUE_TYPE_ITEM == fields->value_type)
        {
            TRACE_D("\t\t\t|-- type: item");
            TRACE_D("\t\t\t|-- value: %s", fields->value.value_string);
        }
        else if (SCENE_VALUE_TYPE_BOOL == fields->value_type)
        {
            TRACE_D("\t\t\t|-- type: bool");
            TRACE_D("\t\t\t|-- value: %s", (0 == fields->value.value_double) ? "false" : "true");
        }
        else
        {
            TRACE_E("\t\t\t|- value type: %d", fields->value_type);
        }
#endif

        fields = fields->next;
        if (fields)
        {
            TRACE_D("");
        }
    }
    TRACE_D("\t\t\t|------------------------------------");
}

void ezlopi_print_house_modes(l_house_modes_t *house_modes)
{
    TRACE_D("\t|-- house_modes: ");
    while (house_modes)
    {
        TRACE_D("\t\t|-- %s", house_modes->house_mode);
        house_modes = house_modes->next;
    }
}

void ezlopi_print_user_notifications(l_user_notification_t *user_notification)
{
    TRACE_D("\t|-- user_notifications: ");
    while (user_notification)
    {
        TRACE_D("\t\t|-- %s", user_notification->user_id);
        user_notification = user_notification->next;
    }
}

void ezlopi_print_when_blocks(l_when_block_t *when_blocks)
{
    TRACE_D("\t|-- when: ");
    while (when_blocks)
    {
        ezlopi_print_block_options(&when_blocks->block_options, when_blocks->fields);
        TRACE_D("\t\t|-- blockType: when");
        ezlopi_print_fields(when_blocks->fields);
        when_blocks = when_blocks->next;
    }
}

void ezlopi_print_then_blocks(l_then_block_t *then_blocks)
{
    TRACE_D("\t|-- then: ");
    while (then_blocks)
    {
        ezlopi_print_block_options(&then_blocks->block_options, then_blocks->fields);
        TRACE_D("\t\t|-- blockType: then");

        TRACE_D("\t\t|-- Delay:: days: %d, hours: %d, minutes: %d, seconds: %d", then_blocks->delay.days, then_blocks->delay.hours, then_blocks->delay.minutes, then_blocks->delay.seconds);

        ezlopi_print_fields(then_blocks->fields);

        then_blocks = then_blocks->next;
        if (then_blocks)
        {
            TRACE_D("\t\t|--");
        }
    }
}

void ezlopi_scenes_print(l_scenes_list_t *scene_link_list)
{
    int scene_count = 0;
    while (scene_link_list)
    {
        TRACE_D("\t----------------------- scene_count: %d ------------------------", ++scene_count);
        TRACE_D("\t|-- id: 0x%08x", scene_link_list->_id);
        TRACE_D("\t|-- enabled: %d", scene_link_list->enabled);
        TRACE_D("\t|-- is_group: %d", scene_link_list->is_group);
        TRACE_D("\t|-- group_id: %s", scene_link_list->group_id);
        TRACE_D("\t|-- name: %s", scene_link_list->name);
        TRACE_D("\t|-- parent_id: %s", scene_link_list->parent_id);
        ezlopi_print_user_notifications(scene_link_list->user_notifications);
        ezlopi_print_house_modes(scene_link_list->house_modes);
        ezlopi_print_then_blocks(scene_link_list->then);
        ezlopi_print_when_blocks(scene_link_list->when);
        TRACE_D("\t---------------------------------------------------------------");

        scene_link_list = scene_link_list->next;
    }
}
