

#include "sensor_0020_ADC_2axis_joystick.h"
#include "cJSON.h"
#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_category_str.h"
#include "ezlopi_cloud_subcategory_str.h"
#include "ezlopi_item_name_str.h"
#include "ezlopi_cloud_device_types_str.h"
#include "ezlopi_cloud_value_type_str.h"
#include "trace.h"
#include "ezlopi_adc.h"
#include "gpio_isr_service.h"
#include "ezlopi_valueformatter.h"

#define ADD_PROPERTIES_DEVICE_LIST(device_id, category, sub_category, item_name, value_type, cjson_device)        \
    {                                                                                                             \
        s_ezlopi_device_properties_t *_properties = joystick_2_axis_prepare(device_id, category, sub_category,    \
                                                                            item_name, value_type, cjson_device); \
        if (NULL != _properties)                                                                                  \
        {                                                                                                         \
            add_device_to_list(device_prep_arg, _properties, NULL);                                               \
        }                                                                                                         \
    }

static int joystick_2_axis_prepare_and_add(void *args);
static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_args);
static s_ezlopi_device_properties_t *joystick_2_axis_prepare(uint32_t device_id, const char *category,
                                                             const char *sub_category, const char *item_name,
                                                             const char *value_type, cJSON *cjson_device);
static int joystick_2_axis_init(s_ezlopi_device_properties_t *properties);
static void joystick_2_axis_switch_update_from_device(void *params);
static int get_joystick_2_axis_value(s_ezlopi_device_properties_t *properties, void *args);

int sensor_0020_ADC_2axis_joystick(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *ezlo_device, void *arg, void *user_args)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = joystick_2_axis_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = joystick_2_axis_init(ezlo_device);
        break;
    }
    case EZLOPI_ACTION_HUB_GET_ITEM:
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        get_joystick_2_axis_value(ezlo_device, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        if (0 != strcmp(ezlo_device->ezlopi_cloud.item_name, ezlopi_item_name_switch))
        {
            ezlopi_device_value_updated_from_device(ezlo_device);
        }
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static int joystick_2_axis_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *device_prep_arg = (s_ezlopi_prep_arg_t *)args;

    if ((NULL != device_prep_arg) && (NULL != device_prep_arg->cjson_device))
    {
        uint32_t device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_level_sensor, subcategory_electricity, ezlopi_item_name_voltage, value_type_electric_potential, device_prep_arg->cjson_device);
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_level_sensor, subcategory_electricity, ezlopi_item_name_voltage, value_type_electric_potential, device_prep_arg->cjson_device);
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_switch, subcategory_in_wall, ezlopi_item_name_switch, value_type_bool, device_prep_arg->cjson_device);
    }
    return ret;
}

static s_ezlopi_device_properties_t *joystick_2_axis_prepare(uint32_t device_id, const char *category,
                                                             const char *sub_category, const char *item_name,
                                                             const char *value_type, cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *joystick_2_axis_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (joystick_2_axis_properties)
    {
        memset(joystick_2_axis_properties, 0, sizeof(s_ezlopi_device_properties_t));

        joystick_2_axis_properties->ezlopi_cloud.category = category;
        joystick_2_axis_properties->ezlopi_cloud.subcategory = sub_category;
        joystick_2_axis_properties->ezlopi_cloud.item_name = item_name;
        joystick_2_axis_properties->ezlopi_cloud.device_type = dev_type_device;
        joystick_2_axis_properties->ezlopi_cloud.value_type = value_type;
        joystick_2_axis_properties->ezlopi_cloud.has_getter = true;
        joystick_2_axis_properties->ezlopi_cloud.has_setter = false;
        joystick_2_axis_properties->ezlopi_cloud.reachable = true;
        joystick_2_axis_properties->ezlopi_cloud.battery_powered = false;
        joystick_2_axis_properties->ezlopi_cloud.show = true;
        joystick_2_axis_properties->ezlopi_cloud.room_name[0] = '\0';
        joystick_2_axis_properties->ezlopi_cloud.device_id = ezlopi_cloud_generate_device_id();
        joystick_2_axis_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        joystick_2_axis_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        char *device_name = NULL;
        if (strcmp(item_name, "joystick-x") == 0)
        {
            joystick_2_axis_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;
            device_name = "Joystick-X";
            CJSON_GET_VALUE_INT(cjson_device, "gpio_x", joystick_2_axis_properties->interface.adc.gpio_num);
            CJSON_GET_VALUE_INT(cjson_device, "resln_bit", joystick_2_axis_properties->interface.adc.resln_bit);
            joystick_2_axis_properties->interface.adc.resln_bit = 3;
        }
        if (strcmp(item_name, "joystick-y") == 0)
        {
            joystick_2_axis_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;
            device_name = "Joystick-Y";
            CJSON_GET_VALUE_INT(cjson_device, "gpio_y", joystick_2_axis_properties->interface.adc.gpio_num);
            CJSON_GET_VALUE_INT(cjson_device, "resln_bit", joystick_2_axis_properties->interface.adc.resln_bit);
            joystick_2_axis_properties->interface.adc.resln_bit = 3;
        }
        if (ezlopi_item_name_switch == item_name)
        {
            joystick_2_axis_properties->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT;
            device_name = "Joystick switch";
            CJSON_GET_VALUE_INT(cjson_device, "gpio_sw", joystick_2_axis_properties->interface.gpio.gpio_in.gpio_num);
            joystick_2_axis_properties->interface.gpio.gpio_out.enable = false;
            joystick_2_axis_properties->interface.gpio.gpio_out.gpio_num = -1;
            joystick_2_axis_properties->interface.gpio.gpio_in.enable = true;
            joystick_2_axis_properties->interface.gpio.gpio_in.interrupt = GPIO_INTR_ANYEDGE;
            joystick_2_axis_properties->interface.gpio.gpio_in.pull = GPIO_PULLUP_ONLY;
        }
        ASSIGN_DEVICE_NAME(joystick_2_axis_properties, device_name);
    }

    return joystick_2_axis_properties;
}

