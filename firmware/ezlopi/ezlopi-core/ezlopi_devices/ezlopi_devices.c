#include "ctype.h"

#include "cJSON.h"

#include "items.h"
#include "trace.h"
#include "ezlopi_nvs.h"
#include "ezlopi_devices.h"
#include "web_provisioning.h"
#include "ezlopi_factory_info.h"

#if 0
static uint32_t device_id = 0;
static uint32_t item_id = 0;
static uint32_t room_id = 0;
static uint32_t gateway_id = 0;
#endif

static l_ezlopi_device_t *l_device_head = NULL;

static void ezlopi_device_parse_json(char *config_string);

void ezlopi_device_prepare(void)
{
    // char *config_string = ezlopi_factory_info_get_ezlopi_config();
    // TRACE_D("config_string: %s", config_string ? config_string : "");
    char *config_string = ezlopi_factory_info_v2_get_ezlopi_config();

    if (config_string)
    {
        ezlopi_device_parse_json(config_string);
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
        // TRACE_D("device->ezlopi_cloud.room_id: 0x%08x", device->ezlopi_cloud.room_id);
        TRACE_D("device->ezlopi_cloud.room_id: \"\"");
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
        {
            TRACE_D("device->interface.adc.gpio_num: %d", device->interface.adc.gpio_num);
            TRACE_D("device->interface.adc.resln_bit: %d", device->interface.adc.resln_bit);
            TRACE_B("###################################################################################################");
            break;
        }
        case EZLOPI_DEVICE_INTERFACE_ANALOG_OUTPUT:
        {
            break;
        }
        case EZLOPI_DEVICE_INTERFACE_PWM:
        {
            TRACE_D("device->interface.pwm.gpio_num: %d", device->interface.pwm.gpio_num);
            TRACE_D("device->interface.pwm.channel: %d", device->interface.pwm.channel);
            TRACE_D("device->interface.pwm.speed_mode: %d", device->interface.pwm.speed_mode);
            TRACE_D("device->interface.pwm.pwm_resln: %d", device->interface.pwm.pwm_resln);
            TRACE_D("device->interface.pwm.freq_hz: %d", device->interface.pwm.freq_hz);
            TRACE_D("device->interface.pwm.duty_cycle: %d", device->interface.pwm.duty_cycle);
            TRACE_B("###################################################################################################");
            break;
        }
        case EZLOPI_DEVICE_INTERFACE_UART:
        {
            TRACE_D("device->interface.uart.channel: %d", device->interface.uart.channel);
            TRACE_D("device->interface.uart.baudrate: %d", device->interface.uart.baudrate);
            TRACE_D("device->interface.uart.tx: %d", device->interface.uart.tx);
            TRACE_D("device->interface.uart.rx: %d", device->interface.uart.rx);
            TRACE_D("device->interface.uart.enable: %d", device->interface.uart.enable);
            TRACE_B("###################################################################################################");
            break;
        }
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
    vTaskDelay(1);
}

static void ezlopi_device_parse_json(char *config_string)
{
    TRACE_I("PARSING - config_string: \n%s", config_string);
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
                            sensor_list[dev_idx].func(EZLOPI_ACTION_PREPARE, NULL, (void *)&device_prep_arg, NULL);
                        }

                        dev_idx++;
                    }
                }

                config_dev_idx++;
                TRACE_B("---------------------------------------------");
            }
        }

        cJSON_Delete(cjson_config);
    }
    else
    {
        TRACE_E("EZLOPI-CONFIG parse- failed!");
    }

    int device_count = 0;
    l_ezlopi_device_t *tm_device_l_list = l_device_head;
    while (tm_device_l_list)
    {
        TRACE_D("|~~~~~~~~~~~~~~~~ Device - %d ~~~~~~~~~~~~~~~~|", device_count);
        TRACE_D("|- Name: %.*s", 32, isprint(tm_device_l_list->cloud_properties.device_name[0]) ? tm_device_l_list->cloud_properties.device_name : "null");
        TRACE_D("|- Id: %08X", tm_device_l_list->cloud_properties.device_id);
        TRACE_D("|- Category: %s", tm_device_l_list->cloud_properties.category ? tm_device_l_list->cloud_properties.category : "null");
        TRACE_D("|- Sub-category: %s", tm_device_l_list->cloud_properties.subcategory ? tm_device_l_list->cloud_properties.subcategory : "null");
        TRACE_D("|- Device-type: %s", tm_device_l_list->cloud_properties.device_type ? tm_device_l_list->cloud_properties.device_type : "null");

        int item_count = 0;
        l_ezlopi_item_t *tm_itme_l_list = tm_device_l_list->items;
        while (tm_itme_l_list)
        {
            TRACE_D("|~~~|--------------- Item - %d ---------------|", item_count);
            TRACE_D("|~~~|- Id: %08X", tm_itme_l_list->cloud_properties.item_id);
            TRACE_D("|~~~|- Interface-type: %d", tm_itme_l_list->interface_type);
            TRACE_D("|~~~|- Category: %s", tm_itme_l_list->cloud_properties.item_name ? tm_itme_l_list->cloud_properties.item_name : "null");
            TRACE_D("|~~~|- Value: %s", tm_itme_l_list->cloud_properties.value_type ? tm_itme_l_list->cloud_properties.value_type : "null");
            TRACE_D("|~~~|- Device-type: %.*s", 32, tm_device_l_list->cloud_properties.device_type ? tm_device_l_list->cloud_properties.device_type : "null");

            tm_itme_l_list = tm_itme_l_list->next;
            item_count++;
        }

        TRACE_D("|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|");

        tm_device_l_list = tm_device_l_list->next;
        device_count++;
    }

