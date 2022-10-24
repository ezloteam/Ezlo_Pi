#include "cJSON.h"

#include "trace.h"
#include "ezlopi_nvs.h"
#include "ezlopi_devices.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_sensors.h"

// typedef

// static void ezlopi_device_map_devices(cJSON *cjson_device);
static void ezlopi_device_parse_json(char *config_string);

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

void ezlopi_device_print_properties(s_ezlopi_device_properties_t *device)
{
    if (device)
    {
        TRACE_D("device->ezlopi_cloud.device_name: %.*s", sizeof(device->ezlopi_cloud.device_name), device->ezlopi_cloud.device_name);
        TRACE_D("device->ezlopi_cloud.category: %s", device->ezlopi_cloud.category ? device->ezlopi_cloud.category : "");
        TRACE_D("device->ezlopi_cloud.subcategory: %s", device->ezlopi_cloud.subcategory ? device->ezlopi_cloud.subcategory : "");
        TRACE_D("device->ezlopi_cloud.item_name: %s", device->ezlopi_cloud.item_name ? device->ezlopi_cloud.item_name : "");
        TRACE_D("device->ezlopi_cloud.device_type: %s", device->ezlopi_cloud.device_type ? device->ezlopi_cloud.device_type : "");
        TRACE_D("device->ezlopi_cloud.value_type: %s", device->ezlopi_cloud.value_type ? device->ezlopi_cloud.value_type : "");
        TRACE_D("device->ezlopi_cloud.has_getter: %s", device->ezlopi_cloud.has_getter ? "true" : "false");
        TRACE_D("device->ezlopi_cloud.has_setter: %s", device->ezlopi_cloud.has_setter ? "true" : "false");
        TRACE_D("device->ezlopi_cloud.reachable: %s", device->ezlopi_cloud.reachable ? "true" : "false");
        TRACE_D("device->ezlopi_cloud.battery_powered: %s", device->ezlopi_cloud.battery_powered ? "true" : "false");
        TRACE_D("device->ezlopi_cloud.show: %s", device->ezlopi_cloud.show ? "true" : "false");
        TRACE_D("device->ezlopi_cloud.room_name: %s", device->ezlopi_cloud.room_name ? device->ezlopi_cloud.room_name : "");
        TRACE_D("device->ezlopi_cloud.device_id: %d", device->ezlopi_cloud.device_id);
        TRACE_D("device->ezlopi_cloud.room_id: %d", device->ezlopi_cloud.room_id);
        TRACE_D("device->ezlopi_cloud.item_id: %d", device->ezlopi_cloud.item_id);

        TRACE_D("device->interface.gpio.gpio_in.enable: %s", device->interface.gpio.gpio_in.enable ? "true" : "false");
        TRACE_D("device->interface.gpio.gpio_in.gpio_num: %d", device->interface.gpio.gpio_in.gpio_num);
        TRACE_D("device->interface.gpio.gpio_in.invert: %s", device->interface.gpio.gpio_in.invert ? "true" : "false");
        TRACE_D("device->interface.gpio.gpio_in.value: %d", device->interface.gpio.gpio_in.value);
        TRACE_D("device->interface.gpio.gpio_in.pull: %d", device->interface.gpio.gpio_in.pull);
        TRACE_D("device->interface.gpio.gpio_in.interrupt: %d", device->interface.gpio.gpio_in.interrupt);

        TRACE_D("device->interface.gpio.gpio_out.enable: %s", device->interface.gpio.gpio_out.enable ? "true" : "false");
        TRACE_D("device->interface.gpio.gpio_out.gpio_num: %d", device->interface.gpio.gpio_out.gpio_num);
        TRACE_D("device->interface.gpio.gpio_out.invert: %s", device->interface.gpio.gpio_out.invert ? "true" : "false");
        TRACE_D("device->interface.gpio.gpio_out.value: %d", device->interface.gpio.gpio_out.value);
        TRACE_D("device->interface.gpio.gpio_out.pull: %d", device->interface.gpio.gpio_out.pull);
        TRACE_D("device->interface.gpio.gpio_in.interrupt: %d", device->interface.gpio.gpio_in.interrupt);

        TRACE_D("device->interface.gpio.gpio_in.enable: %s", device->interface.gpio.gpio_in.enable ? "true" : "false");
        TRACE_D("device->interface.gpio.gpio_in.enable: %s", device->interface.gpio.gpio_in.enable ? "true" : "false");
        TRACE_D("device->interface.gpio.gpio_in.enable: %s", device->interface.gpio.gpio_in.enable ? "true" : "false");
        TRACE_D("device->interface.gpio.gpio_in.enable: %s", device->interface.gpio.gpio_in.enable ? "true" : "false");
        TRACE_D("device->interface.gpio.gpio_in.enable: %s", device->interface.gpio.gpio_in.enable ? "true" : "false");
        TRACE_D("device->interface.gpio.gpio_in.enable: %s", device->interface.gpio.gpio_in.enable ? "true" : "false");
        TRACE_D("device->ezlopi_cloud.subcategory: %s", device->ezlopi_cloud.subcategory ? device->ezlopi_cloud.subcategory : "");
        TRACE_D("device->ezlopi_cloud.subcategory: %s", device->ezlopi_cloud.subcategory ? device->ezlopi_cloud.subcategory : "");
        TRACE_D("device->ezlopi_cloud.subcategory: %s", device->ezlopi_cloud.subcategory ? device->ezlopi_cloud.subcategory : "");
        TRACE_D("device->ezlopi_cloud.subcategory: %s", device->ezlopi_cloud.subcategory ? device->ezlopi_cloud.subcategory : "");
        TRACE_D("device->ezlopi_cloud.subcategory: %s", device->ezlopi_cloud.subcategory ? device->ezlopi_cloud.subcategory : "");
    }
}

#if 0
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
                    s_ezlopi_device_t *sensor_list = ezlopi_devices_list_get_list();

                    int dev_idx = 0;
                    while (NULL != sensor_list[dev_idx].func)
                    {
                        if (id_item == sensor_list[dev_idx].id)
                        {
                            sensor_list[dev_idx].properties = (s_ezlopi_device_properties_t *)sensor_list[dev_idx].func(EZLOPI_ACTION_PREPARE, (void *)cjson_device);
                        }
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

static uint32_t device_id = 0;
static uint32_t item_id = 0;
static uint32_t room_id = 0;

uint32_t ezlopi_device_generate_device_id(void)
{
    device_id = (0 == device_id) ? 0x30000001 : device_id + 1;
    printf("device_id: %u\r\n", device_id);
    return device_id;
}

uint32_t ezlopi_device_generate_item_id(void)
{
    item_id = (0 == item_id) ? 0x20000001 : item_id + 1;
    printf("item_id: %u\r\n", item_id);
    return item_id;
}

uint32_t ezlopi_device_generate_room_id(void)
{
    room_id = (0 == room_id) ? 0x10000001 : room_id + 1;
    printf("room_id: %u\r\n", room_id);
    return room_id;
}