
#include <math.h>
#include "ezlopi_util_trace.h"

// #include "ezlopi_core_timer.h"
#include "ezlopi_core_cloud.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_valueformatter.h"
#include "ezlopi_core_device_value_updated.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_hal_adc.h"
#include "ezlopi_hal_gpio.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

#include "ezlopi_service_gpioisr.h"

#include "sensor_0020_other_2axis_joystick.h"
#include "EZLOPI_USER_CONFIG.h"

typedef struct s_joystick_data
{
    uint32_t sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_MAX];
    uint32_t adc_x;
    uint32_t adc_y;
} s_joystick_data_t;

static ezlopi_error_t __prepare(void *arg);
static ezlopi_error_t __init(l_ezlopi_item_t *item);
static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg);
static ezlopi_error_t __notify(l_ezlopi_item_t *item);
static void __joystick_intr_callback(void *arg);

ezlopi_error_t sensor_0020_other_2axis_joystick(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;

    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __prepare(arg);
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
        ret = __get_value_cjson(item, arg);
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

static void __setup_device_cloud_properties(l_ezlopi_device_t *device, cJSON *cj_device)
{
    device->cloud_properties.info = NULL;
    device->cloud_properties.device_type_id = NULL;
    device->cloud_properties.device_type = dev_type_device;
}

static void __setup_item_cloud_properties(l_ezlopi_item_t *item, s_joystick_data_t *user_data)
{
    if (item)
    {
        item->cloud_properties.has_getter = true;
        item->cloud_properties.has_setter = true;
        item->cloud_properties.show = true;
        item->user_arg = (void *)user_data;

        if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_X] == item->cloud_properties.item_id)
        {
            item->is_user_arg_unique = true;
            item->cloud_properties.item_name = ezlopi_item_name_voltage;
            item->cloud_properties.value_type = value_type_electric_potential;
            item->cloud_properties.scale = scales_milli_volt;
        }
        else if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_Y] == item->cloud_properties.item_id)
        {

            item->is_user_arg_unique = false;
            item->cloud_properties.item_name = ezlopi_item_name_voltage;
            item->cloud_properties.value_type = value_type_electric_potential;
            item->cloud_properties.scale = scales_milli_volt;
        }
        else if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_SWITCH] == item->cloud_properties.item_id)
        {

            item->is_user_arg_unique = false;
            item->cloud_properties.item_name = ezlopi_item_name_switch;
            item->cloud_properties.value_type = value_type_bool;
            item->cloud_properties.scale = NULL;
        }
    }
}

static void __setup_item_interface_properties(l_ezlopi_item_t *item, cJSON *cj_device)
{
    if (item)
    {
        s_joystick_data_t *user_data = (s_joystick_data_t *)item->user_arg;
        if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_X] == item->cloud_properties.item_id)
        {
            item->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;
            CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio1_str, item->interface.adc.gpio_num);
            item->interface.adc.resln_bit = 3;
        }
        else if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_Y] == item->cloud_properties.item_id)
        {
            item->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;
            CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio2_str, item->interface.adc.gpio_num);
            item->interface.adc.resln_bit = 3;
        }
        else if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_SWITCH] == item->cloud_properties.item_id)
        {
            item->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;
            CJSON_GET_VALUE_GPIO(cj_device, ezlopi_gpio3_str, item->interface.gpio.gpio_in.gpio_num);
            item->interface.gpio.gpio_in.enable = true;
            item->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
            item->interface.gpio.gpio_in.pull = GPIO_PULLUP_ONLY;
        }
    }
}

