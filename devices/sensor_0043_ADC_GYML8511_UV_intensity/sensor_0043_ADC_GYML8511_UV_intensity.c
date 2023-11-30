#include "trace.h"
#include "cJSON.h"
#include "math.h"
#include "ezlopi_adc.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_valueformatter.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_device_value_updated.h"

#include "sensor_0043_ADC_GYML8511_UV_intensity.h"
//--------------------------------------------------------------------------------------------------------

typedef struct s_gyml8511_data
{
    float uv_data;
} s_gyml8511_data_t;

static int __0043_prepare(void *arg);
static int __0043_init(l_ezlopi_item_t *item);
static int __0043_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __0043_notify(l_ezlopi_item_t *item);
static float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);

//--------------------------------------------------------------------------------------------------------
int sensor_0043_ADC_GYML8511_UV_intensity(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __0043_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __0043_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __0043_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __0043_notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}
//-------------------------------------------------------------------------------------------------------------------------
static void __prepare_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    char *dev_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", dev_name);
    ASSIGN_DEVICE_NAME_V2(device, dev_name);
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type = dev_type_sensor;
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, void *user_data)
{
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.item_name = ezlopi_item_name_solar_radiation;
    item->cloud_properties.show = true;
    item->cloud_properties.value_type = value_type_ultraviolet;
    item->cloud_properties.scale = scales_watt_per_square_meter;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = user_data;
}
static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    if (item && cj_device)
    {
        item->interface_type = EZLOPI_DEVICE_INTERFACE_MAX; // other
        CJSON_GET_VALUE_INT(cj_device, "gpio", item->interface.adc.gpio_num);
        item->interface.adc.resln_bit = 3;
    }
}
//-------------------------------------------------------------------------------------------------------------------------

static int __0043_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        cJSON *cj_device = device_prep_arg->cjson_device;

        s_gyml8511_data_t *gyml8511_value = (s_gyml8511_data_t *)malloc(sizeof(s_gyml8511_data_t));
        if (NULL != gyml8511_value)
        {
            memset(gyml8511_value, 0, sizeof(s_gyml8511_data_t));
            l_ezlopi_device_t *gyml8511_device = ezlopi_device_add_device();
            if (gyml8511_device)
            {
                __prepare_device_cloud_properties(gyml8511_device, cj_device);

                l_ezlopi_item_t *gyml8511_item = ezlopi_device_add_item_to_device(gyml8511_device, sensor_0043_ADC_GYML8511_UV_intensity);
                if (gyml8511_item)
                {
                    __prepare_item_cloud_properties(gyml8511_item, gyml8511_value);
                    __prepare_item_interface_properties(gyml8511_item, cj_device);
                }
                else
                {
                    ezlopi_device_free_device(gyml8511_device);
                    free(gyml8511_value);
                }
            }
            else
            {
                ezlopi_device_free_device(gyml8511_device);
                free(gyml8511_value);
            }
            ret = 1;
        }
    }
    return ret;
}

static int __0043_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (NULL != item)
    {
        if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
        {
            ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit);
            ret = 1;
        }
    }
    return ret;
}

static int __0043_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            s_gyml8511_data_t *user_data = (s_gyml8511_data_t *)item->user_arg;

            char *valueFormatted = ezlopi_valueformatter_float((user_data->uv_data) / 10); // [mW/cm^2] -> [W/m^2]
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cj_result, "value", (user_data->uv_data) / 10); // [mW/cm^2] -> [W/m^2]
            // TRACE_I("UV_intensity : %.2f", user_data->uv_data);
            free(valueFormatted);

            ret = 1;
        }
    }
    return ret;
}

static int __0043_notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        s_gyml8511_data_t *user_data = (s_gyml8511_data_t *)item->user_arg;
        s_ezlopi_analog_data_t adc_data = {.value = 0, .voltage = 0};
        ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &adc_data);
        float new_uvIntensity = mapfloat(((float)(adc_data.voltage) / 1000), 0.97, 2.7, 0.0, 15.0);
        TRACE_I("%dmv -> intensity: %.2f", adc_data.voltage, new_uvIntensity);
        if (fabs((user_data->uv_data) - new_uvIntensity) > 0.01)
        {
            user_data->uv_data = new_uvIntensity;
            ezlopi_device_value_updated_from_device_v3(item);
        }
        ret = 1;
    }
    return ret;
}

static float mapfloat(float x, float in_min, float in_max, float out_min, float out_max)
{
    float res = ((x - in_min) * ((out_max - out_min) / (in_max - in_min)) + out_min);
    if (res < 0)
    {
        res = 0;
    }
    return res;
}