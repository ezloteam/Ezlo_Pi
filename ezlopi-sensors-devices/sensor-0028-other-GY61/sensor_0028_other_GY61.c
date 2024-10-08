#include <math.h>
#include "ezlopi_util_trace.h"

// #include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_hal_adc.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "sensor_0028_other_GY61.h"
#include "EZLOPI_USER_CONFIG.h"
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

static ezlopi_error_t __0028_prepare(void *arg);
static ezlopi_error_t __0028_init(l_ezlopi_item_t *item);
static ezlopi_error_t __0028_get_cjson_value(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __0028_notify(l_ezlopi_item_t *item);

static float __update_gy61_axis_value(l_ezlopi_item_t *item);
//--------------------------------------------------------------------------------------------------------
ezlopi_error_t sensor_0028_other_GY61(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __0028_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __0028_init(item);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = __0028_get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ret = __0028_notify(item);
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
    item->cloud_properties.show = true;
    item->cloud_properties.value_type = value_type_acceleration;
    item->cloud_properties.scale = scales_meter_per_square_second;
    //----- CUSTOM DATA STRUCTURE -----------------------------------------
    item->user_arg = user_data;
    if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
    {
        item->is_user_arg_unique = true;
    }
}

static void __prepare_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    if (item && cj_device)
    {
        item->interface_type = EZLOPI_DEVICE_INTERFACE_MAX; // other
        item->interface.adc.resln_bit = 3;
        if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
        {
            CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio1_str, item->interface.adc.gpio_num);
            TRACE_S("Accel X-axis gpio1: %d ", item->interface.adc.gpio_num);
        }
        else if (ezlopi_item_name_acceleration_y_axis == item->cloud_properties.item_name)
        {
            CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio2_str, item->interface.adc.gpio_num);
            TRACE_S("Accel Y-axis gpio2: %d ", item->interface.adc.gpio_num);
        }
        else if (ezlopi_item_name_acceleration_z_axis == item->cloud_properties.item_name)
        {
            CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio3_str, item->interface.adc.gpio_num);
            TRACE_S("Accel Z-axis gpio3: %d ", item->interface.adc.gpio_num);
        }
    }
}
//-------------------------------------------------------------------------------------------------------------------------

static ezlopi_error_t __0028_prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (device_prep_arg && (NULL != device_prep_arg->cjson_device))
    {
        cJSON *cj_device = device_prep_arg->cjson_device;

        s_gy61_data_t *gy61_value = (s_gy61_data_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_gy61_data_t));
        if (NULL != gy61_value)
        {
            memset(gy61_value, 0, sizeof(s_gy61_data_t));
            l_ezlopi_device_t *gy61_device_x_parent = ezlopi_device_add_device(cj_device, "acc_x");
            if (gy61_device_x_parent)
            {
                TRACE_I("Parent_gy61_device_x-[0x%x] ", gy61_device_x_parent->cloud_properties.device_id);
                __prepare_device_cloud_properties(gy61_device_x_parent, cj_device);
                l_ezlopi_item_t *gy61_item_x = ezlopi_device_add_item_to_device(gy61_device_x_parent, sensor_0028_other_GY61);
                if (gy61_item_x)
                {
                    gy61_item_x->cloud_properties.item_name = ezlopi_item_name_acceleration_x_axis;
                    __prepare_item_cloud_properties(gy61_item_x, gy61_value);
                    __prepare_item_interface_properties(gy61_item_x, cj_device);
                    ret = EZPI_SUCCESS;
                }

                l_ezlopi_device_t *gy61_device_y_child = ezlopi_device_add_device(device_prep_arg->cjson_device, "acc_y");
                if (gy61_device_y_child)
                {
                    TRACE_I("Child_gy61_device_y-[0x%x] ", gy61_device_y_child->cloud_properties.device_id);
                    __prepare_device_cloud_properties(gy61_device_y_child, cj_device);

                    l_ezlopi_item_t *gy61_item_y = ezlopi_device_add_item_to_device(gy61_device_y_child, sensor_0028_other_GY61);
                    if (gy61_item_y)
                    {
                        gy61_item_y->cloud_properties.item_name = ezlopi_item_name_acceleration_y_axis;
                        __prepare_item_cloud_properties(gy61_item_y, gy61_value);
                        __prepare_item_interface_properties(gy61_item_y, cj_device);
                    }
                    else
                    {
                        ezlopi_device_free_device(gy61_device_y_child);
                        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                    }
                }

                l_ezlopi_device_t *gy61_device_z_child = ezlopi_device_add_device(device_prep_arg->cjson_device, "acc_z");
                if (gy61_device_z_child)
                {
                    TRACE_I("Child_gy61_device_z-[0x%x] ", gy61_device_z_child->cloud_properties.device_id);
                    __prepare_device_cloud_properties(gy61_device_z_child, cj_device);

                    l_ezlopi_item_t *gy61_item_z = ezlopi_device_add_item_to_device(gy61_device_z_child, sensor_0028_other_GY61);
                    if (gy61_item_z)
                    {
                        gy61_item_z->cloud_properties.item_name = ezlopi_item_name_acceleration_z_axis;
                        __prepare_item_cloud_properties(gy61_item_z, gy61_value);
                        __prepare_item_interface_properties(gy61_item_z, cj_device);
                    }
                    else
                    {
                        ezlopi_device_free_device(gy61_device_z_child);
                    }
                }

                if ((NULL == gy61_item_x) &&
                    (NULL == gy61_device_y_child) &&
                    (NULL == gy61_device_z_child))
                {
                    ezlopi_device_free_device(gy61_device_x_parent);
                    ezlopi_free(__FUNCTION__, gy61_value);
                }
            }
            else
            {
                ezlopi_free(__FUNCTION__, gy61_value);
            }
        }
    }
    return ret;
}

