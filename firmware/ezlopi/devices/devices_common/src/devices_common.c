#include "string.h"

#include "esp_err.h"
#include "cJSON.h"

#include "devices_common.h"
#include "nvs_storage.h"
#include "debug.h"

static uint32_t device_count = 0;
static s_device_properties_t devices[MAX_DEV];

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
static void add_device_to_list(cJSON *devices, uint32_t dev_idx);

void devices_common_init_devices(void)
{
    char *nvs_json_conf = NULL;
    nvs_storage_read_config_data_str(&nvs_json_conf);
    memset(devices, 0, sizeof(s_device_properties_t) * MAX_DEV);

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

    memset(&devices[dev_idx], 0, sizeof(s_device_properties_t));

    if (dev_name)
    {
        snprintf(devices[dev_idx].name, sizeof(devices[dev_idx].name), "%s", dev_name);
    }
    else
    {
        snprintf(devices[dev_idx].name, sizeof(devices[dev_idx].name), "dev-%d:digital_out", dev_idx);
    }

    devices[dev_idx].dev_type = EZPI_DEV_TYPE_DIGITAL_OP;
    snprintf(devices[dev_idx].device_id, sizeof(devices[dev_idx].device_id), "de100%.3d", dev_idx);
    snprintf(devices[dev_idx].item_id, sizeof(devices[dev_idx].item_id), "ab100%.3d", dev_idx);
    snprintf(devices[dev_idx].roomId, sizeof(devices[dev_idx].roomId), "abcd%d", id_room);
    snprintf(devices[dev_idx].roomName, sizeof(devices[dev_idx].roomName), "room-%d", id_room);

    snprintf(devices[dev_idx].category, sizeof(devices[dev_idx].category), "switch");
    snprintf(devices[dev_idx].subcategory, sizeof(devices[dev_idx].subcategory), "in_wall");
    snprintf(devices[dev_idx].item_name, sizeof(devices[dev_idx].item_name), "switch"); // defined here: https://api.ezlo.com/devices/items/index.html
    snprintf(devices[dev_idx].devicType, sizeof(devices[dev_idx].devicType), "switch.inwall");
    snprintf(devices[dev_idx].value_type, sizeof(devices[dev_idx].value_type), "bool");

    devices[dev_idx].input_gpio = gpio_in;
    devices[dev_idx].input_inv = ip_inv;
    devices[dev_idx].input_vol = val_ip;
    devices[dev_idx].is_input = is_ip;
    devices[dev_idx].is_meter = 0;
    devices[dev_idx].out_gpio = gpio_out;
    devices[dev_idx].out_inv = op_inv;
    devices[dev_idx].out_vol = val_op;
    devices[dev_idx].input_pullup = pullup_ip;
    devices[dev_idx].output_pullup = pullup_op;
    devices[dev_idx].has_getter = true;
    devices[dev_idx].has_setter = true;
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
        snprintf(devices[dev_idx].name, sizeof(devices[dev_idx].name), "%s", dev_name);
    }
    else
    {
        snprintf(devices[dev_idx].name, sizeof(devices[dev_idx].name), "dev-%d:digital_out", dev_idx);
    }

    devices[dev_idx].dev_type = EZPI_DEV_TYPE_DIGITAL_IP;
    snprintf(devices[dev_idx].device_id, sizeof(devices[dev_idx].device_id), "de100%.3d", dev_idx);
    snprintf(devices[dev_idx].item_id, sizeof(devices[dev_idx].item_id), "ab100%.3d", dev_idx);
    snprintf(devices[dev_idx].roomId, sizeof(devices[dev_idx].roomId), "abcd%d", id_room);
    snprintf(devices[dev_idx].roomName, sizeof(devices[dev_idx].roomName), "room-%d", id_room);

    snprintf(devices[dev_idx].category, sizeof(devices[dev_idx].category), "switch");
    snprintf(devices[dev_idx].subcategory, sizeof(devices[dev_idx].subcategory), "in_wall");
    snprintf(devices[dev_idx].item_name, sizeof(devices[dev_idx].item_name), "switch"); // defined here: https://api.ezlo.com/devices/items/index.html
    snprintf(devices[dev_idx].devicType, sizeof(devices[dev_idx].devicType), "switch.inwall");
    snprintf(devices[dev_idx].value_type, sizeof(devices[dev_idx].value_type), "bool");

    devices[dev_idx].input_gpio = gpio_in;
    devices[dev_idx].input_inv = ip_inv;
    devices[dev_idx].input_vol = val_ip;
    devices[dev_idx].is_input = true;
    devices[dev_idx].is_meter = 0;
    devices[dev_idx].input_pullup = pullup_ip;
    devices[dev_idx].has_getter = true;
    devices[dev_idx].has_setter = false;
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

    memset(&devices[dev_idx], 0, sizeof(s_device_properties_t));

    devices[dev_idx].dev_type = EZPI_DEV_TYPE_ONE_WIRE;

    if (dev_name)
    {
        snprintf(devices[dev_idx].name, sizeof(devices[dev_idx].name), "%s", dev_name);
    }
    else
    {
        snprintf(devices[dev_idx].name, sizeof(devices[dev_idx].name), "dev-%d:digital_out", dev_idx);
    }

    snprintf(devices[dev_idx].device_id, sizeof(devices[dev_idx].device_id), "de100%.3d", dev_idx);
    snprintf(devices[dev_idx].item_id, sizeof(devices[dev_idx].item_id), "ab100%.3d", dev_idx);
    snprintf(devices[dev_idx].roomId, sizeof(devices[dev_idx].roomId), "abcd%d", id_room);
    snprintf(devices[dev_idx].roomName, sizeof(devices[dev_idx].roomName), "room-%d", id_room);

    snprintf(devices[dev_idx].category, sizeof(devices[dev_idx].category), "temperature");
    devices[dev_idx].subcategory[0] = '\0';
    snprintf(devices[dev_idx].item_name, sizeof(devices[dev_idx].item_name), "temp"); // defined here: https://api.ezlo.com/devices/items/index.html
    snprintf(devices[dev_idx].devicType, sizeof(devices[dev_idx].devicType), "sensor");
    snprintf(devices[dev_idx].value_type, sizeof(devices[dev_idx].value_type), "temperature");

    devices[dev_idx].has_getter = true;
    devices[dev_idx].has_setter = false;

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

    memset(&devices[dev_idx], 0, sizeof(s_device_properties_t));

    devices[dev_idx].dev_type = EZPI_DEV_TYPE_I2C;

    if (dev_name)
    {
        snprintf(devices[dev_idx].name, sizeof(devices[dev_idx].name), "%s", dev_name);
    }
    else
    {
        snprintf(devices[dev_idx].name, sizeof(devices[dev_idx].name), "dev-%d:digital_out", dev_idx);
    }

    snprintf(devices[dev_idx].device_id, sizeof(devices[dev_idx].device_id), "de100%.3d", dev_idx);
    snprintf(devices[dev_idx].item_id, sizeof(devices[dev_idx].item_id), "ab100%.3d", dev_idx);
    snprintf(devices[dev_idx].roomId, sizeof(devices[dev_idx].roomId), "abcd%.3d", dev_idx);
    snprintf(devices[dev_idx].roomName, sizeof(devices[dev_idx].roomName), "room-%.3d", dev_idx);

    snprintf(devices[dev_idx].category, sizeof(devices[dev_idx].category), "level_sensor");
    // devices[dev_idx].subcategory[0] = '\0';
    snprintf(devices[dev_idx].subcategory, sizeof(devices[dev_idx].subcategory), "navigation");
    snprintf(devices[dev_idx].item_name, sizeof(devices[dev_idx].item_name), "acceleration_x_axis");
    snprintf(devices[dev_idx].devicType, sizeof(devices[dev_idx].devicType), "sensor");
    snprintf(devices[dev_idx].value_type, sizeof(devices[dev_idx].value_type), "value");
    devices[dev_idx].has_getter = true;
    devices[dev_idx].has_setter = false;

    mpu_service_init(gpio_scl, gpio_sda, dev_idx);
}

