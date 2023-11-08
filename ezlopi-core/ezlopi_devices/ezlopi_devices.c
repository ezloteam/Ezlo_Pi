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
static s_ezlopi_cloud_controller_t s_controller_information;

static void ezlopi_device_parse_json_v3(char *config_string);
static void ezlopi_device_print_controller_cloud_information_v3(void);

void ezlopi_device_prepare(void)
{
    s_controller_information.armed = false;
    s_controller_information.battery_powered = false;
    // s_controller_information.device_type_id[0] = '\0';
    snprintf(s_controller_information.device_type_id, sizeof(s_controller_information.device_type_id), "ezlopi");
    s_controller_information.gateway_id[0] = '\0';
    // snprintf(s_controller_information.gateway_id, sizeof(s_controller_information.gateway_id), "\0");
    s_controller_information.parent_device_id[0] = '\0';
    // snprintf(s_controller_information.parent_device_id, sizeof(s_controller_information.parent_device_id), "\0");
    s_controller_information.persistent = true;
    s_controller_information.reachable = true;
    s_controller_information.room_id[0] = '\0';
    // snprintf(s_controller_information.room_id, sizeof(s_controller_information.room_id), "\0");
    s_controller_information.security = "null";
    s_controller_information.service_notification = false;
    s_controller_information.ready = true;
    s_controller_information.status = "synced";

    // char *config_string = ezlopi_factory_info_get_ezlopi_config();
    // TRACE_D("config_string: %s", config_string ? config_string : "");
    char *config_string = ezlopi_factory_info_v2_get_ezlopi_config();

    if (config_string)
    {
        ezlopi_device_parse_json_v3(config_string);
    }
}

static void ezlopi_device_print_controller_cloud_information_v3(void)
{
    TRACE_B("Armed: %d", s_controller_information.armed);
    TRACE_B("Battery Powered: %d", s_controller_information.battery_powered);
    TRACE_B("Device Type Id: %.*s", sizeof(s_controller_information.device_type_id), s_controller_information.device_type_id);
    TRACE_B("Gateway Id: %.*s", sizeof(s_controller_information.gateway_id), s_controller_information.gateway_id);
    TRACE_B("Parent Device Id: %.*s", sizeof(s_controller_information.parent_device_id), s_controller_information.parent_device_id);
    TRACE_B("Persistent: %d", s_controller_information.persistent);
    TRACE_B("Reachable: %d", s_controller_information.reachable);
    TRACE_B("Room Id: %.*s", sizeof(s_controller_information.room_id), s_controller_information.room_id);
    TRACE_B("Security: %s", s_controller_information.security ? s_controller_information.security : "null");
    TRACE_B("Service Notification: %d", s_controller_information.service_notification);
    TRACE_B("Ready: %d", s_controller_information.ready);
    TRACE_B("Status: %s", s_controller_information.status ? s_controller_information.status : "null");
}

static void ezlopi_device_print_interface_digital_io(l_ezlopi_item_t *item)
{
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.enable: %s", item->interface.gpio.gpio_in.enable ? "true" : "false");
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.gpio_num: %d", item->interface.gpio.gpio_in.gpio_num);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.invert: %s", item->interface.gpio.gpio_in.invert ? "true" : "false");
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.value: %d", item->interface.gpio.gpio_in.value);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.pull: %d", item->interface.gpio.gpio_in.pull);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.interrupt: %d", item->interface.gpio.gpio_in.interrupt);

    TRACE_D(" |~~~|- item->interface.gpio.gpio_out.enable: %s", item->interface.gpio.gpio_out.enable ? "true" : "false");
    TRACE_D(" |~~~|- item->interface.gpio.gpio_out.gpio_num: %d", item->interface.gpio.gpio_out.gpio_num);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_out.invert: %s", item->interface.gpio.gpio_out.invert ? "true" : "false");
    TRACE_D(" |~~~|- item->interface.gpio.gpio_out.value: %d", item->interface.gpio.gpio_out.value);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_out.pull: %d", item->interface.gpio.gpio_out.pull);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.interrupt: %d", item->interface.gpio.gpio_in.interrupt);
}

