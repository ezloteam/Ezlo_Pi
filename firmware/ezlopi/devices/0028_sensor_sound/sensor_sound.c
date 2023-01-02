#include "sensor_sound.h"
#include "ADS131M08.h"

#include "cJSON.h"
#include "trace.h"
#include "frozen.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"
#include "stdlib.h"

static bool is_motion_detected = false;
static bool prev_motion_status = true;

static int ezlopi_sound_prepare_and_add(void *args);
static s_ezlopi_device_properties_t *ezlopi_sound_prepare(cJSON *cjson_device);
static int ezlopi_sound_init(s_ezlopi_device_properties_t *properties);
static int ezlopi_sound_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);

int sound_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = ezlopi_sound_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = ezlopi_sound_init(properties);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        static int count;
        is_motion_detected = ADS131_value();
        if ((prev_motion_status != is_motion_detected) || (count > 10))
        {
            ret = ezlopi_device_value_updated_from_device(properties);
            prev_motion_status = is_motion_detected;
            count = 0;
        }
        count++;
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ezlopi_sound_get_value_cjson(properties, arg);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static int ezlopi_sound_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *ezlopi_sound_properties = ezlopi_sound_prepare(device_prep_arg->cjson_device);
        if (ezlopi_sound_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, ezlopi_sound_properties, NULL))
            {
                free(ezlopi_sound_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}

static s_ezlopi_device_properties_t *ezlopi_sound_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *ezlopi_sound_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (ezlopi_sound_properties)
    {
        memset(ezlopi_sound_properties, 0, sizeof(s_ezlopi_device_properties_t));
        ezlopi_sound_properties->interface_type = EZLOPI_DEVICE_INTERFACE_UART;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(ezlopi_sound_properties, device_name);
        ezlopi_sound_properties->ezlopi_cloud.category = category_level_sensor;
        ezlopi_sound_properties->ezlopi_cloud.subcategory = subcategory_sound;
        ezlopi_sound_properties->ezlopi_cloud.item_name = ezlopi_item_name_sounding_mode;
        ezlopi_sound_properties->ezlopi_cloud.device_type = dev_type_sensor;
        ezlopi_sound_properties->ezlopi_cloud.value_type = value_type_bool;
        ezlopi_sound_properties->ezlopi_cloud.has_getter = true;
        ezlopi_sound_properties->ezlopi_cloud.has_setter = false;
        ezlopi_sound_properties->ezlopi_cloud.reachable = true;
        ezlopi_sound_properties->ezlopi_cloud.battery_powered = false;
        ezlopi_sound_properties->ezlopi_cloud.show = true;
        ezlopi_sound_properties->ezlopi_cloud.room_name[0] = '\0';
        ezlopi_sound_properties->ezlopi_cloud.device_id = ezlopi_device_generate_device_id();
        ezlopi_sound_properties->ezlopi_cloud.room_id = ezlopi_device_generate_room_id();
        ezlopi_sound_properties->ezlopi_cloud.item_id = ezlopi_device_generate_item_id();
    }

    return ezlopi_sound_properties;
}

static int ezlopi_sound_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    ADS131_init();
    return ret;
}

static int ezlopi_sound_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)args;
    if (cjson_propertise)
    {
        static const char *audible = "audible";
        static const char *silent = "silent";

        static bool value;
        value = (false == value) ? true : false;
        // cJSON_AddStringToObject(cjson_propertise, "value", is_motion_detected ? "audible" : "silent");
        cJSON_AddBoolToObject(cjson_propertise, "value", value);
        ret = 1;
    }

    return ret;
}