static ezlopi_error_t __0028_init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        s_gy61_data_t *user_data = (s_gy61_data_t *)item->user_arg;
        if (user_data)
        {
            if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
            {
                if (EZPI_SUCCESS == ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit))
                {
                    ret = EZPI_SUCCESS;
                }
            }
        }
    }
    return ret;
}

static ezlopi_error_t __0028_get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item && arg)
    {
        cJSON *cj_result = (cJSON *)arg;
        if (cj_result)
        {
            s_gy61_data_t *user_data = ((s_gy61_data_t *)item->user_arg);

            if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
            {
                ezlopi_valueformatter_float_to_cjson(cj_result, user_data->x_data, item->cloud_properties.scale);
            }
            else if (ezlopi_item_name_acceleration_y_axis == item->cloud_properties.item_name)
            {
                ezlopi_valueformatter_float_to_cjson(cj_result, user_data->y_data, item->cloud_properties.scale);
            }
            else if (ezlopi_item_name_acceleration_z_axis == item->cloud_properties.item_name)
            {
                ezlopi_valueformatter_float_to_cjson(cj_result, user_data->z_data, item->cloud_properties.scale);
            }

            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

static ezlopi_error_t __0028_notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        s_gy61_data_t *user_data = (s_gy61_data_t *)item->user_arg;
        if (user_data)
        {
            float new_value = 0;
            if (ezlopi_item_name_acceleration_x_axis == item->cloud_properties.item_name)
            {
                new_value = __update_gy61_axis_value(item);

                if (fabs((user_data->x_data) - new_value) > 0.5)
                {
                    user_data->x_data = new_value;
                    ezlopi_device_value_updated_from_device_broadcast(item);
                }
            }
            if (ezlopi_item_name_acceleration_y_axis == item->cloud_properties.item_name)
            {
                new_value = __update_gy61_axis_value(item);
                if (fabs((user_data->y_data) - new_value) > 0.5)
                {
                    user_data->y_data = new_value;
                    ezlopi_device_value_updated_from_device_broadcast(item);
                }
            }
            if (ezlopi_item_name_acceleration_z_axis == item->cloud_properties.item_name)
            {
                new_value = __update_gy61_axis_value(item);
                if (fabs((user_data->z_data) - new_value) > 0.5)
                {
                    user_data->z_data = new_value;
                    ezlopi_device_value_updated_from_device_broadcast(item);
                }
            }
            ret = EZPI_SUCCESS;
        }
    }
    return ret;
}

static float __update_gy61_axis_value(l_ezlopi_item_t *item)
{
    float G_data = 0;
    if (item)
    {
        s_ezlopi_analog_data_t ezlopi_analog_data = { .value = 0, .voltage = 0 };
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
