#include <cJSON.h>

#include "items.h"
#include "trace.h"
#include "ezlopi_nvs.h"
#include "ezlopi_devices.h"
#include "web_provisioning.h"

static uint32_t device_id = 0;
static uint32_t item_id = 0;
static uint32_t room_id = 0;

static void ezlopi_device_parse_json(char *config_string);

void ezlopi_device_prepare(void)
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
        TRACE_B("***************************************************************************************************");
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
        TRACE_D("device->ezlopi_cloud.device_id: 0x%08x", device->ezlopi_cloud.device_id);
        TRACE_D("device->ezlopi_cloud.room_id: 0x%08x", device->ezlopi_cloud.room_id);
        TRACE_D("device->ezlopi_cloud.item_id: 0x%08x", device->ezlopi_cloud.item_id);
        switch (device->interface_type)
        {
        case EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT:
        case EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT:
        {
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
            TRACE_B("###################################################################################################");
            break;
        }
        case EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT:
        case EZLOPI_DEVICE_INTERFACE_ANALOG_OUTPUT:
        case EZLOPI_DEVICE_INTERFACE_I2C_MASTER:
        {
            TRACE_D("device->interface.i2c_master.enable: %s", device->interface.i2c_master.enable ? "true" : "false");
            TRACE_D("device->interface.i2c_master.channel: %d", device->interface.i2c_master.channel);
            TRACE_D("device->interface.i2c_master.clock_speed: %d", device->interface.i2c_master.clock_speed);
            TRACE_D("device->interface.i2c_master.scl: %d", device->interface.i2c_master.scl);
            TRACE_D("device->interface.i2c_master.sda: %d", device->interface.i2c_master.sda);
            TRACE_B("###################################################################################################");
            break;
        }
        default:
        {
            TRACE_E("Default interface type: %d", device->interface_type);
            break;
        }
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
            int config_dev_idx = 0;
            cJSON *cjson_device = NULL;

            TRACE_B("---------------------------------------------");
            while (NULL != (cjson_device = cJSON_GetArrayItem(cjson_device_list, config_dev_idx)))
            {
                TRACE_B("Device-%d:", config_dev_idx);

                // char *device_name = NULL;
                // CJSON_GET_VALUE_STRING(cjson_device, "dev_name", device_name);
                // TRACE_D("device name: %s", device_name ? device_name : "");

                int id_item = 0;
                CJSON_GET_VALUE_INT(cjson_device, "id_item", id_item);
                TRACE_B("id_item: %d", id_item);

                if (0 != id_item)
                {
                    s_ezlopi_device_t *sensor_list = ezlopi_devices_list_get_list();
                    int dev_idx = 0;
                    while (NULL != sensor_list[dev_idx].func)
                    {
                        
                        if (id_item == sensor_list[dev_idx].id)
                        {
                            
                            s_ezlopi_prep_arg_t device_prep_arg = {.device = &sensor_list[dev_idx], .cjson_device = cjson_device};
                            sensor_list[dev_idx].func(EZLOPI_ACTION_PREPARE, NULL, (void *)&device_prep_arg);
                        }

                        dev_idx++;
                    }

                    l_ezlopi_configured_devices_t *current_head = ezlopi_devices_list_get_configured_items();
                    while (NULL != current_head)
                    {
                        ezlopi_device_print_properties(current_head->properties);
                        current_head = current_head->next;
                    }
                }

                config_dev_idx++;
                TRACE_B("---------------------------------------------");
            }
        }

        cJSON_Delete(cjson_config);
    }
}

uint32_t ezlopi_device_generate_device_id(void)
{
    device_id = (0 == device_id) ? 0x30000001 : device_id + 1;
    // TRACE_D("device_id: %u\r\n", device_id);
    return device_id;
}

uint32_t ezlopi_device_generate_item_id(void)
{
    item_id = (0 == item_id) ? 0x20000001 : item_id + 1;
    // TRACE_D("item_id: %u\r\n", item_id);
    return item_id;
}

uint32_t ezlopi_device_generate_room_id(void)
{
    room_id = (0 == room_id) ? 0x10000001 : room_id + 1;
    // TRACE_D("room_id: %u\r\n", room_id);
    return room_id;
}