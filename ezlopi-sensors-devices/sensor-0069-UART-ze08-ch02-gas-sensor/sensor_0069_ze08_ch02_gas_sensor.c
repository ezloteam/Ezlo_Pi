
#include <stdlib.h>
#include <string.h>

#include "ezlopi_util_trace.h"

#include "ezlopi_core_actions.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"

#include "ezlopi_cloud_constants.h"

#include "sensor_0069_ze08_ch02_gas_sensor.h"

#include "ze08_ch2o.h"

static int __prepare(void* arg, void* user_arg);
static int __init(l_ezlopi_item_t* item);
static int __get_cjson_value(l_ezlopi_item_t* item, void* arg);
static int __notify(l_ezlopi_item_t* item);

/*
{\
    \"dev_type\": 6,\
    \"dev_name\": \"ZE08CH02 gas sensor\",\
    \"id_room\": 35,\
    \"id_item\": 69,\
    \"gpio_tx\": 48,\
    \"gpio_rx\": 45,\
    \"baud\": 9600\
}\
*/

int sensor_0069_ze08_ch02_gas_sensor(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg)
{
    int ret = 0;
    // TRACE_D("%s", ezlopi_actions_to_string(action));
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
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __get_cjson_value(item, arg);
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

static int __get_cjson_value(l_ezlopi_item_t* item, void* arg)
{
    int ret = 0;

    cJSON* cj_params = (cJSON*)arg;
    ze08_ch2o_sensor_data_t* ze08_ch2o_sensor = (ze08_ch2o_sensor_data_t*)item->user_arg;

    if (ze08_ch2o_sensor && cj_params)
    {
        ezlopi_valueformatter_float_to_cjson(item, cj_params, ze08_ch2o_sensor->ppm);
    }
    else
    {
        ret = 1;
    }
    return ret;
}

static int __notify(l_ezlopi_item_t* item)
{
    int ret = 0;

    ze08_ch2o_sensor_data_t* ze08_ch2o_sensor = (ze08_ch2o_sensor_data_t*)item->user_arg;
    if (ze08_ch2o_sensor && ze08_ch2o_sensor->available)
    {
        ezlopi_device_value_updated_from_device_broadcast(item);
        ze08_ch2o_sensor->available = false;
    }
    else
    {
        ret = 1;
    }

    return ret;
}

static int __init(l_ezlopi_item_t* item)
{
    int ret = 0;
    ze08_ch2o_sensor_data_t* ze08_ch2o_sensor = (ze08_ch2o_sensor_data_t*)item->user_arg;
    if (ze08_ch2o_sensor)
    {
        ret = 1;
        if (item->interface.uart.enable)
        {
            if (false == ze08_ch2o_sensor_init(&item->interface.uart, ze08_ch2o_sensor))
            {
                ret = -1;
            }
        }
    }
    else
    {
        ret = -1;
    }
    return ret;
}

static void __prepare_ze08_ch2o_sensor_device_cloud_properties(l_ezlopi_device_t* device)
{
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_particulate_matter;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.info = NULL;
}

static void __prepare_ze08_ch2o_sensor_ppm_item_cloud_properties(l_ezlopi_item_t* item, cJSON* cj_device, void* user_arg)
{
    item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.item_name = ezlopi_item_name_particulate_matter_1;
    item->cloud_properties.value_type = value_type_substance_amount;
    item->cloud_properties.scale = scales_parts_per_million;

    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_gpio_tx_str, item->interface.uart.tx);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_gpio_rx_str, item->interface.uart.rx);
    CJSON_GET_VALUE_DOUBLE(cj_device, ezlopi_baud_str, item->interface.uart.baudrate);
    item->interface.uart.enable = true;
    item->user_arg = user_arg;
}

static int __prepare(void* arg, void* user_arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t* prep_arg = (s_ezlopi_prep_arg_t*)arg;
    if (prep_arg)
    {
        ze08_ch2o_sensor_data_t* ze08_ch2o_sensor = (ze08_ch2o_sensor_data_t*)malloc(sizeof(ze08_ch2o_sensor_data_t));
        if (ze08_ch2o_sensor)
        {
            memset(ze08_ch2o_sensor, 0, sizeof(ze08_ch2o_sensor_data_t));
            l_ezlopi_device_t* ze08_ch2o_sensor_device = ezlopi_device_add_device(prep_arg->cjson_device, NULL);
            if (ze08_ch2o_sensor_device)
            {
                ret = 1;
                memset(ze08_ch2o_sensor_device, 0, sizeof(l_ezlopi_device_t));
                __prepare_ze08_ch2o_sensor_device_cloud_properties(ze08_ch2o_sensor_device);
                l_ezlopi_item_t* ze08_ch02_ppm_item = ezlopi_device_add_item_to_device(ze08_ch2o_sensor_device, sensor_0069_ze08_ch02_gas_sensor);
                if (ze08_ch02_ppm_item)
                {
                    __prepare_ze08_ch2o_sensor_ppm_item_cloud_properties(ze08_ch02_ppm_item, prep_arg->cjson_device, ze08_ch2o_sensor);
                }
                else
                {
                    ret = -1;
                    ezlopi_device_free_device(ze08_ch2o_sensor_device);
                    free(ze08_ch2o_sensor);
                }
            }
            else
            {
                ret = -1;
                free(ze08_ch2o_sensor);
            }
        }
        else
        {
            ret = -1;
        }
    }

    return ret;
}
