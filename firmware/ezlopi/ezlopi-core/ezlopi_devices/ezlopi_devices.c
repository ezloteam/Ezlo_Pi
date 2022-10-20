#include "cJSON.h"

#include "trace.h"
#include "ezlopi_nvs.h"
#include "ezlopi_devices.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_sensors.h"

static void ezlopi_device_map_devices(cJSON *cjson_device);
static void ezlopi_device_parse_json(char *config_string);

#if 0
static void add_item_digital_out(cJSON *cjson_device, uint32_t dev_idx);
static void add_item_digital_in(cJSON *cjson_device, uint32_t dev_idx);
static void add_item_analogue_in(cJSON *cjson_device, uint32_t dev_idx);
static void add_item_analogue_out(cJSON *cjson_device, uint32_t dev_idx);
static void add_item_pwm(cJSON *cjson_device, uint32_t dev_idx);
static void add_item_uart(cJSON *cjson_device, uint32_t dev_idx);
static void add_item_other(cJSON *cjson_device, uint32_t dev_idx);
static void add_item_onewire(cJSON *cjson_device, uint32_t dev_idx);
static void add_item_i2c(cJSON *cjson_device, uint32_t dev_idx);
static void add_item_spi(cJSON *cjson_device, uint32_t dev_idx);

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

#define ASSIGN_DEVICE_NAME()                                                                                                                            \
    {                                                                                                                                                   \
        char *device_name = NULL;                                                                                                                       \
        CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);                                                                                  \
        if ((NULL != device_name) && ('\0' != device_name[0]))                                                                                          \
        {                                                                                                                                               \
            snprintf(device->ezlopi_cloud.device_name, sizeof(device->ezlopi_cloud.device_name), "%s", device_name);                                    \
        }                                                                                                                                               \
        else                                                                                                                                            \
        {                                                                                                                                               \
            snprintf(device->ezlopi_cloud.device_name, sizeof(device->ezlopi_cloud.device_name), "dev-%d:digital_out", device->ezlopi_cloud.device_id); \
        }                                                                                                                                               \
    }
#endif

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

#if 0
static void add_item_digital_out(cJSON *cjson_device, uint32_t dev_idx)
{
    s_ezlopi_devices_t *device = malloc(sizeof(s_ezlopi_devices_t));

    if (device)
    {
        int tmp_var = 0;
        device->interface_type = EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT;

        device->ezlopi_cloud.device_id = ezlopi_device_generate_device_id();
        CJSON_GET_VALUE_INT(cjson_device, "id_room", device->ezlopi_cloud.room_id);
        CJSON_GET_VALUE_INT(cjson_device, "id_item", device->ezlopi_cloud.item_id);
        ASSIGN_DEVICE_NAME()
        device->ezlopi_cloud.has_getter = true;
        device->ezlopi_cloud.has_setter = true;

        CJSON_GET_VALUE_INT(cjson_device, "is_ip", device->interface.gpio.gpio_in.enable);
        CJSON_GET_VALUE_INT(cjson_device, "gpio_in", device->interface.gpio.gpio_in.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, "ip_inv", device->interface.gpio.gpio_in.invert);
        CJSON_GET_VALUE_INT(cjson_device, "val_ip", device->interface.gpio.gpio_in.value);
        CJSON_GET_VALUE_INT(cjson_device, "pullup_ip", tmp_var);
        device->interface.gpio.gpio_in.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;

        device->interface.gpio.gpio_out.enable = true;
        CJSON_GET_VALUE_INT(cjson_device, "gpio_out", device->interface.gpio.gpio_out.gpio_num);
        CJSON_GET_VALUE_INT(cjson_device, "op_inv", device->interface.gpio.gpio_out.invert);
        CJSON_GET_VALUE_INT(cjson_device, "val_op", device->interface.gpio.gpio_out.value);
        CJSON_GET_VALUE_INT(cjson_device, "pullup_op", tmp_var);
        device->interface.gpio.gpio_out.pull = tmp_var ? GPIO_PULLUP_ONLY : GPIO_PULLDOWN_ONLY;
    }

#if 0
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
#endif
}

