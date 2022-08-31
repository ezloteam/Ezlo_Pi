#include "string.h"

#include "esp_err.h"
#include "cJSON.h"

#include "devices_common.h"
#include "nvs_storage.h"
#include "debug.h"

static uint32_t device_count = 0;
static s_device_properties_t g_devices[MAX_DEV];

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

extern void mpu_service_init(uint8_t scl_pin, uint8_t sda_pin, uint32_t dev_idx);
extern void dht11_service_init(uint32_t dht_pin, uint32_t dev_idx);
static void add_device_to_list(cJSON *j_devices, uint32_t dev_idx);
static void reset_device_list_gpio_pins(void);

void devices_common_init_devices(void)
{
    char *nvs_json_conf = NULL;
    nvs_storage_read_config_data_str(&nvs_json_conf);
    memset(g_devices, 0, sizeof(s_device_properties_t) * MAX_DEV);
    reset_device_list_gpio_pins();

    if (nvs_json_conf)
    {
        TRACE_B("Config read from nvs:");
        TRACE_B(nvs_json_conf);

        cJSON *root = cJSON_Parse(nvs_json_conf);

        if (root)
        {
            cJSON *o_device_list = cJSON_GetObjectItem(root, "dev_detail");
            if (o_device_list)
            {
                int dev_idx = 0;
                cJSON *o_device = NULL;

                TRACE_B("---------------------------------------------");
                while (NULL != (o_device = cJSON_GetArrayItem(o_device_list, dev_idx)))
                {
                    TRACE_B("Device-%d - %d:", dev_idx, (uint32_t)o_device);

                    cJSON *o_dev_name = NULL;
                    if (NULL != (o_dev_name = cJSON_GetObjectItem(o_device, "dev_name")))
                    {
                        TRACE_D("dev_name: %s", o_dev_name->valuestring);
                    }

                    add_device_to_list(o_device, dev_idx);

                    dev_idx++;
                    TRACE_B("---------------------------------------------");
                }
            }

            cJSON_Delete(root);
        }
    }

    if (nvs_json_conf)
    {
        free(nvs_json_conf);
    }
}

static void reset_device_list_gpio_pins(void)
{
    for (int idx = 0; idx < MAX_DEV; idx++)
    {
        g_devices[idx].input_gpio = 0xff;
        g_devices[idx].out_gpio = 0xff;
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

static void add_device_to_list(cJSON *o_device, uint32_t dev_idx)
{
    int id_item, id_room, dev_type = 0, gpio_out, gpio_in = 0;
    CJSON_GET_VALUE_INT(o_device, "dev_type", dev_type);
    TRACE_D("--------------dev_type: %u---------------", dev_type);
    TRACE_D("-----------------------------------------", dev_type);

    switch ((e_dev_type_t)dev_type)
    {
    case EZPI_DEV_TYPE_DIGITAL_OP:
    {
        TRACE_D("device type: EZPI_DEV_TYPE_DIGITAL_OP");
        add_item_digital_out(o_device, dev_idx);
        break;
    }
    case EZPI_DEV_TYPE_DIGITAL_IP:
    {
        TRACE_D("device type: EZPI_DEV_TYPE_DIGITAL_IP");
        add_item_digital_in(o_device, dev_idx);
        break;
    }
    case EZPI_DEV_TYPE_ANALOG_IP:
    {
        TRACE_D("device type: EZPI_DEV_TYPE_ANALOG_IP");
        add_item_analogue_in(o_device, dev_idx);
        break;
    }
    case EZPI_DEV_TYPE_ANALOG_OP:
    {
        TRACE_D("device type: EZPI_DEV_TYPE_ANALOG_OP");
        add_item_analogue_out(o_device, dev_idx);
        break;
    }
    case EZPI_DEV_TYPE_PWM:
    {
        TRACE_D("device type: EZPI_DEV_TYPE_PWM");
        add_item_pwm(o_device, dev_idx);
        break;
    }
    case EZPI_DEV_TYPE_UART:
    {
        TRACE_D("device type: EZPI_DEV_TYPE_UART");
        add_item_uart(o_device, dev_idx);
        break;
    }
    case EZPI_DEV_TYPE_ONE_WIRE:
    {
        TRACE_D("device type: EZPI_DEV_TYPE_ONE_WIRE");
        add_item_onewire(o_device, dev_idx);
        break;
    }
    case EZPI_DEV_TYPE_I2C:
    {
        TRACE_D("device type: EZPI_DEV_TYPE_I2C");
        add_item_i2c(o_device, dev_idx);
        break;
    }
    case EZPI_DEV_TYPE_SPI:
    {
        TRACE_D("device type: EZPI_DEV_TYPE_SPI");
        add_item_spi(o_device, dev_idx);
        break;
    }
    default:
    {
        TRACE_W("Un-recognized device type");
    }
    }

    TRACE_D("device_id: %s", g_devices[dev_idx].device_id);
    TRACE_D("name: %s", g_devices[dev_idx].name);
    TRACE_D("roomId: %s", g_devices[dev_idx].roomId);
    TRACE_D("roomName: %s", g_devices[dev_idx].roomName);
    TRACE_D("item_id: %s", g_devices[dev_idx].item_id);
    TRACE_D("item_name: %s", g_devices[dev_idx].item_name);
    TRACE_D("input_vol: %u", g_devices[dev_idx].input_vol);
    TRACE_D("out_vol: %u", g_devices[dev_idx].out_vol);
    TRACE_D("input_gpio: %u", g_devices[dev_idx].input_gpio);
    TRACE_D("out_gpio: %u", g_devices[dev_idx].out_gpio);
    TRACE_D("input_pullup: %u", g_devices[dev_idx].input_pullup);
    TRACE_D("output_pullup: %u", g_devices[dev_idx].output_pullup);
    TRACE_D("is_input: %u", g_devices[dev_idx].is_input);
    TRACE_D("input_inv: %u", g_devices[dev_idx].input_inv);
    TRACE_D("out_inv: %u", g_devices[dev_idx].out_inv);
    TRACE_D("is_meter: %u", g_devices[dev_idx].is_meter);
    TRACE_D("category: %s", g_devices[dev_idx].category);
    TRACE_D("subcategory: %s", g_devices[dev_idx].subcategory);
    TRACE_D("devicType: %s", g_devices[dev_idx].devicType);
    TRACE_D("value_type: %s", g_devices[dev_idx].value_type);
    TRACE_D("has_getter: %u", g_devices[dev_idx].has_getter);
    TRACE_D("has_setter: %u", g_devices[dev_idx].has_setter);
    TRACE_D("-----------------------------------------", dev_type);
}

const char *devices_common_get_device_type_id(void)
{
    return DEVICE_TYPE_ID;
}

s_device_properties_t *devices_common_device_list(void)
{
    return g_devices;
}

int devices_common_get_device_by_item_id(const char *item_id)
{
    for (int idx = 0; idx < MAX_DEV; idx++)
    {
        if (g_devices[idx].name[0] && item_id[0])
        {
            TRACE_D("Checking: %.*s | %s", sizeof(g_devices[idx].item_id), g_devices[idx].item_id, item_id);
            if (0 == strncmp(g_devices[idx].item_id, item_id, sizeof(g_devices[idx].item_id)))
            {
                return idx;
            }
        }
        else
        {
            TRACE_B("Found device");
            break;
        }
    }

    return 0xFFFF;
}