static void ezlopi_device_print_interface_analogue_input(l_ezlopi_item_t *item)
{
    TRACE_D(" |~~~|- item->interface.adc.gpio_num: %d", item->interface.adc.gpio_num);
    TRACE_D(" |~~~|- item->interface.adc.resln_bit: %d", item->interface.adc.resln_bit);
}

static void ezlopi_device_print_interface_analogue_output(l_ezlopi_item_t *item) {}

static void ezlopi_device_print_interface_pwm(l_ezlopi_item_t *item)
{
    TRACE_D(" |~~~|- item->interface.pwm.gpio_num: %d", item->interface.pwm.gpio_num);
    TRACE_D(" |~~~|- item->interface.pwm.channel: %d", item->interface.pwm.channel);
    TRACE_D(" |~~~|- item->interface.pwm.speed_mode: %d", item->interface.pwm.speed_mode);
    TRACE_D(" |~~~|- item->interface.pwm.pwm_resln: %d", item->interface.pwm.pwm_resln);
    TRACE_D(" |~~~|- item->interface.pwm.freq_hz: %d", item->interface.pwm.freq_hz);
    TRACE_D(" |~~~|- item->interface.pwm.duty_cycle: %d", item->interface.pwm.duty_cycle);
}

static void ezlopi_device_print_interface_uart(l_ezlopi_item_t *item)
{
    TRACE_D(" |~~~|- item->interface.uart.channel: %d", item->interface.uart.channel);
    TRACE_D(" |~~~|- item->interface.uart.baudrate: %d", item->interface.uart.baudrate);
    TRACE_D(" |~~~|- item->interface.uart.tx: %d", item->interface.uart.tx);
    TRACE_D(" |~~~|- item->interface.uart.rx: %d", item->interface.uart.rx);
    TRACE_D(" |~~~|- item->interface.uart.enable: %d", item->interface.uart.enable);
}

static void ezlopi_device_print_interface_i2c_master(l_ezlopi_item_t *item)
{
    TRACE_D("|~~~|- item->interface.i2c_master.enable: %s", item->interface.i2c_master.enable ? "true" : "false");
    TRACE_D("|~~~|- item->interface.i2c_master.channel: %d", item->interface.i2c_master.channel);
    TRACE_D("|~~~|- item->interface.i2c_master.clock_speed: %d", item->interface.i2c_master.clock_speed);
    TRACE_D("|~~~|- item->interface.i2c_master.scl: %d", item->interface.i2c_master.scl);
    TRACE_D("|~~~|- item->interface.i2c_master.sda: %d", item->interface.i2c_master.sda);
}

static void ezlopi_device_print_interface_spi_master(l_ezlopi_item_t *item)
{
    TRACE_D(" |~~~|- item->interface.spi_master.enable: %d", item->interface.spi_master.enable);
    TRACE_D(" |~~~|- item->interface.spi_master.channel: %d", item->interface.spi_master.channel);
    TRACE_D(" |~~~|- item->interface.spi_master.mode: %d", item->interface.spi_master.mode);
    TRACE_D(" |~~~|- item->interface.spi_master.mosi: %d", item->interface.spi_master.mosi);
    TRACE_D(" |~~~|- item->interface.spi_master.miso: %d", item->interface.spi_master.miso);
    TRACE_D(" |~~~|- item->interface.spi_master.sck: %d", item->interface.spi_master.sck);
    TRACE_D(" |~~~|- item->interface.spi_master.cs: %d", item->interface.spi_master.cs);
    TRACE_D(" |~~~|- item->interface.spi_master.clock_speed_mhz: %d", item->interface.spi_master.clock_speed_mhz);
    TRACE_D(" |~~~|- item->interface.spi_master.command_bits: %d", item->interface.spi_master.command_bits);
    TRACE_D(" |~~~|- item->interface.spi_master.addr_bits: %d", item->interface.spi_master.addr_bits);
    TRACE_D(" |~~~|- item->interface.spi_master.queue_size: %d", item->interface.spi_master.queue_size);
    TRACE_D(" |~~~|- item->interface.spi_master.transfer_sz: %d", item->interface.spi_master.transfer_sz);
    TRACE_D(" |~~~|- item->interface.spi_master.flags: %d", item->interface.spi_master.flags);
}

