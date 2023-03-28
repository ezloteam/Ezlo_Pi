

#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "ezlopi_adc.h"

#include "esp_err.h"
#include "items.h"
#include "trace.h"
#include "cJSON.h"

#include "030_sens_ds18b20_sensor.h"


static int ds18b20_sensor_prepare_and_add(void *args);
static s_ezlopi_device_properties_t *ds18b20_sensor_prepare(cJSON *cjson_device);
static int ds18b20_sensor_init(s_ezlopi_device_properties_t *properties);
static int get_ds18b20_sensor_value_to_cloud(s_ezlopi_device_properties_t *properties, void *args);
static int ds18b20_sensor_get_value(s_ezlopi_device_properties_t *properties);


static esp_err_t ds18b20_write_data(uint8_t* data, uint32_t gpio_pin);
static esp_err_t ds18b20_read_data(uint8_t* data, uint32_t gpio_pin);
static bool ds18b20_reset_line(uint32_t gpio_pin);
static esp_err_t ds18b20_write_to_scratchpad(uint8_t th_val, uint8_t tl_val, uint8_t resolution, uint8_t gpio_pin);
static bool ds18b20_recognize_device(uint32_t gpio_pin);
static esp_err_t ds18b20_get_temperature_data(uint8_t* temperature_data, uint32_t gpio_pin);


int ds18b20_sensor(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_arg)
{
    int ret = 0;

    switch (action)
    {
        case EZLOPI_ACTION_PREPARE:
        {
            ret = ds18b20_sensor_prepare_and_add(arg);
            break;
        }
        case EZLOPI_ACTION_INITIALIZE:
        {
            break;
        }
        case EZLOPI_ACTION_GET_EZLOPI_VALUE:
        {
            break;
        }
        case EZLOPI_ACTION_NOTIFY_200_MS:
        {
            break;
        }
        default:
        {
            break;
        }
    }
    return ret;
}


static int ds18b20_sensor_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        s_ezlopi_device_properties_t *ds18b20_sensor_properties = ds18b20_sensor_prepare(device_prep_arg->cjson_device);
        if (ds18b20_sensor_properties)
        {
            if (0 == ezlopi_devices_list_add(device_prep_arg->device, ds18b20_sensor_properties, NULL))
            {
                free(ds18b20_sensor_properties);
            }
            else
            {
                ret = 1;
            }
        }
    }

    return ret;
}


static s_ezlopi_device_properties_t *ds18b20_sensor_prepare(cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *ds18b20_sensor_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (ds18b20_sensor_properties)
    {
        memset(ds18b20_sensor_properties, 0, sizeof(s_ezlopi_device_properties_t));
        ds18b20_sensor_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;

        char *device_name = NULL;
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(ds18b20_sensor_properties, device_name);
        ds18b20_sensor_properties->ezlopi_cloud.category = category_temperature;
        ds18b20_sensor_properties->ezlopi_cloud.subcategory = subcategory_not_defined;
        ds18b20_sensor_properties->ezlopi_cloud.item_name = ezlopi_item_name_temp;
        ds18b20_sensor_properties->ezlopi_cloud.device_type = dev_type_sensor;
        ds18b20_sensor_properties->ezlopi_cloud.value_type = value_type_temperature;
        ds18b20_sensor_properties->ezlopi_cloud.has_getter = true;
        ds18b20_sensor_properties->ezlopi_cloud.has_setter = false;
        ds18b20_sensor_properties->ezlopi_cloud.reachable = true;
        ds18b20_sensor_properties->ezlopi_cloud.battery_powered = false;
        ds18b20_sensor_properties->ezlopi_cloud.show = true;
        ds18b20_sensor_properties->ezlopi_cloud.room_name[0] = '\0';
        ds18b20_sensor_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        ds18b20_sensor_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        ds18b20_sensor_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        // CJSON_GET_VALUE_INT(cjson_device, "gpio", ds18b20_sensor_properties->interface.gpio.gpio_in.gpio_num);
        ds18b20_sensor_properties->interface.gpio.gpio_in.gpio_num = 2;
        
    }

    return ds18b20_sensor_properties;
}