static void add_item_spi(cJSON *o_device, uint32_t dev_idx)
{
}

static void add_device_to_list(cJSON *o_device, uint32_t dev_idx)
{
    int id_item, id_room, dev_type = 0, gpio_out, gpio_in = 0;
    CJSON_GET_VALUE_INT(o_device, "dev_type", dev_type);

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
}

const char *devices_common_get_device_type_id(void)
{
    return DEVICE_TYPE_ID;
}

s_device_properties_t *devices_common_device_list(void)
{
    return devices;
}

int devices_common_get_device_by_item_id(const char *item_id)
{
    for (int idx = 0; idx < MAX_DEV; idx++)
    {
        if (devices[idx].name[0] && item_id[0])
        {
            TRACE_D("Checking: %.*s | %s", sizeof(devices[idx].item_id), devices[idx].item_id, item_id);
            if (0 == strncmp(devices[idx].item_id, item_id, sizeof(devices[idx].item_id)))
            {
                return idx;
            }
        }
        else
        {
            TRACE_B("Found device: ");
            break;
        }
    }

    return 0xFFFF;
}

#if 0
void devices_common_init_devices(void)
{
    esp_err_t err;

    device_count = 0;
    memset(devices, 0, sizeof(s_device_properties_t) * MAX_DEV);

    // char *device_id = devices[0].device_id;
    err = nvs_storage_read_device_config((void *)devices, sizeof(s_device_properties_t) * MAX_DEV);

    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND)
    {
        TRACE_E("NVS get_blob ERROR: %s!\n", esp_err_to_name(err));
    }
    else
    {
        if (err == ESP_OK)
        {
            for (uint8_t i = 0; i < MAX_DEV; i++)
            {

                if (devices[i].name[0])
                {
                    device_count++;
                    devices[i].device_id[7] = 0;
                    snprintf(devices[i].roomName, sizeof(devices[i].roomName), "room_n%d", i);
                    snprintf(devices[i].roomId, sizeof(devices[i].roomId), "acd1%d11", i);

                    switch ((e_device_type_t)devices[i].dev_type)
                    {
                    // case TAMPER:
                    case LED:
                    case SWITCH:
                    case PLUG:
                    {
                        snprintf(devices[i].category, sizeof(devices[i].category), "switch");
                        snprintf(devices[i].subcategory, sizeof(devices[i].subcategory), "in_wall");
                        snprintf(devices[i].item_name, sizeof(devices[i].item_name), "switch");
                        snprintf(devices[i].devicType, sizeof(devices[i].devicType), "switch.inwall");
                        snprintf(devices[i].value_type, sizeof(devices[i].value_type), "bool");
                        devices[i].has_getter = true;
                        devices[i].has_setter = true;
                        break;
                    }
                    case TAMPER:
                    {
                        snprintf(devices[i].category, sizeof(devices[i].category), "temperature");
                        devices[i].subcategory[0] = '\0';
                        // snprintf(devices[i].subcategory, sizeof(devices[i].subcategory), "temperature");
                        snprintf(devices[i].item_name, sizeof(devices[i].item_name), "temp");
                        snprintf(devices[i].devicType, sizeof(devices[i].devicType), "sensor");
                        snprintf(devices[i].value_type, sizeof(devices[i].value_type), "temperature");
                        devices[i].has_getter = true;
                        devices[i].has_setter = false;

                        dht11_service_init(devices[i].input_gpio, i);

                        break;
                    }
                    default:
                        break;
                    }

                    TRACE_B("############# Device Count: %d, type: %u #############", i, devices[i].dev_type);
                    TRACE_B("Name:           %.*s", sizeof(devices[i].name), devices[i].name);
                    TRACE_B("deviceId:       %.*s", sizeof(devices[i].device_id), devices[i].device_id);
                    TRACE_B("itemId:         %.*s", sizeof(devices[i].item_id), devices[i].item_id);
                    TRACE_B("itemName(type): %.*s", sizeof(devices[i].item_name), devices[i].item_name);
                    TRACE_B("roomId:         %.*s", sizeof(devices[i].roomId), devices[i].roomId);
                    TRACE_B("roomName:       %.*s", sizeof(devices[i].roomName), devices[i].roomName);
                    TRACE_B("device type:    %d", devices[i].dev_type);
                    TRACE_B("input gpio:     %d", devices[i].input_gpio);
                    TRACE_B("input inv:      %d", devices[i].input_inv);
                    TRACE_B("input vol:      %d", devices[i].input_vol);
                    TRACE_B("is input:       %d", devices[i].is_input);
                    TRACE_B("is meter:       %d", devices[i].is_meter);
                    TRACE_B("out gpio:       %d", devices[i].out_gpio);
                    TRACE_B("out inv:        %d", devices[i].out_inv);
                    TRACE_B("out vol:        %d", devices[i].out_vol);
                    TRACE_B("*****************************************");
                }
            }
        }
    }
}

#endif