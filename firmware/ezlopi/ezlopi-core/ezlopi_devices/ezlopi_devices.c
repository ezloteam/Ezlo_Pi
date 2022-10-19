#include "cJSON.h"

#include "trace.h"
#include "ezlopi_nvs.h"
#include "ezlopi_devices.h"

static void ezlopi_device_map_devices(cJSON *cjson_device);
static void ezlopi_device_parse_json(char *config_string);

#define CJSON_GET_VALUE_INT(root, item_name, item_val)        \
    {                                                         \
        cJSON *o_item = cJSON_GetObjectItem(root, item_name); \
        if (o_item)                                           \
        {                                                     \
            item_val = o_item->valueint;                      \
        }                                                     \
    }

#define CJSON_GET_VALUE_STRING(root, item_name, item_val)     \
    {                                                         \
        cJSON *o_item = cJSON_GetObjectItem(root, item_name); \
        if (o_item)                                           \
        {                                                     \
            item_val = o_item->valuestring;                   \
        }                                                     \
    }

void ezlopi_device_init(void)
{
    char *config_string = NULL;
    ezlopi_nvs_read_config_data_str(&config_string);
    TRACE_D("config_string: %s", config_string ? config_string : "");

    if (config_string)
    {
        ezlopi_device_parse_json(config_string);
        free(config_string);
    }
}

