
#include "trace.h"
#include "cJSON.h"
#include "math.h"
#include "gpio_isr_service.h"
#include "ezlopi_adc.h"
#include "ezlopi_gpio.h"
#include "ezlopi_timer.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_valueformatter.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_valueformatter.h"

#include "sensor_0020_other_2axis_joystick.h"

typedef struct s_joystick_data
{
    uint32_t sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_MAX];
    uint32_t adc_x;
    uint32_t adc_y;
} s_joystick_data_t;

static int __prepare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __get_value_cjson(l_ezlopi_item_t *item, void *arg);
static int __notify(l_ezlopi_item_t *item);
static void __joystick_intr_callback(void *arg);

int sensor_0020_other_2axis_joystick(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;

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
    char *device_name = NULL;
    CJSON_GET_VALUE_STRING(cj_device, "dev_name", device_name);

    ASSIGN_DEVICE_NAME_V2(device, device_name);
    device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
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
            item->cloud_properties.item_name = ezlopi_item_name_voltage;
            item->cloud_properties.value_type = value_type_electric_potential;
            item->cloud_properties.scale = scales_milli_volt;
        }
        if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_Y] == item->cloud_properties.item_id)
        {
            item->cloud_properties.item_name = ezlopi_item_name_voltage;
            item->cloud_properties.value_type = value_type_electric_potential;
            item->cloud_properties.scale = scales_milli_volt;
        }
        if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_SWITCH] == item->cloud_properties.item_id)
        {
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
            CJSON_GET_VALUE_INT(cj_device, "gpio1", item->interface.adc.gpio_num);
            item->interface.adc.resln_bit = 3;
        }
        if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_Y] == item->cloud_properties.item_id)
        {
            item->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;
            CJSON_GET_VALUE_INT(cj_device, "gpio2", item->interface.adc.gpio_num);
            item->interface.adc.resln_bit = 3;
        }
        if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_SWITCH] == item->cloud_properties.item_id)
        {
            item->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;
            CJSON_GET_VALUE_INT(cj_device, "gpio3", item->interface.gpio.gpio_in.gpio_num);
            item->interface.gpio.gpio_in.enable = true;
            item->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
            item->interface.gpio.gpio_in.pull = GPIO_PULLUP_ONLY;
        }
    }
}

static int __prepare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *dev_prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (dev_prep_arg && (dev_prep_arg->cjson_device))
    {
        cJSON *cj_device = dev_prep_arg->cjson_device;
        if (cj_device)
        {
            s_joystick_data_t *user_data = (s_joystick_data_t *)malloc(sizeof(s_joystick_data_t));
            if (user_data)
            {
                memset(user_data, 0, sizeof(s_joystick_data_t));
                for (uint8_t i = 0; i < JOYSTICK_ITEM_ID_MAX; i++)
                {
                    user_data->sensor_0020_joystick_item_ids[i] = ezlopi_cloud_generate_item_id();
                }

                l_ezlopi_device_t *joystick_x_device = ezlopi_device_add_device(cj_device);
                if (joystick_x_device)
                {
                    joystick_x_device->cloud_properties.category = category_level_sensor;
                    joystick_x_device->cloud_properties.subcategory = subcategory_electricity;
                    __setup_device_cloud_properties(joystick_x_device, cj_device);

                    l_ezlopi_item_t *joystick_x_item = ezlopi_device_add_item_to_device(joystick_x_device, sensor_0020_other_2axis_joystick);
                    if (joystick_x_item)
                    {
                        joystick_x_item->cloud_properties.item_id = user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_X];
                        __setup_item_cloud_properties(joystick_x_item, user_data);
                        __setup_item_interface_properties(joystick_x_item, cj_device);
                    }
                    else
                    {
                        ezlopi_device_free_device(joystick_x_device);
                    }
                }
                l_ezlopi_device_t *joystick_y_device = ezlopi_device_add_device(cj_device);
                if (joystick_y_device)
                {
                    joystick_y_device->cloud_properties.category = category_level_sensor;
                    joystick_y_device->cloud_properties.subcategory = subcategory_electricity;
                    __setup_device_cloud_properties(joystick_y_device, cj_device);

                    l_ezlopi_item_t *joystick_y_item = ezlopi_device_add_item_to_device(joystick_y_device, sensor_0020_other_2axis_joystick);
                    if (joystick_y_item)
                    {
                        joystick_y_item->cloud_properties.item_id = user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_Y];
                        __setup_item_cloud_properties(joystick_y_item, user_data);
                        __setup_item_interface_properties(joystick_y_item, cj_device);
                    }
                    else
                    {
                        ezlopi_device_free_device(joystick_y_device);
                    }
                }
                l_ezlopi_device_t *joystick_sw_device = ezlopi_device_add_device(cj_device);
                if (joystick_sw_device)
                {
                    joystick_sw_device->cloud_properties.category = category_switch;
                    joystick_sw_device->cloud_properties.subcategory = subcategory_in_wall;
                    __setup_device_cloud_properties(joystick_sw_device, cj_device);

                    l_ezlopi_item_t *joystick_sw_item = ezlopi_device_add_item_to_device(joystick_sw_device, sensor_0020_other_2axis_joystick);
                    if (joystick_sw_item)
                    {
                        joystick_sw_item->cloud_properties.item_id = user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_SWITCH];
                        __setup_item_cloud_properties(joystick_sw_item, user_data);
                        __setup_item_interface_properties(joystick_sw_item, cj_device);
                    }
                    else
                    {
                        ezlopi_device_free_device(joystick_sw_device);
                    }
                }
                ret = 1;
            }
        }
    }

    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        s_joystick_data_t *user_data = (s_joystick_data_t *)item->user_arg;
        if ((item->cloud_properties.item_id == user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_X]) || (item->cloud_properties.item_id == user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_Y]))
        {
            if (GPIO_IS_VALID_GPIO(item->interface.adc.gpio_num))
            {
                TRACE_E("adc GPIO_NUM is %d", item->interface.adc.gpio_num);
                ezlopi_adc_init(item->interface.adc.gpio_num, item->interface.adc.resln_bit);
            }
        }
        if (item->cloud_properties.item_id == user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_SWITCH])
        {
            if (item->interface.gpio.gpio_in.enable)
            {
                TRACE_B("SW_GPIO_NUM is %d", item->interface.gpio.gpio_in.gpio_num);

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

                ret = gpio_config(&io_conf);
                if (ret)
                {
                    TRACE_E("Error initializing joystick switch");
                }
                else
                {
                    TRACE_I("Joystick switch initialize successfully.");
                    item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
                    TRACE_B("Value is %d", item->interface.gpio.gpio_in.value);
                    gpio_isr_service_register_v3(item, __joystick_intr_callback, 200);
                }
            }
        }
        ret = 1;
    }
    return ret;
}