static ezlopi_error_t __prepare(void *arg)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    s_ezlopi_prep_arg_t *dev_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (dev_prep_arg && (dev_prep_arg->cjson_device))
    {
        cJSON *cj_device = dev_prep_arg->cjson_device;
        if (cj_device)
        {
            s_joystick_data_t *user_data = (s_joystick_data_t *)ezlopi_malloc(__FUNCTION__, sizeof(s_joystick_data_t));
            if (user_data)
            {
                memset(user_data, 0, sizeof(s_joystick_data_t));
                for (uint8_t i = 0; i < JOYSTICK_ITEM_ID_MAX; i++)
                {
                    user_data->sensor_0020_joystick_item_ids[i] = ezlopi_cloud_generate_item_id();
                }

                l_ezlopi_device_t *joystick_parent_x_device = ezlopi_device_add_device(cj_device, "x_axis");
                if (joystick_parent_x_device)
                {
                    ret = 1;
                    TRACE_I("Parent_joystick_x_device-[0x%x] ", joystick_parent_x_device->cloud_properties.device_id);
                    joystick_parent_x_device->cloud_properties.category = category_level_sensor;
                    joystick_parent_x_device->cloud_properties.subcategory = subcategory_electricity;
                    __setup_device_cloud_properties(joystick_parent_x_device, cj_device);

                    l_ezlopi_item_t *joystick_x_item = ezlopi_device_add_item_to_device(joystick_parent_x_device, sensor_0020_other_2axis_joystick);
                    if (joystick_x_item)
                    {
                        joystick_x_item->cloud_properties.item_id = user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_X];
                        joystick_x_item->is_user_arg_unique = true; // allow to clear in 'parent_device->item_x'

                        __setup_item_cloud_properties(joystick_x_item, user_data);
                        __setup_item_interface_properties(joystick_x_item, cj_device);
                    }

                    l_ezlopi_device_t *joystick_child_y_device = ezlopi_device_add_device(cj_device, "y-axis");
                    if (joystick_child_y_device)
                    {
                        // assigning parent_device_id to child_device
                        TRACE_I("Child_joystick_y_device-[0x%x] ", joystick_child_y_device->cloud_properties.device_id);

                        joystick_child_y_device->cloud_properties.category = category_level_sensor;
                        joystick_child_y_device->cloud_properties.subcategory = subcategory_electricity;
                        __setup_device_cloud_properties(joystick_child_y_device, cj_device);

                        l_ezlopi_item_t *joystick_y_item = ezlopi_device_add_item_to_device(joystick_child_y_device, sensor_0020_other_2axis_joystick);
                        if (joystick_y_item)
                        {
                            joystick_y_item->cloud_properties.item_id = user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_Y];

                            __setup_item_cloud_properties(joystick_y_item, user_data);
                            __setup_item_interface_properties(joystick_y_item, cj_device);
                        }
                        else
                        {
                            ezlopi_device_free_device(joystick_child_y_device);
                            ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                        }
                    }

                    l_ezlopi_device_t *joystick_child_sw_device = ezlopi_device_add_device(cj_device, "switch");
                    if (joystick_child_sw_device)
                    {
                        // assigning parent_device_id to child_device
                        TRACE_I("Child_joystick_sw_device-[0x%x] ", joystick_child_sw_device->cloud_properties.device_id);

                        joystick_child_sw_device->cloud_properties.category = category_switch;
                        joystick_child_sw_device->cloud_properties.subcategory = subcategory_in_wall;
                        __setup_device_cloud_properties(joystick_child_sw_device, cj_device);

                        l_ezlopi_item_t *joystick_sw_item = ezlopi_device_add_item_to_device(joystick_child_sw_device, sensor_0020_other_2axis_joystick);
                        if (joystick_sw_item)
                        {
                            joystick_sw_item->cloud_properties.item_id = user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_SWITCH];

                            __setup_item_cloud_properties(joystick_sw_item, user_data);
                            __setup_item_interface_properties(joystick_sw_item, cj_device);
                        }
                        else
                        {
                            ezlopi_device_free_device(joystick_child_sw_device);
                            ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                        }
                    }

                    if ((NULL == joystick_x_item) &&
                        (NULL == joystick_child_y_device) &&
                        (NULL == joystick_child_sw_device))
                    {
                        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                        ezlopi_device_free_device(joystick_parent_x_device);
                    }
                }
                else
                {
                    ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
                    ezlopi_free(__FUNCTION__, user_data); // needed here only
                }
            }
            else
            {
                ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
            }
        }
        else
        {
            ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
        }
    }
    else
    {
        ret = EZPI_ERR_PREP_DEVICE_PREP_FAILED;
    }

    return ret;
}