static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_args)
{
    int ret = 0;

    if (properties)
    {
        if (0 == ezlopi_devices_list_add(prep_arg->device, properties, user_args))
        {
            free(properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

static int joystick_2_axis_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    if ((0 == strcmp(properties->ezlopi_cloud.item_name, "joystick-x")) || (0 == strcmp(properties->ezlopi_cloud.item_name, "joystick-y")))
    {
        TRACE_E("HERE!!, GPIO_NUM is %d", properties->interface.adc.gpio_num);
        ezlopi_adc_init(properties->interface.adc.gpio_num, properties->interface.adc.resln_bit);
        ret = 1;
    }
    if (0 == strcmp(properties->ezlopi_cloud.item_name, ezlopi_item_name_switch))
    {
        TRACE_B("HERE!!, gpio num is %d", properties->interface.gpio.gpio_in.gpio_num);
        TRACE_B("interrupt type is %d", properties->interface.gpio.gpio_in.interrupt);
        const gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << properties->interface.gpio.gpio_in.gpio_num),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = properties->interface.gpio.gpio_in.interrupt,
        };

        ret = gpio_config(&io_conf);
        if (ret)
        {
            TRACE_E("Error initializing joystick switch");
        }
        else
        {
            TRACE_I("Joystick switch initialize successfully.");
            properties->interface.gpio.gpio_in.value = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
            TRACE_B("Value is %d", properties->interface.gpio.gpio_in.value);
        }
        // gpio_isr_service_register(properties, joystick_2_axis_switch_update_from_device, 100);
        xTaskCreate(joystick_2_axis_switch_update_from_device, "joystick_2_axis_switch_update_from_device", 2048, properties, 0, NULL);
    }
    return ret;
}

static void joystick_2_axis_switch_update_from_device(void *params)
{
    s_ezlopi_device_properties_t *properties = (s_ezlopi_device_properties_t *)params;
    uint32_t switch_state = 0;
    while (1)
    {
        properties->interface.gpio.gpio_in.value = gpio_get_level(properties->interface.gpio.gpio_in.gpio_num);
        ezlopi_device_value_updated_from_device(properties);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

static int get_joystick_2_axis_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_propertise = (cJSON *)arg;
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    if (cjson_propertise)
    {
        memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
        if ((0 == strcmp(properties->ezlopi_cloud.item_name, "joystick-x")))
        {
            ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data);
            cJSON_AddNumberToObject(cjson_propertise, "value", ezlopi_analog_data->voltage);
            cJSON_AddStringToObject(cjson_propertise, "scale", "milli_volt");
            char *valueFormatted = ezlopi_valueformatter_uint32(ezlopi_analog_data->voltage);
            cJSON_AddStringToObject(cjson_propertise, "valueFormatted", valueFormatted);
            free(valueFormatted);
            TRACE_B("X-axis value is %d and voltage is %d", ezlopi_analog_data->value, ezlopi_analog_data->voltage);
        }
        if ((0 == strcmp(properties->ezlopi_cloud.item_name, "joystick-y")))
        {
            ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data);

            cJSON_AddNumberToObject(cjson_propertise, "value", ezlopi_analog_data->voltage);
            char *valueFormatted = ezlopi_valueformatter_uint32(ezlopi_analog_data->voltage);
            cJSON_AddStringToObject(cjson_propertise, "valueFormatted", valueFormatted);
            free(valueFormatted);
            cJSON_AddStringToObject(cjson_propertise, "scale", "milli_volt");
            TRACE_B("Y-axis value is %d and voltage is %d", ezlopi_analog_data->value, ezlopi_analog_data->voltage);
        }
        if (0 == strcmp(properties->ezlopi_cloud.item_name, ezlopi_item_name_switch))
        {
            cJSON_AddBoolToObject(cjson_propertise, "value", ((0 == properties->interface.gpio.gpio_in.value) ? true : false));
            char *valueFormatted = ezlopi_valueformatter_bool(properties->interface.gpio.gpio_in.value ? true : false);
            cJSON_AddStringToObject(cjson_propertise, "valueFormatted", valueFormatted);
        }
        ret = 1;
    }
    free(ezlopi_analog_data);

    return ret;
}