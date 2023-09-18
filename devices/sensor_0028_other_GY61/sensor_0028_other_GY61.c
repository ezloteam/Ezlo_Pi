
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

#include "sensor_0028_other_GY61.h"
//---------------------------------------------------------------------

#define ADD_PROPERTIES_DEVICE_LIST(device_id, category, subcategory, item_name, value_type, cjson_device)                              \
    {                                                                                                                                  \
        s_ezlopi_device_properties_t *_properties = sensor_gy61_analog_sensor_prepare_properties(device_id, category, subcategory,     \
                                                                                                 item_name, value_type, cjson_device); \
        if (NULL != _properties)                                                                                                       \
        {                                                                                                                              \
            add_device_to_list(prep_arg, _properties, NULL);                                                                           \
        }                                                                                                                              \
    }

//---------------------------------------------------------------------

// conversion for acd to G

#ifdef CONFIG_IDF_TARGET_ESP32
#define esp32_convert_mV_to_G(temp_vol) (((6.0f * (temp_vol - 1300)) / 750.0f) - 3.0f)
#elif CONFIG_IDF_TARGET_ESP32S3
#define esp32S3_convert_mV_to_G(temp_vol) (((6.0f / 3000.0f) * (temp_vol - 100)) - 3.0f)
#endif

/********************************** DECLARATION ******************************************/
static int sensor_gy61_analog_sensor_prepare_and_add(void *args);
static s_ezlopi_device_properties_t *sensor_gy61_analog_sensor_prepare_properties(uint32_t DEVICE_ID, const char *CATEGORY, const char *SUB_CATEGORY, const char *ITEM_NAME, const char *VALUE_TYPE, cJSON *cjson_device);
static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *sensor_gy61_analog_sensor_properties, void *user_arg);
static int sensor_gy61_analog_sensor_init(s_ezlopi_device_properties_t *properties);
static float get_gy61_x_axis_value(s_ezlopi_device_properties_t *properties);
static float get_gy61_y_axis_value(s_ezlopi_device_properties_t *properties);
static float get_gy61_z_axis_value(s_ezlopi_device_properties_t *properties);
static int get_sensor_gy61_analog_sensor_value(s_ezlopi_device_properties_t *properties, void *args);

/**********************************DEFINATION ******************************************/

int sensor_0028_other_GY61(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = sensor_gy61_analog_sensor_prepare_and_add(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = sensor_gy61_analog_sensor_init(properties);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        get_sensor_gy61_analog_sensor_value(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        ezlopi_device_value_updated_from_device(properties);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static int sensor_gy61_analog_sensor_prepare_and_add(void *args)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)args;
    if ((NULL != prep_arg) && (NULL != prep_arg->cjson_device))
    {
        uint32_t device_id = 0;
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_acceleration_x_axis, value_type_int, prep_arg->cjson_device);
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_acceleration_y_axis, value_type_int, prep_arg->cjson_device);
        device_id = ezlopi_cloud_generate_device_id();
        ADD_PROPERTIES_DEVICE_LIST(device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_acceleration_z_axis, value_type_int, prep_arg->cjson_device);
    }
    return ret;
}

static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *sensor_gy61_analog_sensor_properties, void *user_arg)
{
    int ret = 0;

    if (sensor_gy61_analog_sensor_properties)
    {
        if (0 == ezlopi_devices_list_add(prep_arg->device, sensor_gy61_analog_sensor_properties, NULL))
        {
            free(sensor_gy61_analog_sensor_properties);
        }
        else
        {
            ret = 1;
        }
    }
    return ret;
}