static void ezlopi_device_print_interface_onewire_master(l_ezlopi_item_t *item)
{
    TRACE_D(" |~~~|- item->interface.onewire_master.enable: %d", item->interface.onewire_master.enable);
    TRACE_D(" |~~~|- item->interface.onewire_master.onewire_pin: %d", item->interface.onewire_master.onewire_pin);
}

static void ezlopi_device_print_interface_type(l_ezlopi_item_t *item)
{
    switch (item->interface_type)
    {
    case EZLOPI_DEVICE_INTERFACE_DIGITAL_OUTPUT:
    case EZLOPI_DEVICE_INTERFACE_DIGITAL_INPUT:
    {
        ezlopi_device_print_interface_digital_io(item);
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_ANALOG_INPUT:
    {
        ezlopi_device_print_interface_analogue_input(item);
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_ANALOG_OUTPUT:
    {
        ezlopi_device_print_interface_analogue_output(item);
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_PWM:
    {
        ezlopi_device_print_interface_pwm(item);
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_UART:
    {
        ezlopi_device_print_interface_uart(item);
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_ONEWIRE_MASTER:
    {
        ezlopi_device_print_interface_onewire_master(item);
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_I2C_MASTER:
    {
        ezlopi_device_print_interface_i2c_master(item);
        break;
    }
    case EZLOPI_DEVICE_INTERFACE_SPI_MASTER:
    {
        ezlopi_device_print_interface_spi_master(item);
        break;
    }
    default:
    {
        break;
    }
    }
}

static void ezlopi_device_parse_json_v3(char *config_string)
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

                if (0 != id_item)
                {

                    s_ezlopi_device_v3_t *v3_sensor_list = ezlopi_devices_list_get_list_v3();
                    int dev_idx = 0;

                    while (NULL != v3_sensor_list[dev_idx].func)
                    {
                        if (id_item == v3_sensor_list[dev_idx].id)
                        {
                            s_ezlopi_prep_arg_t device_prep_arg = {.device = &v3_sensor_list[dev_idx], .cjson_device = cjson_device};
                            v3_sensor_list[dev_idx].func(EZLOPI_ACTION_PREPARE, NULL, (void *)&device_prep_arg, NULL);
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

    ezlopi_device_print_controller_cloud_information_v3();

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
            TRACE_D("|~~~|- Category: %s", tm_itme_l_list->cloud_properties.item_name ? tm_itme_l_list->cloud_properties.item_name : "null");
            TRACE_D("|~~~|- Value: %s", tm_itme_l_list->cloud_properties.value_type ? tm_itme_l_list->cloud_properties.value_type : "null");
            TRACE_D("|~~~|- Device-type: %.*s", 32, tm_device_l_list->cloud_properties.device_type ? tm_device_l_list->cloud_properties.device_type : "null");

            TRACE_D("|~~~|- Interface-type: %d", tm_itme_l_list->interface_type);
            ezlopi_device_print_interface_type(tm_itme_l_list);

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
        ezlopi_device_print_properties(current_head->properties);
        TRACE_B("Device name: %.*s", sizeof(current_head->properties->ezlopi_cloud.device_name), current_head->properties->ezlopi_cloud.device_name);
        current_head = current_head->next;
    }
#endif
}

l_ezlopi_device_t *ezlopi_device_get_head(void)
{
    return l_device_head;
}

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

l_ezlopi_item_t *ezlopi_device_add_item_to_device(l_ezlopi_device_t *device,
                                                  int (*item_func)(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg))
{
    l_ezlopi_item_t *new_item = NULL;
    if (device)
    {
        new_item = malloc(sizeof(l_ezlopi_item_t));
        if (new_item)
        {
            memset(new_item, 0, sizeof(l_ezlopi_item_t));
            new_item->func = item_func;

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

l_ezlopi_device_settings_v3_t *ezlopi_device_add_settings_to_device_v3(l_ezlopi_device_t *device, int (*setting_func)(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3 *setting, void *arg, void *user_arg))
{
    l_ezlopi_device_settings_v3_t *new_setting = NULL;
    if (device)
    {
        new_setting = malloc(sizeof(l_ezlopi_device_settings_v3_t));
        if (new_setting)
        {
            memset(new_setting, 0, sizeof(l_ezlopi_device_settings_v3_t));
            new_setting->func = setting_func;

            if (NULL == device->settings)
            {
                device->settings = new_setting;
            }
            else
            {
                l_ezlopi_device_settings_v3_t *curr_setting = device->settings;
                while (curr_setting->next)
                {
                    curr_setting = curr_setting->next;
                }

                curr_setting->next = new_setting;
            }
        }
    }

    return new_setting;
}

static void ezlopi_device_free_item(l_ezlopi_item_t *items)
{
    if (items->next)
    {
        ezlopi_device_free_item(items->next);
    }

    free(items);
}

// static void ezlopi_device_free_setting(l_ezlopi_device_settings_v3 *settings)
// {
//     if (settings->next)
//     {
//         ezlopi_device_free_item(settings->next);
//     }

//     free(settings);
// }

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

cJSON *ezlopi_device_create_device_table_from_prop(l_ezlopi_device_t *device_prop)
{
    cJSON *cj_device = NULL;
    if (device_prop)
    {
        cj_device = cJSON_CreateObject();
        if (cj_device)
        {
            char tmp_string[64];
            snprintf(tmp_string, sizeof(tmp_string), "%08x", device_prop->cloud_properties.device_id);
            cJSON_AddStringToObject(cj_device, "_id", tmp_string);
            if (device_prop->cloud_properties.device_type_id)
            {
                cJSON_AddStringToObject(cj_device, "deviceTypeId", device_prop->cloud_properties.device_type_id);
            }
            else
            {
                cJSON_AddStringToObject(cj_device, "deviceTypeId", "ezlopi");
            }
            cJSON_AddStringToObject(cj_device, "parentDeviceId", "");
            cJSON_AddStringToObject(cj_device, "category", device_prop->cloud_properties.category);
            cJSON_AddStringToObject(cj_device, "subcategory", device_prop->cloud_properties.subcategory);
            snprintf(tmp_string, sizeof(tmp_string), "%08x", ezlopi_cloud_generate_gateway_id());
            cJSON_AddStringToObject(cj_device, "gatewayId", tmp_string);
            cJSON_AddBoolToObject(cj_device, "batteryPowered", false);
            cJSON_AddStringToObject(cj_device, "name", device_prop->cloud_properties.device_name);
            cJSON_AddStringToObject(cj_device, "type", device_prop->cloud_properties.device_type);
            cJSON_AddBoolToObject(cj_device, "reachable", true);
            cJSON_AddBoolToObject(cj_device, "persistent", true);
            cJSON_AddBoolToObject(cj_device, "serviceNotification", false);
            // cJSON_AddBoolToObject(cj_device, "armed", false);
            cJSON_AddStringToObject(cj_device, "roomId", "");
            cJSON_AddStringToObject(cj_device, "security", "");
            cJSON_AddBoolToObject(cj_device, "ready", true);
            cJSON_AddStringToObject(cj_device, "status", "synced");
            if (NULL != device_prop->cloud_properties.info)
            {
                cJSON_AddItemReferenceToObject(cj_device, "info", device_prop->cloud_properties.info);
            }
        }
    }

    return cj_device;
}