static int __get_value_cjson(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    cJSON *cj_result = (cJSON *)arg;
    if (cj_result && item)
    {

        s_joystick_data_t *user_data = (s_joystick_data_t *)item->user_arg;
        if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_X] == item->cloud_properties.item_id)
        {
            char *valueFormatted = ezlopi_valueformatter_uint32(user_data->adc_x);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cj_result, "value", user_data->adc_x);
            cJSON_AddStringToObject(cj_result, "scale", "milli_volt");
            free(valueFormatted);
        }
        if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_Y] == item->cloud_properties.item_id)
        {

            char *valueFormatted = ezlopi_valueformatter_uint32(user_data->adc_y);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
            cJSON_AddNumberToObject(cj_result, "value", user_data->adc_y);
            cJSON_AddStringToObject(cj_result, "scale", "milli_volt");
            free(valueFormatted);
        }
        if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_SWITCH] == item->cloud_properties.item_id)
        {

            cJSON_AddBoolToObject(cj_result, "value", ((0 == item->interface.gpio.gpio_in.value) ? true : false));
            char *valueFormatted = ezlopi_valueformatter_bool((0 == item->interface.gpio.gpio_in.value) ? true : false);
            cJSON_AddStringToObject(cj_result, "valueFormatted", valueFormatted);
        }
        ret = 1;
    }
    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        if (ezlopi_item_name_switch != item->cloud_properties.item_name)
        {
            s_joystick_data_t *user_data = (s_joystick_data_t *)item->user_arg;
            s_ezlopi_analog_data_t ezlopi_analog_data = {.value = 0, .voltage = 0};

            if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_X] == item->cloud_properties.item_id)
            {
                ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
                if (fabs(user_data->adc_x - ezlopi_analog_data.voltage) > 100)
                {
                    user_data->adc_x = ezlopi_analog_data.voltage;
                    // TRACE_I("X-axis value is %d and voltage is %d", ezlopi_analog_data.value, ezlopi_analog_data.voltage);
                    ezlopi_device_value_updated_from_device_item_id_v3(user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_X]);
                }
            }
            if (user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_Y] == item->cloud_properties.item_id)
            {
                ezlopi_adc_get_adc_data(item->interface.adc.gpio_num, &ezlopi_analog_data);
                if (fabs(user_data->adc_y - ezlopi_analog_data.voltage) > 100)
                {
                    user_data->adc_y = ezlopi_analog_data.voltage;
                    // TRACE_I("Y-axis value is %d and voltage is %d", ezlopi_analog_data.value, ezlopi_analog_data.voltage);
                    ezlopi_device_value_updated_from_device_item_id_v3(user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_Y]);
                }
            }
        }
        ret = 1;
    }
    return ret;
}

static void __joystick_intr_callback(void *arg)
{
    l_ezlopi_item_t *item = (l_ezlopi_item_t *)arg;
    if (item)
    {
        s_joystick_data_t *user_data = (s_joystick_data_t *)item->user_arg;
        item->interface.gpio.gpio_in.value = gpio_get_level(item->interface.gpio.gpio_in.gpio_num);
        item->interface.gpio.gpio_in.value = (false == item->interface.gpio.gpio_in.invert) ? (item->interface.gpio.gpio_in.value) : (!item->interface.gpio.gpio_in.value);
        ezlopi_device_value_updated_from_device_item_id_v3(user_data->sensor_0020_joystick_item_ids[JOYSTICK_ITEM_ID_SWITCH]);
    }
}
