

#include "cJSON.h"
#include "trace.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"
#include "stdlib.h"

#include "sensor_0014_UART_PMS5003.h"
#include "pms5003.h"

#define ADD_PROPERTIES_DEVICE_LIST(_properties, device_id, category, sub_category, item_name, value_type, cjson_device, pms5003_sensor_structure) \
    {                                                                                                                                             \
        _properties = sensor_0014_UART_PMS5003_prepare_properties(device_id, category, sub_category,                                              \
                                                                  item_name, value_type, cjson_device, pms5003_sensor_structure);                 \
        if (NULL != _properties)                                                                                                                  \
        {                                                                                                                                         \
            add_device_to_list(prep_arg, _properties, NULL);                                                                                      \
        }                                                                                                                                         \
    }

static int counter = 0;
static bool pms5003_initialized = false;

static s_ezlopi_device_properties_t *PMS_PM1_properties = NULL;
static s_ezlopi_device_properties_t *PMS_PM2_5_properties = NULL;
static s_ezlopi_device_properties_t *PMS_PM10_properties = NULL;

static int sensor_0014_UART_PMS5003_prepare(void *arg);
static s_ezlopi_device_properties_t *sensor_0014_UART_PMS5003_prepare_properties(uint32_t device_id,
                                                                                 const char *category,
                                                                                 const char *subcategory,
                                                                                 const char *item_name,
                                                                                 const char *value_type,
                                                                                 cJSON *cjson_device,
                                                                                 PMS_sensor_structure_t *pms5003_sensor_structure);
static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_arg);
static int sensor_0014_UART_PMS5003_init(s_ezlopi_device_properties_t *properties);
static int sensor_0014_UART_PMS5003_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args);
static int sensor_0014_UART_PMS5003_check_new_data_available_and_update(s_ezlopi_device_properties_t *properties);

static bool sensor_0014_UART_PMS5003_check_config_gpio_json(cJSON *gpio_config_cjson);
static int sensor_0014_UART_PMS5003_set_config_gpio(cJSON *gpio_config_cjson);

