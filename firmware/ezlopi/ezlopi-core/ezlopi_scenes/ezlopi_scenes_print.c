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

        switch (fields->value_type)
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
