#include "trace.h"
#include "cJSON.h"
#include "math.h"
#include "ezlopi_adc.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_valueformatter.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_device_value_updated.h"

#include "sensor_0041_ADC_FC28_soilMoisture.h"
//--------------------------------------------------------------------------------------------------------

typedef struct s_fc28_data
{
    float hum_val;
} s_fc28_data_t;

static int __0041_prepare(void *arg);
static int __0041_init(l_ezlopi_item_t *item);
static int __0041_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __0041_notify(l_ezlopi_item_t *item);

//--------------------------------------------------------------------------------------------------------
int sensor_0041_ADC_FC28_soilMoisture(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __0041_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __0041_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __0041_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __0041_notify(item);
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
    device->cloud_properties.category = category_humidity;
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
    item->cloud_properties.item_name = ezlopi_item_name_moisture;
    item->cloud_properties.show = true;
    item->cloud_properties.value_type = value_type_moisture;
    item->cloud_properties.scale = scales_percent;
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

static int __0041_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        cJSON *cj_device = device_prep_arg->cjson_device;

        s_fc28_data_t *user_data = (s_fc28_data_t *)malloc(sizeof(s_fc28_data_t));
        if (NULL != user_data)
        {
            memset(user_data, 0, sizeof(s_fc28_data_t));
            l_ezlopi_device_t *fc28_device = ezlopi_device_add_device();
            if (fc28_device)
            {
                __prepare_device_cloud_properties(fc28_device, cj_device);

                l_ezlopi_item_t *fc28_item = ezlopi_device_add_item_to_device(fc28_device, sensor_0041_ADC_FC28_soilMoisture);
                if (fc28_item)
                {
                    __prepare_item_cloud_properties(fc28_item, user_data);
                    __prepare_item_interface_properties(fc28_item, cj_device);
                }
                else
                {
                    ezlopi_device_free_device(fc28_device);
                    free(user_data);
                }
            }
            else
            {
                ezlopi_device_free_device(fc28_device);
                free(user_data);
            }
            ret = 1;
        }
    }
    return ret;
}

static int __0041_init(l_ezlopi_item_t *item)
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

static int __0041_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            s_fc28_data_t *user_data = (s_fc28_data_t *)item->user_arg;
            char *valueFormatted = ezlopi_valueformatter_float(user_data->hum_val);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cj_result, "value", (user_data->hum_val));
            // TRACE_I("soil moisture  : %.2f", user_data->hum_val);
            free(valueFormatted);
            ret = 1;
        }
    }
    return ret;
}

static int __0041_notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        s_fc28_data_t *user_data = (s_fc28_data_t *)item->user_arg;
        s_ezlopi_analog_data_t adc_data = {.value = 0, .voltage = 0};
        ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &adc_data);

        float new_hum = (((float)(4095.0f - (adc_data.value)) / 4095.0f) * 100);

        if (fabs((user_data->hum_val) - new_hum) > 0.05)
        {
            user_data->hum_val = new_hum;
            ezlopi_device_value_updated_from_device_v3(item);
        }
        ret = 1;
    }
    return ret;
}