static s_ezlopi_device_properties_t *sensor_gy61_analog_sensor_prepare_properties(uint32_t DEVICE_ID, const char *CATEGORY, const char *SUB_CATEGORY, const char *ITEM_NAME, const char *VALUE_TYPE, cJSON *cjson_device)
{
    s_ezlopi_device_properties_t *sensor_gy61_analog_sensor_properties = malloc(sizeof(s_ezlopi_device_properties_t));

    if (sensor_gy61_analog_sensor_properties)
    {
        memset(sensor_gy61_analog_sensor_properties, 0, sizeof(s_ezlopi_device_properties_t));
        sensor_gy61_analog_sensor_properties->interface_type = EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT;

        char *device_name = NULL;
        if (ezlopi_item_name_acceleration_x_axis == ITEM_NAME)
        {
            device_name = "Acceleration-X";
        }
        if (ezlopi_item_name_acceleration_y_axis == ITEM_NAME)
        {
            device_name = "Acceleration-Y";
        }
        if (ezlopi_item_name_acceleration_z_axis == ITEM_NAME)
        {
            device_name = "Acceleration-Z";
        }
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
        ASSIGN_DEVICE_NAME(sensor_gy61_analog_sensor_properties, device_name);
        sensor_gy61_analog_sensor_properties->ezlopi_cloud.category = CATEGORY;
        sensor_gy61_analog_sensor_properties->ezlopi_cloud.subcategory = SUB_CATEGORY;
        sensor_gy61_analog_sensor_properties->ezlopi_cloud.item_name = ITEM_NAME;
        sensor_gy61_analog_sensor_properties->ezlopi_cloud.device_type = dev_type_sensor;
        sensor_gy61_analog_sensor_properties->ezlopi_cloud.value_type = VALUE_TYPE;
        sensor_gy61_analog_sensor_properties->ezlopi_cloud.has_getter = true;
        sensor_gy61_analog_sensor_properties->ezlopi_cloud.has_setter = false;
        sensor_gy61_analog_sensor_properties->ezlopi_cloud.reachable = true;
        sensor_gy61_analog_sensor_properties->ezlopi_cloud.battery_powered = false;
        sensor_gy61_analog_sensor_properties->ezlopi_cloud.show = true;
        sensor_gy61_analog_sensor_properties->ezlopi_cloud.room_name[0] = '\0';
        sensor_gy61_analog_sensor_properties->ezlopi_cloud.device_id = DEVICE_ID;
        sensor_gy61_analog_sensor_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        sensor_gy61_analog_sensor_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        if (ezlopi_item_name_acceleration_x_axis == ITEM_NAME)
        {
            CJSON_GET_VALUE_INT(cjson_device, "gpio1", sensor_gy61_analog_sensor_properties->interface.adc.gpio_num);
            TRACE_I("Accel X-axis gpio1: %d ", sensor_gy61_analog_sensor_properties->interface.adc.gpio_num);
        }
        else if (ezlopi_item_name_acceleration_y_axis == ITEM_NAME)
        {
            CJSON_GET_VALUE_INT(cjson_device, "gpio2", sensor_gy61_analog_sensor_properties->interface.adc.gpio_num);
            TRACE_I("Accel Y-axis gpio2: %d ", sensor_gy61_analog_sensor_properties->interface.adc.gpio_num);
        }
        else if (ezlopi_item_name_acceleration_z_axis == ITEM_NAME)
        {
            CJSON_GET_VALUE_INT(cjson_device, "gpio3", sensor_gy61_analog_sensor_properties->interface.adc.gpio_num);
            TRACE_I("Accel Z-axis gpio3: %d ", sensor_gy61_analog_sensor_properties->interface.adc.gpio_num);
        }
        sensor_gy61_analog_sensor_properties->interface.adc.resln_bit = 3; // ADC_RES_12_BIT :
    }
    return sensor_gy61_analog_sensor_properties;
}

static int sensor_gy61_analog_sensor_init(s_ezlopi_device_properties_t *properties)
{

    int ret = 0;
    if (GPIO_IS_VALID_GPIO(properties->interface.adc.gpio_num))
    {
        ezlopi_adc_init(properties->interface.adc.gpio_num, properties->interface.adc.resln_bit);
        ret = 1;
    }
    return ret;
}

