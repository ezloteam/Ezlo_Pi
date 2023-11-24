#include "trace.h"
#include "cJSON.h"
#include "math.h"
#include "ezlopi_adc.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_valueformatter.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_device_value_updated.h"

#include "sensor_0028_other_GY61.h"
//--------------------------------------------------------------------------------------------------------

typedef struct s_gy61_data
{
    float x_data;
    float y_data;
    float z_data;
} s_gy61_data_t;

#if (CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32C3)
#define esp32_convert_mV_to_G(temp_vol) (((6.0f * (temp_vol - 1300)) / 750.0f) - 3.0f)
#elif CONFIG_IDF_TARGET_ESP32S3
#define esp32S3_convert_mV_to_G(temp_vol) (((6.0f / 3000.0f) * (temp_vol - 100)) - 3.0f)
#endif

static int __0028_prepare(void *arg);
static int __0028_init(l_ezlopi_item_t *item);
static int __0028_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static int __0028_notify(l_ezlopi_item_t *item);

static float __update_gy61_axis_value(l_ezlopi_item_t *item);
//--------------------------------------------------------------------------------------------------------
int sensor_0028_other_GY61(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        __0028_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        __0028_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __0028_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __0028_notify(item);
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
    device->cloud_properties.category = category_level_sensor;
    device->cloud_properties.subcategory = subcategory_not_defined;
    device->cloud_properties.device_type = dev_type_sensor;
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
}
static void __prepare_item_cloud_properties(l_ezlopi_item_t *item, void *user_data)
{
    item->cloud_properties.show = true;
    item->cloud_properties.has_getter = true;
    item->cloud_properties.has_setter = false;
    item->cloud_properties.value_type = value_type_acceleration;
    item->cloud_properties.scale = scales_meter_per_square_second;
    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = user_data;
}

static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    if (item && cj_device)
    {
        item->interface_type = EZLOPI_DEVICE_INTERFACE_MAX; // other
        item->interface.adc.resln_bit = 3;
        if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
        {
            CJSON_GET_VALUE_INT(cj_device, "gpio1", item->interface.adc.gpio_num);
            TRACE_I("Accel X-axis gpio1: %d ", item->interface.adc.gpio_num);
        }
        else if (ezlopi_item_name_acceleration_y_axis == item->cloud_properties.item_name)
        {
            CJSON_GET_VALUE_INT(cj_device, "gpio2", item->interface.adc.gpio_num);
            TRACE_I("Accel Y-axis gpio2: %d ", item->interface.adc.gpio_num);
        }
        else if (ezlopi_item_name_acceleration_z_axis == item->cloud_properties.item_name)
        {
            CJSON_GET_VALUE_INT(cj_device, "gpio3", item->interface.adc.gpio_num);
            TRACE_I("Accel Z-axis gpio3: %d ", item->interface.adc.gpio_num);
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------

static int __0028_prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        cJSON *cj_device = device_prep_arg->cjson_device;

        s_gy61_data_t *gy61_value = (s_gy61_data_t *)malloc(sizeof(s_gy61_data_t));
        if (NULL != gy61_value)
        {
            memset(gy61_value, 0, sizeof(s_gy61_data_t));
            l_ezlopi_device_t *gy61_device = ezlopi_device_add_device();
            if (gy61_device)
            {

                __prepare_device_cloud_properties(gy61_device, cj_device);

                l_ezlopi_item_t *gy61_item_x = ezlopi_device_add_item_to_device(gy61_device, sensor_0028_other_GY61);
                if (gy61_item_x)
                {
                    gy61_item_x->cloud_properties.item_name = ezlopi_item_name_acceleration_x_axis;
                    __prepare_item_cloud_properties(gy61_item_x, gy61_value);
                    __prepare_item_interface_properties(gy61_item_x, cj_device);
                }

                l_ezlopi_item_t *gy61_item_y = ezlopi_device_add_item_to_device(gy61_device, sensor_0028_other_GY61);
                if (gy61_item_y)
                {
                    gy61_item_y->cloud_properties.item_name = ezlopi_item_name_acceleration_y_axis;
                    __prepare_item_cloud_properties(gy61_item_y, gy61_value);
                    __prepare_item_interface_properties(gy61_item_y, cj_device);
                }

                l_ezlopi_item_t *gy61_item_z = ezlopi_device_add_item_to_device(gy61_device, sensor_0028_other_GY61);
                if (gy61_item_z)
                {
                    gy61_item_z->cloud_properties.item_name = ezlopi_item_name_acceleration_z_axis;
                    __prepare_item_cloud_properties(gy61_item_z, gy61_value);
                    __prepare_item_interface_properties(gy61_item_z, cj_device);
                }

                if ((NULL == gy61_item_x) && (NULL == gy61_item_y) && (NULL == gy61_item_z))
                {
                    ezlopi_device_free_device(gy61_device);
                    free(gy61_value);
                }
            }
            else
            {
                ezlopi_device_free_device(gy61_device);
                free(gy61_value);
            }
            ret = 1;
        }
    }
    return ret;
}

