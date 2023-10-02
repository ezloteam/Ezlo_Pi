
#include <string.h>
#include "sdkconfig.h"

#include "cJSON.h"
#include "trace.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_devices.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_i2c_master.h"
#include "ezlopi_spi_master.h"

#include "sensor_0008_I2C_LTR303ALS.h"
#include "ALS_LTR303.h"

static int count = 5;

static int ltr303_prepare_sensor(void* arg);
static s_ezlopi_device_properties_t* ltr303_ambient_sensor_prepare_properties(cJSON* cjson_device);
static int ltr303_ambient_sensor_init(s_ezlopi_device_properties_t* properties);
static int ltr303_ambient_sensor_update_values(s_ezlopi_device_properties_t* properties);
static int ltr303_ambient_sensor_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg);


int sensor_0008_I2C_LTR303ALS(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;
    switch(action)
    {
        case EZLOPI_ACTION_PREPARE:
        {
            ltr303_prepare_sensor(arg);
            break;
        }
        case EZLOPI_ACTION_INITIALIZE:
        {
            ltr303_ambient_sensor_init(properties);
            break;
        }
        case EZLOPI_ACTION_GET_EZLOPI_VALUE:
        {
            ltr303_ambient_sensor_get_value_cjson(properties, arg);
            break;
        }
        case EZLOPI_ACTION_NOTIFY_1000_MS:
        {
            if(5 == count)
            {
                ltr303_ambient_sensor_update_values(properties);
                ezlopi_device_value_updated_from_device(properties);
                count = 0;
            }
            count++;
            break;
        }
        default:
        {
            break;
        }
    }

    return ret;
}



static int ltr303_prepare_sensor(void* arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *ltr303_ambient_sensor_properties = ltr303_ambient_sensor_prepare_properties(device_prep_arg->cjson_device);
        if (ltr303_ambient_sensor_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, ltr303_ambient_sensor_properties, NULL))
            {
                free(ltr303_ambient_sensor_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}


static s_ezlopi_device_properties_t* ltr303_ambient_sensor_prepare_properties(cJSON* cjson_device)
{
    s_ezlopi_device_properties_t *ltr303_ambient_sensor_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    ltr303_data_t* ltr303_lux_val = (ltr303_data_t*)malloc(sizeof(ltr303_data_t));

    if (ltr303_ambient_sensor_properties && ltr303_lux_val)
    {
        memset(ltr303_ambient_sensor_properties, 0, sizeof(s_ezlopi_device_properties_t));
        memset(ltr303_lux_val, 0, sizeof(ltr303_data_t));

        ltr303_ambient_sensor_properties->interface_type = EZLOPI_DEVICE_INTERFACE_I2C_MASTER;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(ltr303_ambient_sensor_properties, device_name);
        ltr303_ambient_sensor_properties->ezlopi_cloud.category = category_light_sensor;
        ltr303_ambient_sensor_properties->ezlopi_cloud.subcategory = subcategory_not_defined;
        ltr303_ambient_sensor_properties->ezlopi_cloud.item_name = ezlopi_item_name_lux;
        ltr303_ambient_sensor_properties->ezlopi_cloud.device_type = dev_type_sensor;
        ltr303_ambient_sensor_properties->ezlopi_cloud.value_type = value_type_illuminance;
        ltr303_ambient_sensor_properties->ezlopi_cloud.has_getter = true;
        ltr303_ambient_sensor_properties->ezlopi_cloud.has_setter = false;
        ltr303_ambient_sensor_properties->ezlopi_cloud.reachable = true;
        ltr303_ambient_sensor_properties->ezlopi_cloud.battery_powered = false;
        ltr303_ambient_sensor_properties->ezlopi_cloud.show = true;
        ltr303_ambient_sensor_properties->ezlopi_cloud.room_name[0] = '\0';
        ltr303_ambient_sensor_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        ltr303_ambient_sensor_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        ltr303_ambient_sensor_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "gpio_scl", ltr303_ambient_sensor_properties->interface.i2c_master.scl);
        CJSON_GET_VALUE_INT(cjson_device, "gpio_sda", ltr303_ambient_sensor_properties->interface.i2c_master.sda);
        ltr303_ambient_sensor_properties->interface.i2c_master.clock_speed = I2C_MASTER_FREQ_HZ;
        ltr303_ambient_sensor_properties->interface.i2c_master.address = LTR303_ADDR;
        ltr303_ambient_sensor_properties->interface.i2c_master.enable = true;
        ltr303_ambient_sensor_properties->interface.i2c_master.channel = LTR303_I2C_CHANNEL;
        ltr303_ambient_sensor_properties->user_arg = ltr303_lux_val;
    }

    return ltr303_ambient_sensor_properties;
}


static int ltr303_ambient_sensor_init(s_ezlopi_device_properties_t* properties)
{
    int ret = 0;

    ltr303_setup(properties->interface.i2c_master.sda, properties->interface.i2c_master.scl, true);

    return ret;
}


static int ltr303_ambient_sensor_update_values(s_ezlopi_device_properties_t* properties)
{
    int ret = 0;
    ltr303_data_t* ltr303_lux_val = (ltr303_data_t*)properties->user_arg;

    if(ltr303_lux_val)
    {
        if(ltr303_get_val(ltr303_lux_val) == ESP_OK)
        { 
            TRACE_D(" lux: %f\n",ltr303_lux_val->lux);
        }
    }
    return ret;
}



static int ltr303_ambient_sensor_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;

    cJSON *cjson_properties = (cJSON *)arg;
    ltr303_data_t* ltr303_lux_val = (ltr303_data_t*)properties->user_arg;

    if(cjson_properties && ltr303_lux_val)
    {
        char formatted_value[10];
        snprintf(formatted_value, 10, "%.2f", ltr303_lux_val->lux);
        cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
        cJSON_AddNumberToObject(cjson_properties, "value", ltr303_lux_val->lux);
        cJSON_AddStringToObject(cjson_properties, "scale", "lux");
    }

    return ret;
}


