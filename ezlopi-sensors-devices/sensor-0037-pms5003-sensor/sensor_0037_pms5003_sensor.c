
#include <stdlib.h>

#include "cJSON.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_timer.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_valueformatter.h"

#include "ezlopi_cloud_constants.h"
#include "pms5003.h"
#include "sensor_0037_pms5003_sensor.h"

static int __prepare(void* arg, void* user_arg);
static int __init(l_ezlopi_item_t* item);
static int __cjson_get_value(l_ezlopi_item_t* item, void* arg);
static int __notify(l_ezlopi_item_t* item);

int sensor_pms5003_v3(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __prepare(arg, user_arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __cjson_get_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __notify(item);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static int __notify(l_ezlopi_item_t* item)
{
    int ret = 0;

    s_pms5003_sensor_object* pms_object = (s_pms5003_sensor_object*)item->user_arg;

    if (pms_object)
    {
        if ((true == pms_is_data_available(&pms_object->pms_data)) && (pms_object->counter == 0))
        {
            pms_set_data_available_to_false(&pms_object->pms_data);
            pms_object->counter = 1;
        }
        if ((pms_object->counter != 0) && (pms_object->counter <= 9))
        {
            s_pms5003_sensor_object* pms_object = (s_pms5003_sensor_object*)item->user_arg;
            pms_print_data(&pms_object->pms_data);
            ezlopi_device_value_updated_from_device_v3(item);
            pms_object->counter++;
        }
        else if (pms_object->counter > 9)
        {
            pms_object->counter = 0;
        }
    }

    return ret;
}

static int __cjson_get_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;

    cJSON *cj_param = (cJSON *)arg;
    s_pms5003_sensor_object *pms_object = (s_pms5003_sensor_object *)item->user_arg;
    if (cj_param && pms_object)
    {
        if (ezlopi_item_name_particulate_matter_1 == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(cj_param, ezlopi_value_str, pms_object->pms_data.pm10_standard);
            char *valueFormatted = ezlopi_valueformatter_uint32(pms_object->pms_data.pm10_standard);
            if (valueFormatted)
            {
                TRACE_I("Dust particle 1 : %s", valueFormatted);
                cJSON_AddStringToObject(cj_param, ezlopi_valueFormatted_str, valueFormatted);
                free(valueFormatted);
            }
        }
        if (ezlopi_item_name_particulate_matter_2_dot_5 == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(cj_param, ezlopi_value_str, pms_object->pms_data.pm25_standard);
            char *valueFormatted = ezlopi_valueformatter_uint32(pms_object->pms_data.pm25_standard);
            if (valueFormatted)
            {
                cJSON_AddStringToObject(cj_param, ezlopi_valueFormatted_str, valueFormatted);
                free(valueFormatted);
            }
        }
        if (ezlopi_item_name_particulate_matter_10 == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(cj_param, ezlopi_value_str, pms_object->pms_data.pm100_standard);
            char *valueFormatted = ezlopi_valueformatter_uint32(pms_object->pms_data.pm100_standard);
            if (valueFormatted)
            {
                cJSON_AddStringToObject(cj_param, ezlopi_valueFormatted_str, valueFormatted);
                free(valueFormatted);
            }
        }
        if (ezlopi_item_name_particles_0_dot_3_um == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(cj_param, ezlopi_value_str, pms_object->pms_data.particles_03um);
            char *valueFormatted = ezlopi_valueformatter_uint32(pms_object->pms_data.particles_03um);
            if (valueFormatted)
            {
                cJSON_AddStringToObject(cj_param, ezlopi_valueFormatted_str, valueFormatted);
                free(valueFormatted);
            }
        }
        if (ezlopi_item_name_particles_0_dot_5_um == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(cj_param, ezlopi_value_str, pms_object->pms_data.particles_05um);
            char *valueFormatted = ezlopi_valueformatter_uint32(pms_object->pms_data.particles_05um);
            if (valueFormatted)
            {
                cJSON_AddStringToObject(cj_param, ezlopi_valueFormatted_str, valueFormatted);
                free(valueFormatted);
            }
        }
        if (ezlopi_item_name_particles_1_um == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(cj_param, ezlopi_value_str, pms_object->pms_data.particles_10um);
            char *valueFormatted = ezlopi_valueformatter_uint32(pms_object->pms_data.particles_10um);
            if (valueFormatted)
            {
                cJSON_AddStringToObject(cj_param, ezlopi_valueFormatted_str, valueFormatted);
                free(valueFormatted);
            }
        }
        if (ezlopi_item_name_particles_2_dot_5_um == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(cj_param, ezlopi_value_str, pms_object->pms_data.particles_25um);
            char *valueFormatted = ezlopi_valueformatter_uint32(pms_object->pms_data.particles_25um);
            if (valueFormatted)
            {
                cJSON_AddStringToObject(cj_param, ezlopi_valueFormatted_str, valueFormatted);
                free(valueFormatted);
            }
        }
        if (ezlopi_item_name_particles_5_um == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(cj_param, ezlopi_value_str, pms_object->pms_data.particles_50um);
            char *valueFormatted = ezlopi_valueformatter_uint32(pms_object->pms_data.particles_50um);
            if (valueFormatted)
            {
                cJSON_AddStringToObject(cj_param, ezlopi_valueFormatted_str, valueFormatted);
                free(valueFormatted);
            }
        }
        if (ezlopi_item_name_particles_10_um == item->cloud_properties.item_name)
        {
            cJSON_AddNumberToObject(cj_param, ezlopi_value_str, pms_object->pms_data.particles_100um);
            char *valueFormatted = ezlopi_valueformatter_uint32(pms_object->pms_data.particles_100um);
            if (valueFormatted)
            {
                cJSON_AddStringToObject(cj_param, ezlopi_valueFormatted_str, valueFormatted);
                free(valueFormatted);
            }
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t* item)
{
    int ret = 0;
    if (item)
    {
        s_pms5003_sensor_object *pms_object = (s_pms5003_sensor_object *)item->user_arg;
        if (pms_object)
        {
            if (item->interface.uart.enable)
            {
                pms_init(pms_object);
            }
            ret = 1;
        }
        // else
        // {
        //     ret = -1;
        //     ezlopi_device_free_device_by_item(item);
        // }
    }
    return ret;
}

static int __prepare(void* arg, void* user_arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (prep_arg)
    {
        uint32_t parent_id = 0;
        pms5003_sensor_preapre_devices_and_items(prep_arg->cjson_device, &parent_id);
    }

    return ret;
}