static int __0028_init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (NULL != item)
    {
        s_gy61_data_t *user_data = (s_gy61_data_t *)item->user_arg;
        ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit);
        ret = 1;
    }
    return ret;
}

static int __0028_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            s_gy61_data_t *user_data = ((s_gy61_data_t *)item->user_arg);
            if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
            {
                char *valueFormatted = ezlopi_valueformatter_float(user_data->x_data);
                cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
                cJSON_AddNumberToObject(cj_result, "value", (user_data->x_data));
                // TRACE_I("x-axis-G : %.2f", user_data->x_data);
                free(valueFormatted);
            }
            if (ezlopi_item_name_acceleration_y_axis == item->cloud_properties.item_name)
            {
                char *valueFormatted = ezlopi_valueformatter_float(user_data->y_data);
                cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
                cJSON_AddNumberToObject(cj_result, "value", (user_data->y_data));
                // TRACE_I("y-axis-G : %.2f", user_data->y_data);
                free(valueFormatted);
            }
            if (ezlopi_item_name_acceleration_z_axis == item->cloud_properties.item_name)
            {
                char *valueFormatted = ezlopi_valueformatter_float(user_data->z_data);
                cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
                cJSON_AddNumberToObject(cj_result, "value", (user_data->z_data));
                // TRACE_I("z-axis-G : %.2f", user_data->z_data);
                free(valueFormatted);
            }
            ret = 1;
        }
    }
    return ret;
}

static int __0028_notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        s_gy61_data_t *user_data = (s_gy61_data_t *)item->user_arg;
        float new_value = 0;
        if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
        {
            new_value = __update_gy61_axis_value(item);

            if (fabs((user_data->x_data) - new_value) > 0.5)
            {
                user_data->x_data = new_value;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        if (ezlopi_item_name_acceleration_y_axis == item->cloud_properties.item_name)
        {
            new_value = __update_gy61_axis_value(item);
            if (fabs((user_data->y_data) - new_value) > 0.5)
            {
                user_data->y_data = new_value;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        if (ezlopi_item_name_acceleration_z_axis == item->cloud_properties.item_name)
        {
            new_value = __update_gy61_axis_value(item);
            if (fabs((user_data->z_data) - new_value) > 0.5)
            {
                user_data->z_data = new_value;
                ezlopi_device_value_updated_from_device_v3(item);
            }
        }
        ret = 1;
    }
    return ret;
}

static float __update_gy61_axis_value(l_ezlopi_item_t *item)
{
    float G_data = 0;
    if (item)
    {
        s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0, .voltage = 0};
        ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
        int temp_vol = ezlopi_analog_data.voltage;
#if (CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32C3)
        if (temp_vol <= 1300)
        {
            G_data = 0;
        }
        else if (temp_vol >= 2050)
        {
            G_data = 3;
        }
        else
        {
            G_data = esp32_convert_mV_to_G(temp_vol);
        }
#elif CONFIG_IDF_TARGET_ESP32S3
        if (temp_vol <= 100)
        {
            G_data = 0;
        }
        else if (temp_vol >= 3100)
        {
            G_data = 3;
        }
        else
        {
            G_data = esp32S3_convert_mV_to_G(temp_vol);
        }
#endif
    }
    return G_data;
}
