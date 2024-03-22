


#include <stdio.h>

#include "cJSON.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_actions.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_cloud_constants.h"

#include "device_0070_battery_sensor.h"

static int __prepare(void* arg);
int __get_cjson_value(l_ezlopi_item_t* item, void* arg);

int device_0070_battery_sensor(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ezlopi_device_information_updated_from_device_v3(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

static int __prepare(void* arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (prep_arg && prep_arg->cjson_device)
    {
        l_ezlopi_device_t* device_battery = ezlopi_device_add_device(prep_arg->cjson_device);
        if (device_battery)
        {
            device_battery->cloud_properties.category = category_level_sensor;
            device_battery->cloud_properties.subcategory = subcategory_not_defined;
            device_battery->cloud_properties.device_type = dev_type_sensor;
            device_battery->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
            device_battery->cloud_properties.parent_device_id = 0;

            l_ezlopi_item_t *item_battery_state = ezlopi_device_add_item_to_device(device_battery, device_0070_battery_sensor);
            if(item_battery_state)
            {
                item_battery_state->cloud_properties.has_getter = true;
                item_battery_state->cloud_properties.has_setter = false;
                item_battery_state->cloud_properties.item_name = ezlopi_item_name_battery;
                item_battery_state->cloud_properties.scale = NULL;
                item_battery_state->cloud_properties.show = true;
                item_battery_state->cloud_properties.value_type = value_type_int;
            }
            else 
            {
                ezlopi_device_free_device(device_battery);
            }
        }
        else
        {
            ret = 1;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

int __get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;

    cJSON* cjson_propreties = (cJSON*)arg;
    if (cjson_propreties)
    {
        cJSON_AddStringToObject(cjson_propreties, ezlopi_status_str, "charging");
        cJSON_AddNumberToObject(cjson_propreties, "remainingTime", 60);
    }

    return ret;
}