static int get_sensor_gy61_analog_sensor_value(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;
    cJSON *cjson_properties = (cJSON *)arg;
    int acceleration_value;
    if (cjson_properties)
    {
        if (ezlopi_item_name_acceleration_x_axis == properties->ezlopi_cloud.item_name)
        {
            acceleration_value = (int)(get_gy61_x_axis_value(properties) * GY61_STANDARD_G_TO_ACCEL_CONVERSION_VALUE);
            TRACE_I("X-axis : %d m/s^2", acceleration_value);
            cJSON_AddNumberToObject(cjson_properties, "value", acceleration_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter_per_square_second");
        }
        if (ezlopi_item_name_acceleration_y_axis == properties->ezlopi_cloud.item_name)
        {
            acceleration_value = (int)(get_gy61_y_axis_value(properties) * GY61_STANDARD_G_TO_ACCEL_CONVERSION_VALUE);
            TRACE_I("Y-axis : %d m/s^2", acceleration_value);
            cJSON_AddNumberToObject(cjson_properties, "value", acceleration_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter_per_square_second");
        }
        if (ezlopi_item_name_acceleration_z_axis == properties->ezlopi_cloud.item_name)
        {
            acceleration_value = (int)(get_gy61_z_axis_value(properties) * GY61_STANDARD_G_TO_ACCEL_CONVERSION_VALUE);
            TRACE_I("Z-axis : %d  m/s^2", acceleration_value);
            cJSON_AddNumberToObject(cjson_properties, "value", acceleration_value);
            cJSON_AddStringToObject(cjson_properties, "scale", "meter_per_square_second");
        }
        ret = 1;
    }

    return ret;
}

static float get_gy61_x_axis_value(s_ezlopi_device_properties_t *properties)
{
    float X_G_data = 0;
    int temp_vol = 0;
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
    ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data); // returns channel
    // TRACE_E("gpio-num is %d", properties->interface.adc.gpio_num);

    temp_vol = (int)ezlopi_analog_data->voltage;
#ifdef CONFIG_IDF_TARGET_ESP32
    if (temp_vol <= 1300)
    {
        X_G_data = 0;
    }
    else if (temp_vol >= 2050)
    {
        X_G_data = 3;
    }
    else
    {
        X_G_data = esp32_convert_mV_to_G(temp_vol);
    }
#elif CONFIG_IDF_TARGET_ESP32S3
    if (temp_vol <= 100)
    {
        X_G_data = 0;
    }
    else if (temp_vol >= 3100)
    {
        X_G_data = 3;
    }
    else
    {
        X_G_data = esp32S3_convert_mV_to_G(temp_vol);
    }
#endif

    free(ezlopi_analog_data);

    TRACE_B("X_Vol_data : {%d mV} ;  X_G_data : {%.2fG}", temp_vol, X_G_data);
    return X_G_data;
}

static float get_gy61_y_axis_value(s_ezlopi_device_properties_t *properties)
{
    float Y_G_data = 0;
    int temp_vol = 0;
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
    ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data); // returns channel
    temp_vol = (int)ezlopi_analog_data->voltage;

#ifdef CONFIG_IDF_TARGET_ESP32
    if (temp_vol <= 1300)
    {
        Y_G_data = 0;
    }
    else if (temp_vol >= 2050)
    {
        Y_G_data = 3;
    }
    else
    {
        Y_G_data = esp32_convert_mV_to_G(temp_vol);
    }
#elif CONFIG_IDF_TARGET_ESP32S3
    if (temp_vol <= 100)
    {
        Y_G_data = 0;
    }
    else if (temp_vol >= 3100)
    {
        Y_G_data = 3;
    }
    else
    {
        Y_G_data = esp32S3_convert_mV_to_G(temp_vol);
    }
#endif
    free(ezlopi_analog_data);

    TRACE_B("Y_Vol_data : {%d mV} ;  Y_G_data : {%.2fG}", temp_vol, Y_G_data);
    return Y_G_data;
}
static float get_gy61_z_axis_value(s_ezlopi_device_properties_t *properties)
{

    float Z_G_data = 0;
    int temp_vol = 0;
    s_ezlopi_analog_data_t *ezlopi_analog_data = (s_ezlopi_analog_data_t *)malloc(sizeof(s_ezlopi_analog_data_t));
    memset(ezlopi_analog_data, 0, sizeof(s_ezlopi_analog_data_t));
    ezlopi_adc_get_adc_data(properties->interface.adc.gpio_num, ezlopi_analog_data); // returns channel

    temp_vol = (int)ezlopi_analog_data->voltage;
#ifdef CONFIG_IDF_TARGET_ESP32
    if (temp_vol <= 1300)
    {
        Z_G_data = 0;
    }
    else if (temp_vol >= 2050)
    {
        Z_G_data = 3;
    }
    else
    {
        Z_G_data = esp32_convert_mV_to_G(temp_vol);
    }
#elif CONFIG_IDF_TARGET_ESP32S3
    if (temp_vol <= 100)
    {
        Z_G_data = 0;
    }
    else if (temp_vol >= 3100)
    {
        Z_G_data = 3;
    }
    else
    {
        Z_G_data = esp32S3_convert_mV_to_G(temp_vol);
    }
#endif
    free(ezlopi_analog_data);
    TRACE_B("Z_Vol_data : {%d mV} ;  Z_G_data : {%0.2fG}", temp_vol, Z_G_data);
    return Z_G_data;
}