static void add_item_digital_in(cJSON *cjson_device, uint32_t dev_idx)
{
    char *dev_name = NULL;
    int id_room = 0, gpio_in = 0, id_item = 0, ip_inv = 0, pullup_ip = 0, val_ip = 0;

    CJSON_GET_VALUE_STRING(cjson_device, "dev_name", dev_name);
    CJSON_GET_VALUE_INT(cjson_device, "id_room", id_room);
    CJSON_GET_VALUE_INT(cjson_device, "gpio", gpio_in);
    CJSON_GET_VALUE_INT(cjson_device, "id_item", id_item);
    CJSON_GET_VALUE_INT(cjson_device, "logic_inv", ip_inv);
    CJSON_GET_VALUE_INT(cjson_device, "pull_up", pullup_ip);
    CJSON_GET_VALUE_INT(cjson_device, "val_ip", val_ip);

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

static void add_item_analogue_in(cJSON *cjson_device, uint32_t dev_idx)
{
}

static void add_item_analogue_out(cJSON *cjson_device, uint32_t dev_idx)
{
}

static void add_item_pwm(cJSON *cjson_device, uint32_t dev_idx)
{
}

static void add_item_uart(cJSON *cjson_device, uint32_t dev_idx)
{
}

static void add_item_other(cJSON *cjson_device, uint32_t dev_idx)
{
    char *dev_name = NULL;
    int id_item = 0;
    int id_room = 0;

    CJSON_GET_VALUE_STRING(cjson_device, "dev_name", dev_name);
    CJSON_GET_VALUE_INT(cjson_device, "id_item", id_item);
    CJSON_GET_VALUE_INT(cjson_device, "id_room", id_room);

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

static void add_item_onewire(cJSON *cjson_device, uint32_t dev_idx)
{
    char *dev_name = NULL;
    int gpio = 0;
    int id_item = 0;
    int id_room = 0;
    int pullup = 0;
    int val_ip = 0;

    CJSON_GET_VALUE_STRING(cjson_device, "dev_name", dev_name);
    CJSON_GET_VALUE_INT(cjson_device, "gpio", gpio);
    CJSON_GET_VALUE_INT(cjson_device, "id_item", id_item);
    CJSON_GET_VALUE_INT(cjson_device, "id_room", id_room);
    CJSON_GET_VALUE_INT(cjson_device, "pull_up", pullup);
    CJSON_GET_VALUE_INT(cjson_device, "val_ip", val_ip);

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

static void add_item_i2c(cJSON *cjson_device, uint32_t dev_idx)
{
    char *dev_name = NULL;
    int id_item = 0;
    int id_room = 0;
    int gpio_scl = 0;
    int gpio_sda = 0;
    int pullup_scl = 0;
    int pullup_sda = 0;
    int slave_addr = 0;

    CJSON_GET_VALUE_STRING(cjson_device, "dev_name", dev_name);
    CJSON_GET_VALUE_INT(cjson_device, "id_item", id_item);
    CJSON_GET_VALUE_INT(cjson_device, "id_room", id_room);
    CJSON_GET_VALUE_INT(cjson_device, "gpio_scl", gpio_scl);
    CJSON_GET_VALUE_INT(cjson_device, "gpio_sda", gpio_sda);
    CJSON_GET_VALUE_INT(cjson_device, "pullup_scl", pullup_scl);
    CJSON_GET_VALUE_INT(cjson_device, "pullup_sda", pullup_sda);
    CJSON_GET_VALUE_INT(cjson_device, "slave_addr", slave_addr);

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

static void add_item_spi(cJSON *cjson_device, uint32_t dev_idx)
{
}
#endif

static void ezlopi_device_parse_json(char *config_string)
{
    cJSON *cjson_config = cJSON_Parse(config_string);

    if (cjson_config)
    {
        cJSON *cjson_device_list = cJSON_GetObjectItem(cjson_config, "dev_detail");
        if (cjson_device_list)
        {
            int config_dev_idx = 0;
            cJSON *cjson_device = NULL;

            TRACE_B("---------------------------------------------");
            while (NULL != (cjson_device = cJSON_GetArrayItem(cjson_device_list, config_dev_idx)))
            {
                TRACE_B("Device-%d - %d:", config_dev_idx, (uint32_t)cjson_device);

                char *device_name = NULL;
                CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);

                if (NULL != device_name)
                {
                    TRACE_D("device name: %s", device_name);
                }

                int id_item = 0;
                CJSON_GET_VALUE_INT(cjson_device, "id_item", id_item);

                if (0 != id_item)
                {
                    const s_ezlopi_sensor_t *sensor_list = ezlopi_sensor_get_list();

                    int dev_idx = 0;
                    while (NULL != sensor_list[dev_idx].func)
                    {
                        sensor_list[dev_idx].func(EZLOPI_ACTION_PREPARE, (void *)cjson_device);
                        dev_idx++;
                    }
                }

                // ezlopi_device_map_devices(cjson_device);

                config_dev_idx++;
                TRACE_B("---------------------------------------------");
            }
        }

        cJSON_Delete(cjson_config);
    }
}

uint16_t ezlopi_device_generate_device_id(void)
{
    static uint16_t device_id;
    device_id = (0 == device_id) ? 0xd001 : device_id + 1;
    return device_id;
}

uint16_t ezlopi_device_generate_item_id(void)
{
    static uint16_t item_id;
    item_id = (0 == item_id) ? 0x1001 : item_id + 1;
    return item_id;
}

uint16_t ezlopi_device_generate_room_id(void)
{
    static uint16_t room_id;
    room_id = (0 == room_id) ? 0x4001 : room_id + 1;
    return room_id;
}