static ezlopi_error_t __init(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_SUCCESS;
    if (item)
    {
        s_joystick_data_t *user_data = (s_joystick_data_t *)item->user_arg;
        if (user_data)
        {
            if ((item->cloud_properties.item_id == user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_X]) ||
                (item->cloud_properties.item_id == user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_Y]))
            {
                if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
                {
                    TRACE_I("adc GPIO_NUM is %d", item->interface.adc.gpio_num);
                    if (EZPI_SUCCESS == ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit))
                    {
                        ret = EZPI_SUCCESS;
                    }
                    else
                    {
                        ret = EZPI_ERR_INIT_DEVICE_FAILED;
                    }
                }
                else
                {
                    ret = EZPI_ERR_INIT_DEVICE_FAILED;
                }
            }
            else if (item->cloud_properties.item_id == user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_SWITCH] &&
                     (item->interface.gpio.gpio_in.enable))
            {
                if (GPIO_IS_VALID_GPIO(item->interface.gpio.gpio_in.gpio_num))
                {
                    TRACE_I("SW_GPIO_NUM is %d", item->interface.gpio.gpio_in.gpio_num);

                    const gpio_config_t io_conf = {
                        .pin_bit_mask = (1ULL << item->interface.gpio.gpio_in.gpio_num),
                        .mode = GPIO_MODE_INPUT,
                        .pull_up_en = ((item->interface.gpio.gpio_in.pull == GPIO_PULLUP_ONLY) ||
                                       (item->interface.gpio.gpio_in.pull == GPIO_PULLUP_PULLDOWN))
                                          ? GPIO_PULLUP_ENABLE
                                          : GPIO_PULLUP_DISABLE,
                        .pull_down_en = GPIO_PULLDOWN_DISABLE,
                        .intr_type = item->interface.gpio.gpio_in.interrupt,
                    };

                    if (ESP_OK == gpio_config(&io_conf))
                    {
                        TRACE_I("Joystick switch initialize successfully.");
                        item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
                        // TRACE_D("Value is %d", (int)item->interface.gpio.gpio_in.value);
                        EZPI_service_gpioisr_register_v3(item, __joystick_intr_callback, 200);
                    }
                    else
                    {
                        ret = EZPI_ERR_INIT_DEVICE_FAILED;
                    }
                }
                else
                {
                    ret = EZPI_ERR_INIT_DEVICE_FAILED;
                }
            }
            else
            {
                TRACE_E("Unknown....Item_id : [%d]", item->cloud_properties.item_id);
                ret = EZPI_ERR_INIT_DEVICE_FAILED;
            }
        }
        else
        {
            ret = EZPI_ERR_INIT_DEVICE_FAILED;
        }
    }
    else
    {
        ret = EZPI_ERR_INIT_DEVICE_FAILED;
    }
    return ret;
}

static ezlopi_error_t __get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    ezlopi_error_t ret = EZPI_FAILED;
    cJSON *cj_result = (cJSON *)arg;
    if (cj_result && item)
    {

        s_joystick_data_t *user_data = (s_joystick_data_t *)item->user_arg;
        if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_X] == item->cloud_properties.item_id)
        {
            ezlopi_valueformatter_uint32_to_cjson(cj_result, user_data->adc_x, item->cloud_properties.scale);
        }
        else if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_Y] == item->cloud_properties.item_id)
        {
            ezlopi_valueformatter_uint32_to_cjson(cj_result, user_data->adc_y, item->cloud_properties.scale);
        }
        else if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_SWITCH] == item->cloud_properties.item_id)
        {
            ezlopi_valueformatter_bool_to_cjson(cj_result, (0 == item->interface.gpio.gpio_in.value) ? true : false, item->cloud_properties.scale);
        }
        ret = EZPI_SUCCESS;
    }
    return ret;
}

static ezlopi_error_t __notify(l_ezlopi_item_t *item)
{
    ezlopi_error_t ret = EZPI_FAILED;
    if (item)
    {
        if (ezlopi_item_name_switch != item->cloud_properties.item_name)
        {
            s_joystick_data_t *user_data = (s_joystick_data_t *)item->user_arg;
            if (user_data)
            {
                s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0, .voltage = 0};

                if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_X] == item->cloud_properties.item_id)
                {
                    ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
                    if (fabs(user_data->adc_x - ezlopi_analog_data.voltage) > 100)
                    {
                        user_data->adc_x = ezlopi_analog_data.voltage;
                        // TRACE_S("X-axis value is %d and voltage is %d", ezlopi_analog_data.value, ezlopi_analog_data.voltage);
                        ezlopi_device_value_updated_from_device_broadcast_by_item_id(user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_X]);
                    }
                }
                else if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_Y] == item->cloud_properties.item_id)
                {
                    ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
                    if (fabs(user_data->adc_y - ezlopi_analog_data.voltage) > 100)
                    {
                        user_data->adc_y = ezlopi_analog_data.voltage;
                        // TRACE_S("Y-axis value is %d and voltage is %d", ezlopi_analog_data.value, ezlopi_analog_data.voltage);
                        ezlopi_device_value_updated_from_device_broadcast_by_item_id(user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_Y]);
                    }
                }
            }
        }
        ret = EZPI_SUCCESS;
    }
    return ret;
}

static void __joystick_intr_callback(void *arg)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)arg;
    if (item)
    {
        s_joystick_data_t *user_data = (s_joystick_data_t *)item->user_arg;
        if (user_data)
        {
            item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
            item->interface.gpio.gpio_in.value = (false == item->interface.gpio.gpio_in.invert) ? (item->interface.gpio.gpio_in.value) : (!item->interface.gpio.gpio_in.value);
            ezlopi_device_value_updated_from_device_broadcast_by_item_id(user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_SWITCH]);
        }
    }
}