#if 0
    l_ezlopi_configured_devices_t *current_head = ezlopi_devices_list_get_configured_items();
    while (NULL != current_head)
    {
        // ezlopi_device_print_properties(current_head->properties);
        TRACE_B("Device name: %.*s", sizeof(current_head->properties->ezlopi_cloud.device_name), current_head->properties->ezlopi_cloud.device_name);
        current_head = current_head->next;
    }
#endif
}

#if 0
// uint32_t ezlopi_device_generate_device_id(void)
// {
//     device_id = (0 == device_id) ? 0x30000001 : device_id + 1;
//     // TRACE_D("device_id: %u\r\n", device_id);
//     return device_id;
// }

// uint32_t ezlopi_device_generate_item_id(void)
// {
//     item_id = (0 == item_id) ? 0x20000001 : item_id + 1;
//     // TRACE_D("item_id: %u\r\n", item_id);
//     return item_id;
// }

// uint32_t ezlopi_device_generate_room_id(void)
// {
//     room_id = (0 == room_id) ? 0x10000001 : room_id + 1;
//     // TRACE_D("room_id: %u\r\n", room_id);
//     return room_id;
// }

// uint32_t ezlopi_device_generate_gateway_id(void)
// {
//     gateway_id = (0 == gateway_id) ? 0x40000001 : gateway_id + 1;
//     // TRACE_D("gateway_id: %u\r\n", gateway_id);
//     return gateway_id;
// }
#endif

l_ezlopi_device_t *ezlopi_device_add_device(void)
{
    l_ezlopi_device_t *new_device = malloc(sizeof(l_ezlopi_device_t));
    if (new_device)
    {
        memset(new_device, 0, sizeof(l_ezlopi_device_t));
        if (NULL == l_device_head)
        {
            l_device_head = new_device;
        }
        else
        {
            l_ezlopi_device_t *curr_device = l_device_head;
            while (curr_device->next)
            {
                curr_device = curr_device->next;
            }

            curr_device->next = new_device;
        }
    }

    return new_device;
}

l_ezlopi_item_t *ezlopi_device_add_item_to_device(l_ezlopi_device_t *device)
{
    l_ezlopi_item_t *new_item = NULL;
    if (device)
    {
        new_item = malloc(sizeof(l_ezlopi_item_t));
        if (new_item)
        {
            memset(new_item, 0, sizeof(l_ezlopi_item_t));

            if (NULL == device->items)
            {
                device->items = new_item;
            }
            else
            {
                l_ezlopi_item_t *curr_item = device->items;
                while (curr_item->next)
                {
                    curr_item = curr_item->next;
                }

                curr_item->next = new_item;
            }
        }
    }

    return new_item;
}

static void ezlopi_device_free_item(l_ezlopi_item_t *items)
{
    if (items->next)
    {
        ezlopi_device_free_item(items->next);
    }

    free(items);
}

static void ezlopi_device_free(l_ezlopi_device_t *device)
{
    if (device->items)
    {
        ezlopi_device_free_item(device->items);
    }
    free(device);
}

void ezlopi_device_free_device(l_ezlopi_device_t *device)
{
    if (device)
    {
        if (l_device_head)
        {
            if (l_device_head == device)
            {
                ezlopi_device_free(l_device_head);
                l_device_head = NULL;
            }
            else
            {
                l_ezlopi_device_t *curr_device = l_device_head;
                while (curr_device->next)
                {
                    if (curr_device->next == device)
                    {
                        break;
                    }

                    curr_device = curr_device->next;
                }

                if (curr_device->next)
                {
                    l_ezlopi_device_t *free_device = curr_device->next;
                    curr_device->next = curr_device->next->next;
                    ezlopi_device_free(free_device);
                }
            }
        }
    }
}