static void ezlopi_device_map_devices(cJSON *cjson_device)
{
    e_ezlopi_device_interface_type_t device_type = EZLOPI_DEVICE_INTERFACE_NONE;
    CJSON_GET_VALUE_INT(cjson_device, "dev_type", device_type);

    switch (device_type)
    {
    case EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT:
    {
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT:
    {
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT:
    {
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_ANALOG_OUTPUT:
    {
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_PWM:
    {
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_UART:
    {
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_ONEWIRE_MASTER:
    {
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_I2C_MASTER:
    {
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_SPI_MASTER:
    {
        break;
    }
    default:
    {
        break;
    }
    }
}

static void ezlopi_device_parse_json(char *config_string)
{
    cJSON *cjson_config = cJSON_Parse(config_string);

    if (cjson_config)
    {
        cJSON *cjson_device_list = cJSON_GetObjectItem(cjson_config, "dev_detail");
        if (cjson_device_list)
        {
            int dev_idx = 0;
            cJSON *cjson_device = NULL;

            TRACE_B("---------------------------------------------");
            while (NULL != (cjson_device = cJSON_GetArrayItem(cjson_device_list, dev_idx)))
            {
                TRACE_B("Device-%d - %d:", dev_idx, (uint32_t)cjson_device);

                cJSON *o_dev_name = NULL;
                if (NULL != (o_dev_name = cJSON_GetObjectItem(cjson_device, "dev_name")))
                {
                    TRACE_D("dev_name: %s", o_dev_name->valuestring);
                }

                ezlopi_device_map_devices(cjson_device);

                dev_idx++;
                TRACE_B("---------------------------------------------");
            }
        }

        cJSON_Delete(cjson_config);
    }
}

static void add_item_digital_out(cJSON *o_device, uint32_t dev_idx)
{
    char *dev_name = NULL;
    int id_room = 0, gpio_in = 0, gpio_out = 0, id_item = 0, ip_inv = 0;
    int is_ip = 0, op_inv = 0, pullup_ip = 0, pullup_op = 0, val_ip = 0, val_op = 0;

    CJSON_GET_VALUE_STRING(o_device, "dev_name", dev_name);
    CJSON_GET_VALUE_INT(o_device, "id_room", id_room);
    CJSON_GET_VALUE_INT(o_device, "gpio_in", gpio_in);
    CJSON_GET_VALUE_INT(o_device, "gpio_out", gpio_out);
    CJSON_GET_VALUE_INT(o_device, "id_item", id_item);
    CJSON_GET_VALUE_INT(o_device, "ip_inv", ip_inv);
    CJSON_GET_VALUE_INT(o_device, "is_ip", is_ip);
    CJSON_GET_VALUE_INT(o_device, "op_inv", op_inv);
    CJSON_GET_VALUE_INT(o_device, "pullup_ip", pullup_ip);
    CJSON_GET_VALUE_INT(o_device, "pullup_op", pullup_op);
    CJSON_GET_VALUE_INT(o_device, "val_ip", val_ip);
    CJSON_GET_VALUE_INT(o_device, "val_op", val_op);

    if (dev_name)
    {
        snprintf(g_devices[dev_idx].name, sizeof(g_devices[dev_idx].name), "%s", dev_name);
    }
    else
    {
        snprintf(g_devices[dev_idx].name, sizeof(g_devices[dev_idx].name), "dev-%d:digital_out", dev_idx);
    }

    g_devices[dev_idx].dev_type = EZPI_DEV_TYPE_DIGITAL_OP;
    snprintf(g_devices[dev_idx].device_id, sizeof(g_devices[dev_idx].device_id), "de100%.3d", dev_idx);
    snprintf(g_devices[dev_idx].item_id, sizeof(g_devices[dev_idx].item_id), "ab100%.3d", dev_idx);
    snprintf(g_devices[dev_idx].roomId, sizeof(g_devices[dev_idx].roomId), "abcd%d", id_room);
    snprintf(g_devices[dev_idx].roomName, sizeof(g_devices[dev_idx].roomName), "room-%d", id_room);

    snprintf(g_devices[dev_idx].category, sizeof(g_devices[dev_idx].category), "switch");
    snprintf(g_devices[dev_idx].subcategory, sizeof(g_devices[dev_idx].subcategory), "in_wall");
    snprintf(g_devices[dev_idx].item_name, sizeof(g_devices[dev_idx].item_name), "switch"); // defined here: https://api.ezlo.com/devices/items/index.html
    snprintf(g_devices[dev_idx].devicType, sizeof(g_devices[dev_idx].devicType), "switch.inwall");
    snprintf(g_devices[dev_idx].value_type, sizeof(g_devices[dev_idx].value_type), "bool");

    g_devices[dev_idx].input_gpio = gpio_in;
    g_devices[dev_idx].input_inv = ip_inv;
    g_devices[dev_idx].input_vol = val_ip;
    g_devices[dev_idx].is_input = is_ip;
    g_devices[dev_idx].is_meter = 0;
    g_devices[dev_idx].out_gpio = gpio_out;
    g_devices[dev_idx].out_inv = op_inv;
    g_devices[dev_idx].out_vol = val_op;
    g_devices[dev_idx].input_pullup = pullup_ip;
    g_devices[dev_idx].output_pullup = pullup_op;
    g_devices[dev_idx].has_getter = true;
    g_devices[dev_idx].has_setter = true;
}

static void add_item_digital_in(cJSON *o_device, uint32_t dev_idx)
{
    char *dev_name = NULL;
    int id_room = 0, gpio_in = 0, id_item = 0, ip_inv = 0, pullup_ip = 0, val_ip = 0;

    CJSON_GET_VALUE_STRING(o_device, "dev_name", dev_name);
    CJSON_GET_VALUE_INT(o_device, "id_room", id_room);
    CJSON_GET_VALUE_INT(o_device, "gpio", gpio_in);
    CJSON_GET_VALUE_INT(o_device, "id_item", id_item);
    CJSON_GET_VALUE_INT(o_device, "logic_inv", ip_inv);
    CJSON_GET_VALUE_INT(o_device, "pull_up", pullup_ip);
    CJSON_GET_VALUE_INT(o_device, "val_ip", val_ip);

    if (dev_name)
    {
        snprintf(g_devices[dev_idx].name, sizeof(g_devices[dev_idx].name), "%s", dev_name);
    }
    else
    {
        snprintf(g_devices[dev_idx].name, sizeof(g_devices[dev_idx].name), "dev-%d:digital_out", dev_idx);
    }

    g_devices[dev_idx].dev_type = EZPI_DEV_TYPE_DIGITAL_IP;
    snprintf(g_devices[dev_idx].device_id, sizeof(g_devices[dev_idx].device_id), "de100%.3d", dev_idx);
    snprintf(g_devices[dev_idx].item_id, sizeof(g_devices[dev_idx].item_id), "ab100%.3d", dev_idx);
    snprintf(g_devices[dev_idx].roomId, sizeof(g_devices[dev_idx].roomId), "abcd%d", id_room);
    snprintf(g_devices[dev_idx].roomName, sizeof(g_devices[dev_idx].roomName), "room-%d", id_room);

    snprintf(g_devices[dev_idx].category, sizeof(g_devices[dev_idx].category), "switch");
    snprintf(g_devices[dev_idx].subcategory, sizeof(g_devices[dev_idx].subcategory), "in_wall");
    snprintf(g_devices[dev_idx].item_name, sizeof(g_devices[dev_idx].item_name), "switch"); // defined here: https://api.ezlo.com/devices/items/index.html
    snprintf(g_devices[dev_idx].devicType, sizeof(g_devices[dev_idx].devicType), "switch.inwall");
    snprintf(g_devices[dev_idx].value_type, sizeof(g_devices[dev_idx].value_type), "bool");

    g_devices[dev_idx].input_gpio = gpio_in;
    g_devices[dev_idx].input_inv = ip_inv;
    g_devices[dev_idx].input_vol = val_ip;
    g_devices[dev_idx].is_input = true;
    g_devices[dev_idx].is_meter = 0;
    g_devices[dev_idx].input_pullup = pullup_ip;
    g_devices[dev_idx].has_getter = true;
    g_devices[dev_idx].has_setter = false;
}

static void add_item_analogue_in(cJSON *o_device, uint32_t dev_idx)
{
}

static void add_item_analogue_out(cJSON *o_device, uint32_t dev_idx)
{
}

static void add_item_pwm(cJSON *o_device, uint32_t dev_idx)
{
}

static void add_item_uart(cJSON *o_device, uint32_t dev_idx)
{
}

static void add_other(cJSON *o_device, uint32_t dev_idx)
{
    char *dev_name = NULL;
    int id_item = 0;
    int id_room = 0;

    CJSON_GET_VALUE_STRING(o_device, "dev_name", dev_name);
    CJSON_GET_VALUE_INT(o_device, "id_item", id_item);
    CJSON_GET_VALUE_INT(o_device, "id_room", id_room);

    memset(&g_devices[dev_idx], 0, sizeof(s_device_properties_t));
    g_devices[dev_idx].dev_type = EZPI_DEV_TYPE_OTHER;

    if (dev_name)
    {
        snprintf(g_devices[dev_idx].name, sizeof(g_devices[dev_idx].name), "%s", dev_name);
    }
    else
    {
        snprintf(g_devices[dev_idx].name, sizeof(g_devices[dev_idx].name), "dev-%d:other", dev_idx);
    }

    snprintf(g_devices[dev_idx].device_id, sizeof(g_devices[dev_idx].device_id), "de100%.3d", dev_idx);
    snprintf(g_devices[dev_idx].item_id, sizeof(g_devices[dev_idx].item_id), "ab100%.3d", dev_idx);
    snprintf(g_devices[dev_idx].roomId, sizeof(g_devices[dev_idx].roomId), "abcd%.3d", dev_idx);
    snprintf(g_devices[dev_idx].roomName, sizeof(g_devices[dev_idx].roomName), "room-%.3d", dev_idx);

    snprintf(g_devices[dev_idx].category, sizeof(g_devices[dev_idx].category), "security_sensor");
    snprintf(g_devices[dev_idx].subcategory, sizeof(g_devices[dev_idx].subcategory), "door");
    snprintf(g_devices[dev_idx].item_name, sizeof(g_devices[dev_idx].item_name), "dw_state");
    snprintf(g_devices[dev_idx].devicType, sizeof(g_devices[dev_idx].devicType), "doorlock");
    snprintf(g_devices[dev_idx].value_type, sizeof(g_devices[dev_idx].value_type), "token");
    g_devices[dev_idx].has_getter = true;
    g_devices[dev_idx].has_setter = false;

    hall_sensor_service_init(dev_idx);
}

static void add_item_onewire(cJSON *o_device, uint32_t dev_idx)
{
    char *dev_name = NULL;
    int gpio = 0;
    int id_item = 0;
    int id_room = 0;
    int pullup = 0;
    int val_ip = 0;

    CJSON_GET_VALUE_STRING(o_device, "dev_name", dev_name);
    CJSON_GET_VALUE_INT(o_device, "gpio", gpio);
    CJSON_GET_VALUE_INT(o_device, "id_item", id_item);
    CJSON_GET_VALUE_INT(o_device, "id_room", id_room);
    CJSON_GET_VALUE_INT(o_device, "pull_up", pullup);
    CJSON_GET_VALUE_INT(o_device, "val_ip", val_ip);

    memset(&g_devices[dev_idx], 0, sizeof(s_device_properties_t));

    g_devices[dev_idx].dev_type = EZPI_DEV_TYPE_ONE_WIRE;

    if (dev_name)
    {
        snprintf(g_devices[dev_idx].name, sizeof(g_devices[dev_idx].name), "%s", dev_name);
    }
    else
    {
        snprintf(g_devices[dev_idx].name, sizeof(g_devices[dev_idx].name), "dev-%d:digital_out", dev_idx);
    }

    snprintf(g_devices[dev_idx].device_id, sizeof(g_devices[dev_idx].device_id), "de100%.3d", dev_idx);
    snprintf(g_devices[dev_idx].item_id, sizeof(g_devices[dev_idx].item_id), "ab100%.3d", dev_idx);
    snprintf(g_devices[dev_idx].roomId, sizeof(g_devices[dev_idx].roomId), "abcd%d", id_room);
    snprintf(g_devices[dev_idx].roomName, sizeof(g_devices[dev_idx].roomName), "room-%d", id_room);

    snprintf(g_devices[dev_idx].category, sizeof(g_devices[dev_idx].category), "temperature");
    g_devices[dev_idx].subcategory[0] = '\0';
    snprintf(g_devices[dev_idx].item_name, sizeof(g_devices[dev_idx].item_name), "temp"); // defined here: https://api.ezlo.com/devices/items/index.html
    snprintf(g_devices[dev_idx].devicType, sizeof(g_devices[dev_idx].devicType), "sensor");
    snprintf(g_devices[dev_idx].value_type, sizeof(g_devices[dev_idx].value_type), "temperature");

    g_devices[dev_idx].has_getter = true;
    g_devices[dev_idx].has_setter = false;

    dht11_service_init(gpio, dev_idx);
}

static void add_item_i2c(cJSON *o_device, uint32_t dev_idx)
{
    char *dev_name = NULL;
    int id_item = 0;
    int id_room = 0;
    int gpio_scl = 0;
    int gpio_sda = 0;
    int pullup_scl = 0;
    int pullup_sda = 0;
    int slave_addr = 0;

    CJSON_GET_VALUE_STRING(o_device, "dev_name", dev_name);
    CJSON_GET_VALUE_INT(o_device, "id_item", id_item);
    CJSON_GET_VALUE_INT(o_device, "id_room", id_room);
    CJSON_GET_VALUE_INT(o_device, "gpio_scl", gpio_scl);
    CJSON_GET_VALUE_INT(o_device, "gpio_sda", gpio_sda);
    CJSON_GET_VALUE_INT(o_device, "pullup_scl", pullup_scl);
    CJSON_GET_VALUE_INT(o_device, "pullup_sda", pullup_sda);
    CJSON_GET_VALUE_INT(o_device, "slave_addr", slave_addr);

    memset(&g_devices[dev_idx], 0, sizeof(s_device_properties_t));

    g_devices[dev_idx].dev_type = EZPI_DEV_TYPE_I2C;

    if (dev_name)
    {
        snprintf(g_devices[dev_idx].name, sizeof(g_devices[dev_idx].name), "%s", dev_name);
    }
    else
    {
        snprintf(g_devices[dev_idx].name, sizeof(g_devices[dev_idx].name), "dev-%d:digital_out", dev_idx);
    }

    snprintf(g_devices[dev_idx].device_id, sizeof(g_devices[dev_idx].device_id), "de100%.3d", dev_idx);
    snprintf(g_devices[dev_idx].item_id, sizeof(g_devices[dev_idx].item_id), "ab100%.3d", dev_idx);
    snprintf(g_devices[dev_idx].roomId, sizeof(g_devices[dev_idx].roomId), "abcd%.3d", dev_idx);
    snprintf(g_devices[dev_idx].roomName, sizeof(g_devices[dev_idx].roomName), "room-%.3d", dev_idx);

    snprintf(g_devices[dev_idx].category, sizeof(g_devices[dev_idx].category), "level_sensor");
    // g_devices[dev_idx].subcategory[0] = '\0';
    snprintf(g_devices[dev_idx].subcategory, sizeof(g_devices[dev_idx].subcategory), "navigation");
    snprintf(g_devices[dev_idx].item_name, sizeof(g_devices[dev_idx].item_name), "acceleration_x_axis");
    snprintf(g_devices[dev_idx].devicType, sizeof(g_devices[dev_idx].devicType), "sensor");
    snprintf(g_devices[dev_idx].value_type, sizeof(g_devices[dev_idx].value_type), "value");
    g_devices[dev_idx].has_getter = true;
    g_devices[dev_idx].has_setter = false;

    mpu_service_init(gpio_scl, gpio_sda, dev_idx);
}

static void add_item_spi(cJSON *o_device, uint32_t dev_idx)
{
}
