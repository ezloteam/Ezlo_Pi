
#include <stdlib.h>

#include "cjext.h"

#include "ezlopi_util_trace.h"

// #include "ezlopi_core_timer.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_cloud_constants.h"
#include "pms5003.h"
#include "sensor_0037_pms5003_sensor.h"

static ezlopi_error_t __prepare(void *arg, void *user_arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __cjson_get_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __notify(l_ezlopi_item_t *item);

ezlopi_error_t sensor_pms5003_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __prepare(arg, user_arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __cjson_get_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __notify(item);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;

    s_pms5003_sensor_object *pms_object = (s_pms5003_sensor_object *)item->user_arg;

    if (pms_object)
    {
        if ((true == pms_is_data_available(&pms_object->pms_data)) && (pms_object->counter == 0))
        {
            pms_set_data_available_to_false(&pms_object->pms_data);
            pms_object->counter = 1;
        }
        if ((pms_object->counter != 0) && (pms_object->counter <= 9))
        {
            s_pms5003_sensor_object *pms_object = (s_pms5003_sensor_object *)item->user_arg;
            pms_print_data(&pms_object->pms_data);
            EZPI_core_device_value_updated_from_device_broadcast(item);
            pms_object->counter++;
            ret = EZPI_SUCCESS;
        }
        else if (pms_object->counter > 9)
        {
            pms_object->counter = 0;
        }
    }

    return ret;
}

static ezlopi_error_t __cjson_get_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;

    cJSON *cj_param = (cJSON *)arg;
    s_pms5003_sensor_object *pms_object = (s_pms5003_sensor_object *)item->user_arg;
    if (cj_param && pms_object)
    {
        if (ezlopi_item_name_particulate_matter_1 == item->cloud_properties.item_name)
        {
            EZPI_core_valueformatter_uint32_to_cjson(cj_param, pms_object->pms_data.pm10_standard, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_particulate_matter_2_dot_5 == item->cloud_properties.item_name)
        {
            EZPI_core_valueformatter_uint32_to_cjson(cj_param, pms_object->pms_data.pm25_standard, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_particulate_matter_10 == item->cloud_properties.item_name)
        {
            EZPI_core_valueformatter_uint32_to_cjson(cj_param, pms_object->pms_data.pm100_standard, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_particles_0_dot_3_um == item->cloud_properties.item_name)
        {
            EZPI_core_valueformatter_uint32_to_cjson(cj_param, pms_object->pms_data.particles_03um, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_particles_0_dot_5_um == item->cloud_properties.item_name)
        {
            EZPI_core_valueformatter_uint32_to_cjson(cj_param, pms_object->pms_data.particles_05um, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_particles_1_um == item->cloud_properties.item_name)
        {
            EZPI_core_valueformatter_uint32_to_cjson(cj_param, pms_object->pms_data.particles_10um, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_particles_2_dot_5_um == item->cloud_properties.item_name)
        {
            EZPI_core_valueformatter_uint32_to_cjson(cj_param, pms_object->pms_data.particles_25um, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_particles_5_um == item->cloud_properties.item_name)
        {
            EZPI_core_valueformatter_uint32_to_cjson(cj_param, pms_object->pms_data.particles_50um, item->cloud_properties.scale);
        }
        else if (ezlopi_item_name_particles_10_um == item->cloud_properties.item_name)
        {
            EZPI_core_valueformatter_uint32_to_cjson(cj_param, pms_object->pms_data.particles_100um, item->cloud_properties.scale);
        }
        ret = EZPI_SUCCESS;
    }

    return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_ERR_INIT_DEVICE_FAILED;
    if (item)
    {
        s_pms5003_sensor_object *pms_object = (s_pms5003_sensor_object *)item->user_arg;
        if (pms_object)
        {
            if (item->interface.uart.enable)
            {
                pms_init(pms_object);
                ret = EZPI_SUCCESS;
            }
        }
    }
    return ret;
}

static ezlopi_error_t __prepare(void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_FAILED;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg)
    {
        uint32_t parent_id = 0;
        ret = pms5003_sensor_preapre_devices_and_items(prep_arg->cjson_device, &parent_id);
    }

    return ret;
}