int sensor_0014_UART_PMS5003(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        sensor_0014_UART_PMS5003_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        sensor_0014_UART_PMS5003_init(properties);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        // sensor_0014_UART_PMS5003_get_value_cjson(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        sensor_0014_UART_PMS5003_check_new_data_available_and_update(properties);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static int sensor_0014_UART_PMS5003_prepare(void *arg)
{
    int ret = 0;

    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if ((NULL != prep_arg))
    {
        if (!sensor_0014_UART_PMS5003_check_config_gpio_json(prep_arg->cjson_device))
        {
            PMS_sensor_structure_t *pms5003_sensor_structure = (PMS_sensor_structure_t *)malloc(sizeof(PMS_sensor_structure_t));
            PM25_AQI_Data *pms_data = (PM25_AQI_Data *)malloc(sizeof(PM25_AQI_Data));
            if ((NULL != pms5003_sensor_structure) && (NULL != pms_data))
            {
                memset(pms5003_sensor_structure, 0, sizeof(PMS_sensor_structure_t));
                memset(pms_data, 0, sizeof(PM25_AQI_Data));
                pms5003_sensor_structure->pms_data = pms_data;
                uint32_t device_id = ezlopi_cloud_generate_device_id();
                ADD_PROPERTIES_DEVICE_LIST(PMS_PM1_properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_particulate_matter_1, value_type_substance_amount, prep_arg->cjson_device, pms5003_sensor_structure);
                device_id = ezlopi_cloud_generate_device_id();
                ADD_PROPERTIES_DEVICE_LIST(PMS_PM2_5_properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_particulate_matter_2_dot_5, value_type_substance_amount, prep_arg->cjson_device, pms5003_sensor_structure);
                device_id = ezlopi_cloud_generate_device_id();
                ADD_PROPERTIES_DEVICE_LIST(PMS_PM10_properties, device_id, category_generic_sensor, subcategory_not_defined, ezlopi_item_name_particulate_matter_10, value_type_substance_amount, prep_arg->cjson_device, pms5003_sensor_structure);
            }
        }
        else
        {
            sensor_0014_UART_PMS5003_set_config_gpio(prep_arg->cjson_device);
        }
    }
    return ret;
}

static s_ezlopi_device_properties_t *sensor_0014_UART_PMS5003_prepare_properties(uint32_t device_id,
                                                                                 const char *category,
                                                                                 const char *subcategory,
                                                                                 const char *item_name,
                                                                                 const char *value_type,
                                                                                 cJSON *cjson_device,
                                                                                 PMS_sensor_structure_t *pms5003_sensor_structure)
{
    s_ezlopi_device_properties_t *pms5003_properties = (s_ezlopi_device_properties_t *)malloc(sizeof(s_ezlopi_device_properties_t));
    if (pms5003_properties)
    {
        memset(pms5003_properties, 0, sizeof(s_ezlopi_device_properties_t));
        pms5003_properties->interface_type = EZLOPI_DEVICE_INTERFACE_UART;

        char *device_name = NULL;

        if (ezlopi_item_name_particulate_matter_1 == item_name)
        {
            device_name = "PM 1 ";
        }
        else if (ezlopi_item_name_particulate_matter_2_dot_5 == item_name)
        {
            device_name = "PM 2.5 ";
        }
        else if (ezlopi_item_name_particulate_matter_10 == item_name)
        {
            device_name = "PM 10 ";
        }
        else
        {
            device_name = "Dust Particles";
        }

        ASSIGN_DEVICE_NAME(pms5003_properties, device_name);
        pms5003_properties->ezlopi_cloud.category = category;
        pms5003_properties->ezlopi_cloud.subcategory = subcategory;
        pms5003_properties->ezlopi_cloud.item_name = item_name;
        pms5003_properties->ezlopi_cloud.device_type = dev_type_sensor;
        pms5003_properties->ezlopi_cloud.value_type = value_type;
        pms5003_properties->ezlopi_cloud.has_getter = true;
        pms5003_properties->ezlopi_cloud.has_setter = false;
        pms5003_properties->ezlopi_cloud.reachable = true;
        pms5003_properties->ezlopi_cloud.battery_powered = false;
        pms5003_properties->ezlopi_cloud.show = true;
        pms5003_properties->ezlopi_cloud.room_name[0] = '\0';
        pms5003_properties->ezlopi_cloud.device_id = device_id;
        pms5003_properties->ezlopi_cloud.room_id = ezlopi_cloud_generate_room_id();
        pms5003_properties->ezlopi_cloud.item_id = ezlopi_cloud_generate_item_id();

        CJSON_GET_VALUE_INT(cjson_device, "baud_rate", pms5003_properties->interface.uart.baudrate);
        CJSON_GET_VALUE_INT(cjson_device, "gpio_tx", pms5003_properties->interface.uart.tx);
        CJSON_GET_VALUE_INT(cjson_device, "gpio_rx", pms5003_properties->interface.uart.rx);

        pms5003_properties->user_arg = pms5003_sensor_structure;
    }
    return pms5003_properties;
}

static int add_device_to_list(s_ezlopi_prep_arg_t *prep_arg, s_ezlopi_device_properties_t *properties, void *user_arg)
{
    int ret = 0;

    if (properties)
    {
        if (0 == ezlopi_devices_list_add(prep_arg->device, properties, user_arg))
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

static int sensor_0014_UART_PMS5003_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;

    PMS_sensor_structure_t *pms5003_sensor_structure = (PMS_sensor_structure_t *)PMS_PM1_properties->user_arg;

    if ((NULL != pms5003_sensor_structure) && (false == pms5003_initialized))
    {
        pms_uart_setup(pms5003_sensor_structure, properties->interface.uart.tx, properties->interface.uart.rx, properties->interface.uart.baudrate);
        ESP_ERROR_CHECK(pms_setup_control_gpio(pms5003_sensor_structure->pms_set_pin, pms5003_sensor_structure->pms_reset_pin));
        pms_startup(pms5003_sensor_structure->pms_set_pin, pms5003_sensor_structure->pms_reset_pin);
        pms_create_sleep_timer(pms5003_sensor_structure);
        pms5003_initialized = true;
    }

    return ret;
}

static int sensor_0014_UART_PMS5003_get_value_cjson(s_ezlopi_device_properties_t *properties, void *args)
{
    int ret = 0;
    // TRACE_B("HERE!!");
    cJSON *cjson_properties = (cJSON *)args;
    PMS_sensor_structure_t *pms5003_sensor_structure = (PMS_sensor_structure_t *)PMS_PM1_properties->user_arg;
    char formatted_value[20];

    if (cjson_properties && pms5003_sensor_structure->pms_data)
    {
        if (ezlopi_item_name_particulate_matter_1 == properties->ezlopi_cloud.item_name)
        {
            // pms5003_sensor_structure->pms_data->pm10_standard = 0.3205;
            snprintf(formatted_value, 20, "%.2f", pms5003_sensor_structure->pms_data->pm10_standard);
            TRACE_I("Dust particle 1 : %s", formatted_value);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", pms5003_sensor_structure->pms_data->pm10_standard);
            cJSON_AddStringToObject(cjson_properties, "scale", "micro_gram_per_cubic_meter");
        }
        if (ezlopi_item_name_particulate_matter_2_dot_5 == properties->ezlopi_cloud.item_name)
        {
            // pms5003_sensor_structure->pms_data->pm25_standard = 1.5360; // Dummy value
            snprintf(formatted_value, 20, "%.2f", pms5003_sensor_structure->pms_data->pm25_standard);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", pms5003_sensor_structure->pms_data->pm25_standard);
            cJSON_AddStringToObject(cjson_properties, "scale", "micro_gram_per_cubic_meter");
        }
        if (ezlopi_item_name_particulate_matter_10 == properties->ezlopi_cloud.item_name)
        {
            // pms5003_sensor_structure->pms_data->pm25_standard = 7.5360;  // Dummy value
            snprintf(formatted_value, 20, "%.2f", pms5003_sensor_structure->pms_data->pm100_standard);
            cJSON_AddStringToObject(cjson_properties, "valueFormatted", formatted_value);
            cJSON_AddNumberToObject(cjson_properties, "value", pms5003_sensor_structure->pms_data->pm100_standard);
            cJSON_AddStringToObject(cjson_properties, "scale", "micro_gram_per_cubic_meter");
        }
    }

    return ret;
}

static int sensor_0014_UART_PMS5003_check_new_data_available_and_update(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;

    PMS_sensor_structure_t *pms5003_sensor_structure = (PMS_sensor_structure_t *)properties->user_arg;
    // TRACE_B("is data available %d", pms_is_data_available(pms5003_sensor_structure->pms_data));
    if (true == pms_is_data_available(pms5003_sensor_structure->pms_data))
    {
        // pms_print_data(pms5003_sensor_structure->pms_data);
        pms_set_data_available_to_false(pms5003_sensor_structure->pms_data);
    }
    // if ((true == pms_is_data_available(pms5003_sensor_structure->pms_data)) && (counter == 0))
    // {
    //     TRACE_B("Data available!!");
    //     pms_set_data_available_to_false(pms5003_sensor_structure->pms_data);
    //     counter = 1;
    // }
    // if ((counter != 0) && (counter <= 3))
    // {
    //     PM25_AQI_Data *pms5003_data = (PM25_AQI_Data *)pms5003_sensor_structure->pms_data;
    //     pms_print_data(pms5003_data);
    //     ezlopi_device_value_updated_from_device(properties);
    //     counter++;
    // }
    // else if (counter > 3)
    // {
    //     counter = 0;
    // }
    return ret;
}

static bool sensor_0014_UART_PMS5003_check_config_gpio_json(cJSON *gpio_config_cjson)
{
    bool ret = false;
    uint32_t item_id = 0;
    CJSON_GET_VALUE_INT(gpio_config_cjson, "id_item", item_id);
    if (EZLOPI_SENSOR_0037_DIGITAL_OUTPUT_PMS5003_GPIO == item_id)
    {
        ret = true;
    }
    return ret;
}

static int sensor_0014_UART_PMS5003_set_config_gpio(cJSON *gpio_config_cjson)
{
    int ret = 0;

    uint32_t reset_gpio = 0;
    uint32_t set_gpio = 0;

    CJSON_GET_VALUE_INT(gpio_config_cjson, "gpio1", set_gpio);
    CJSON_GET_VALUE_INT(gpio_config_cjson, "gpio2", reset_gpio);

    PMS_sensor_structure_t *pms5003_sensor_structure = (PMS_sensor_structure_t *)PMS_PM1_properties->user_arg;
    pms5003_sensor_structure->pms_reset_pin = reset_gpio;
    pms5003_sensor_structure->pms_set_pin = set_gpio;

    return ret;
}
