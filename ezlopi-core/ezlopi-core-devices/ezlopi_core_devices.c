#include <ctype.h>

#include "ezlopi_core_factory_info.h"
#include "ezlopi_core_cjson_macros.h"

#include "ezlopi_cloud_items.h"
#include "ezlopi_cloud_constants.h"

static l_ezlopi_device_t* l_device_head = NULL;
static volatile uint32_t g_store_dev_config_with_id = 0;
static s_ezlopi_cloud_controller_t s_controller_information;

static void ezlopi_device_parse_json_v3(cJSON* cj_config);
static void ezlopi_device_free_single(l_ezlopi_device_t* device);
static void ezlopi_device_print_controller_cloud_information_v3(void);

void ezlopi_device_name_set_by_device_id(uint32_t a_device_id, cJSON* cj_new_name)
{
    if (a_device_id && cj_new_name && cj_new_name->valuestring)
    {
        l_ezlopi_device_t* l_device_node = l_device_head;
        while (l_device_node)
        {
            if (a_device_id == l_device_node->cloud_properties.device_id)
            {
                snprintf(l_device_node->cloud_properties.device_name, sizeof(l_device_node->cloud_properties.device_name), "%s", cj_new_name->valuestring);
                break;
            }

            l_device_node = l_device_node->next;
        }

        char* device_config_str = ezlopi_factory_info_v3_get_ezlopi_config();
        if (device_config_str)
        {
            cJSON* cj_device_config = cJSON_Parse(device_config_str);
            ezlopi_factory_info_v3_free(device_config_str);

            if (cj_device_config)
            {
                cJSON* cj_devices = cJSON_GetObjectItem(cj_device_config, ezlopi_dev_detail_str);
                if (cj_devices)
                {
                    uint32_t idx = 0;
                    cJSON* cj_device = NULL;
                    while (NULL != (cj_device = cJSON_GetArrayItem(cj_devices, idx)))
                    {
                        cJSON* cj_device_id = cJSON_GetObjectItem(cj_device, ezlopi_device_id_str);
                        if (cj_device_id && cj_device_id->valuestring)
                        {
                            uint32_t device_id = strtoul(cj_device_id->valuestring, NULL, 16);
                            if (device_id == a_device_id)
                            {
                                cJSON_DeleteItemFromObject(cj_device, ezlopi_dev_name_str);
                                cJSON_AddItemReferenceToObject(cj_device, ezlopi_dev_name_str, cj_new_name);
                                break;
                            }
                        }

                        idx++;
                    }
                }

                char* update_device_config = cJSON_Print(cj_device_config);
                cJSON_Delete(cj_device_config);
                if (update_device_config)
                {
                    cJSON_Minify(update_device_config);
                    ezlopi_factory_info_v3_set_ezlopi_config(update_device_config);
                    free(update_device_config);
                }
            }
        }
    }
}

s_ezlopi_cloud_controller_t* ezlopi_device_get_controller_information(void)
{
    return &s_controller_information;
}

l_ezlopi_device_t* ezlopi_device_get_head(void)
{
    return l_device_head;
}

l_ezlopi_device_t* ezlopi_device_get_by_id(uint32_t device_id)
{
    l_ezlopi_device_t* device_node = l_device_head;

    while (device_node)
    {
        if (device_id == device_node->cloud_properties.device_id)
        {
            break;
        }
        device_node = device_node->next;
    }

    return device_node;
}

l_ezlopi_device_t* ezlopi_device_add_device(cJSON* cj_device)
{
    l_ezlopi_device_t* new_device = malloc(sizeof(l_ezlopi_device_t));
    if (new_device)
    {
        memset(new_device, 0, sizeof(l_ezlopi_device_t));

        char* device_name = NULL;

        CJSON_GET_VALUE_STRING(cj_device, ezlopi_dev_name_str, device_name);
        ASSIGN_DEVICE_NAME_V2(new_device, device_name);
        CJSON_GET_ID(new_device->cloud_properties.device_id, cJSON_GetObjectItem(cj_device, ezlopi_device_id_str));

        TRACE_D("Device name: %s", device_name);
        TRACE_D("Device Id (before): %08x", new_device->cloud_properties.device_id);

        if (new_device->cloud_properties.device_id)
        {
            l_ezlopi_device_t* curr_dev_node = l_device_head;
            while (curr_dev_node)
            {
                if (curr_dev_node->cloud_properties.device_id == new_device->cloud_properties.device_id)
                {
                    g_store_dev_config_with_id = 1;
                    new_device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
                    break;
                }

                curr_dev_node = curr_dev_node->next;
            }

            ezlopi_cloud_update_device_id(new_device->cloud_properties.device_id);
        }
        else
        {
            new_device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();
            CJSON_ASSIGN_ID(cj_device, new_device->cloud_properties.device_id, ezlopi_device_id_str);
            g_store_dev_config_with_id = 1;
        }

        TRACE_D("Device Id (after): %08x", new_device->cloud_properties.device_id);

        if (NULL == l_device_head)
        {
            l_device_head = new_device;
        }
        else
        {
            l_ezlopi_device_t* curr_device = l_device_head;
            while (curr_device->next)
            {
                curr_device = curr_device->next;
            }

            curr_device->next = new_device;
        }
    }

    return new_device;
}

void ezlopi_device_free_device(l_ezlopi_device_t* device)
{
    if (device && l_device_head)
    {
        if (l_device_head == device)
        {
            l_device_head = l_device_head->next;
            device->next = NULL;

            TRACE_D("Device-ID: %08x", device->cloud_properties.device_id);
            ezlopi_device_free_single(device);
        }
        else
        {
            l_ezlopi_device_t* curr_device = l_device_head;
            while (curr_device->next)
            {
                TRACE_D("Device-ID: %08x", curr_device->next->cloud_properties.device_id);
                if (curr_device->next == device)
                {
                    TRACE_E("To free Device-ID: %08x", curr_device->next->cloud_properties.device_id);

                    l_ezlopi_device_t* free_device = curr_device->next;
                    curr_device->next = curr_device->next->next;
                    free_device->next = NULL;
                    ezlopi_device_free_single(free_device);
                    break;
                }

                curr_device = curr_device->next;
            }
        }
    }
}

void ezlopi_device_free_device_by_item(l_ezlopi_item_t* item)
{
    if (item)
    {
        l_ezlopi_device_t* device_node = l_device_head;
        while (device_node)
        {
            l_ezlopi_item_t* item_node = device_node->items;
            while (item_node)
            {
                if (item_node == item)
                {
                    ezlopi_device_free_device(device_node);
                    return;
                }

                item_node = item_node->next;
            }

            device_node = device_node->next;
        }
    }
}

l_ezlopi_item_t* ezlopi_device_get_item_by_id(uint32_t item_id)
{
    l_ezlopi_item_t* item_to_return = NULL;
    l_ezlopi_device_t* device_node = l_device_head;

    while (device_node)
    {
        l_ezlopi_item_t* item_node = device_node->items;
        while (item_node)
        {
            if (item_id == item_node->cloud_properties.item_id)
            {
                item_to_return = item_node;
                break;
            }
            item_node = item_node->next;
        }

        if (item_to_return)
        {
            break;
        }

        device_node = device_node->next;
    }

    return item_to_return;
}

l_ezlopi_item_t* ezlopi_device_add_item_to_device(l_ezlopi_device_t* device, int (*item_func)(e_ezlopi_actions_t action, l_ezlopi_item_t* item, void* arg, void* user_arg))
{
    l_ezlopi_item_t* new_item = NULL;
    if (device)
    {
        new_item = malloc(sizeof(l_ezlopi_item_t));
        if (new_item)
        {
            memset(new_item, 0, sizeof(l_ezlopi_item_t));
            new_item->func = item_func;
            new_item->cloud_properties.device_id = device->cloud_properties.device_id;

            if (NULL == device->items)
            {
                device->items = new_item;
            }
            else
            {
                l_ezlopi_item_t* curr_item = device->items;
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

void ezlopi_device_prepare(void)
{
    s_controller_information.battery_powered = false;
    snprintf(s_controller_information.device_type_id, sizeof(s_controller_information.device_type_id), ezlopi_ezlopi_str);
    s_controller_information.gateway_id[0] = '\0';
    s_controller_information.parent_device_id[0] = '\0';
    s_controller_information.persistent = true;
    s_controller_information.reachable = true;
    s_controller_information.room_id[0] = '\0';
    s_controller_information.security = "no";
    s_controller_information.service_notification = false;
    s_controller_information.ready = true;
    s_controller_information.status = "idle";

#if (EZLOPI_DEVICE_TYPE_GENERIC == EZLOPI_DEVICE_TYPE)
    char* config_string = ezlopi_factory_info_v3_get_ezlopi_config();
#elif (EZLOPI_DEVICE_TYPE_TEST_DEVICE == EZLOPI_DEVICE_TYPE)
    char* config_string = ezlopi_config_test;
#else
    char* config_string = ezlopi_factory_info_v3_get_ezlopi_config();
#endif

    if (config_string)
    {
        TRACE_D("Initial config:\r\n%s", config_string);
        cJSON* cj_config = cJSON_Parse(config_string);
        // ezlopi_factory_info_v3_free(config_string);
        if (cj_config)
        {
            ezlopi_device_parse_json_v3(cj_config);

            if (g_store_dev_config_with_id)
            {
                char* updated_config = cJSON_Print(cj_config);
                if (updated_config)
                {
                    TRACE_D("Updated config:\r\n%s", config_string);
                    cJSON_Minify(updated_config);
                    ezlopi_factory_info_v3_set_ezlopi_config(updated_config);
                    free(updated_config);
                }
            }

            cJSON_Delete(cj_config);
        }
    }
}

///////// Print functions start here ////////////
static void ezlopi_device_print_controller_cloud_information_v3(void)
{
    TRACE_I("Armed: %d", s_controller_information.armed);
    TRACE_I("Battery Powered: %d", s_controller_information.battery_powered);
    TRACE_I("Device Type Id: %.*s", sizeof(s_controller_information.device_type_id), s_controller_information.device_type_id);
    TRACE_I("Gateway Id: %.*s", sizeof(s_controller_information.gateway_id), s_controller_information.gateway_id);
    TRACE_I("Parent Device Id: %.*s", sizeof(s_controller_information.parent_device_id), s_controller_information.parent_device_id);
    TRACE_I("Persistent: %d", s_controller_information.persistent);
    TRACE_I("Reachable: %d", s_controller_information.reachable);
    TRACE_I("Room Id: %.*s", sizeof(s_controller_information.room_id), s_controller_information.room_id);
    TRACE_I("Security: %s", s_controller_information.security ? s_controller_information.security : ezlopi_null_str);
    TRACE_I("Service Notification: %d", s_controller_information.service_notification);
    TRACE_I("Ready: %d", s_controller_information.ready);
    TRACE_I("Status: %s", s_controller_information.status ? s_controller_information.status : ezlopi_null_str);
}

static void ezlopi_device_print_interface_digital_io(l_ezlopi_item_t* item)
{
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.enable: %s", item->interface.gpio.gpio_in.enable ? ezlopi_true_str : ezlopi_false_str);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.gpio_num: %d", item->interface.gpio.gpio_in.gpio_num);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.invert: %s", item->interface.gpio.gpio_in.invert ? ezlopi_true_str : ezlopi_false_str);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.value: %d", item->interface.gpio.gpio_in.value);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.pull: %d", item->interface.gpio.gpio_in.pull);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.interrupt: %d", item->interface.gpio.gpio_in.interrupt);

    TRACE_D(" |~~~|- item->interface.gpio.gpio_out.enable: %s", item->interface.gpio.gpio_out.enable ? ezlopi_true_str : ezlopi_false_str);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_out.gpio_num: %d", item->interface.gpio.gpio_out.gpio_num);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_out.invert: %s", item->interface.gpio.gpio_out.invert ? ezlopi_true_str : ezlopi_false_str);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_out.value: %d", item->interface.gpio.gpio_out.value);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_out.pull: %d", item->interface.gpio.gpio_out.pull);
    TRACE_D(" |~~~|- item->interface.gpio.gpio_in.interrupt: %d", item->interface.gpio.gpio_in.interrupt);
}

static void ezlopi_device_print_interface_analogue_input(l_ezlopi_item_t* item)
{
    TRACE_D(" |~~~|- item->interface.adc.gpio_num: %d", item->interface.adc.gpio_num);
    TRACE_D(" |~~~|- item->interface.adc.resln_bit: %d", item->interface.adc.resln_bit);
}

static void ezlopi_device_print_interface_analogue_output(l_ezlopi_item_t* item) {}

static void ezlopi_device_print_interface_pwm(l_ezlopi_item_t* item)
{
    TRACE_D(" |~~~|- item->interface.pwm.gpio_num: %d", item->interface.pwm.gpio_num);
    TRACE_D(" |~~~|- item->interface.pwm.channel: %d", item->interface.pwm.channel);
    TRACE_D(" |~~~|- item->interface.pwm.speed_mode: %d", item->interface.pwm.speed_mode);
    TRACE_D(" |~~~|- item->interface.pwm.pwm_resln: %d", item->interface.pwm.pwm_resln);
    TRACE_D(" |~~~|- item->interface.pwm.freq_hz: %d", item->interface.pwm.freq_hz);
    TRACE_D(" |~~~|- item->interface.pwm.duty_cycle: %d", item->interface.pwm.duty_cycle);
}

static void ezlopi_device_print_interface_uart(l_ezlopi_item_t* item)
{
    TRACE_D(" |~~~|- item->interface.uart.channel: %d", item->interface.uart.channel);
    TRACE_D(" |~~~|- item->interface.uart.baudrate: %d", item->interface.uart.baudrate);
    TRACE_D(" |~~~|- item->interface.uart.tx: %d", item->interface.uart.tx);
    TRACE_D(" |~~~|- item->interface.uart.rx: %d", item->interface.uart.rx);
    TRACE_D(" |~~~|- item->interface.uart.enable: %d", item->interface.uart.enable);
}

static void ezlopi_device_print_interface_i2c_master(l_ezlopi_item_t* item)
{
    TRACE_D("|~~~|- item->interface.i2c_master.enable: %s", item->interface.i2c_master.enable ? ezlopi_true_str : ezlopi_false_str);
    TRACE_D("|~~~|- item->interface.i2c_master.channel: %d", item->interface.i2c_master.channel);
    TRACE_D("|~~~|- item->interface.i2c_master.clock_speed: %d", item->interface.i2c_master.clock_speed);
    TRACE_D("|~~~|- item->interface.i2c_master.scl: %d", item->interface.i2c_master.scl);
    TRACE_D("|~~~|- item->interface.i2c_master.sda: %d", item->interface.i2c_master.sda);
}

static void ezlopi_device_print_interface_spi_master(l_ezlopi_item_t* item)
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

static void ezlopi_device_print_interface_onewire_master(l_ezlopi_item_t* item)
{
    TRACE_D(" |~~~|- item->interface.onewire_master.enable: %d", item->interface.onewire_master.enable);
    TRACE_D(" |~~~|- item->interface.onewire_master.onewire_pin: %d", item->interface.onewire_master.onewire_pin);
}

static void ezlopi_device_print_interface_type(l_ezlopi_item_t* item)
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
//////////////////// Print functions end here /////////////////////////
///////////////////////////////////////////////////////////////////////

static void ezlopi_device_parse_json_v3(cJSON* cjson_config)
{
    cJSON* cjson_device_list = cJSON_GetObjectItem(cjson_config, ezlopi_dev_detail_str);

    if (cjson_device_list)
    {
        int config_dev_idx = 0;
        cJSON* cjson_device = NULL;

        TRACE_I("---------------------------------------------");
        while (NULL != (cjson_device = cJSON_GetArrayItem(cjson_device_list, config_dev_idx)))
        {
            TRACE_I("Device-%d:", config_dev_idx);

            int id_item = 0;
            CJSON_GET_VALUE_INT(cjson_device, ezlopi_id_item_str, id_item);

            if (0 != id_item)
            {

                s_ezlopi_device_v3_t* v3_sensor_list = ezlopi_devices_list_get_list_v3();
                int dev_idx = 0;

                while (NULL != v3_sensor_list[dev_idx].func)
                {
                    if (id_item == v3_sensor_list[dev_idx].id)
                    {
                        s_ezlopi_prep_arg_t device_prep_arg = { .device = &v3_sensor_list[dev_idx], .cjson_device = cjson_device };
                        v3_sensor_list[dev_idx].func(EZLOPI_ACTION_PREPARE, NULL, (void*)&device_prep_arg, NULL);
                    }

                    dev_idx++;
                }
            }

            config_dev_idx++;
            TRACE_I("---------------------------------------------");
        }
    }

#if 0
    ezlopi_device_print_controller_cloud_information_v3();

    int device_count = 0;
    l_ezlopi_device_t* tm_device_l_list = l_device_head;
    while (tm_device_l_list)
    {
        TRACE_D("|~~~~~~~~~~~~~~~~ Device - %d ~~~~~~~~~~~~~~~~|", device_count + 1);
        // TRACE_D("|- Device Pointer: %p", tm_device_l_list);
        TRACE_D("|- Name: %.*s", 32, isprint(tm_device_l_list->cloud_properties.device_name[0]) ? tm_device_l_list->cloud_properties.device_name : ezlopi_null_str);
        TRACE_D("|- Id: %08X", tm_device_l_list->cloud_properties.device_id);
        TRACE_D("|- Parent Device ID: %08X", tm_device_l_list->cloud_properties.parent_device_id);
        TRACE_D("|- Category: %s", tm_device_l_list->cloud_properties.category ? tm_device_l_list->cloud_properties.category : ezlopi_null_str);
        TRACE_D("|- Sub-category: %s", tm_device_l_list->cloud_properties.subcategory ? tm_device_l_list->cloud_properties.subcategory : ezlopi_null_str);
        TRACE_D("|- Device-type: %s", tm_device_l_list->cloud_properties.device_type ? tm_device_l_list->cloud_properties.device_type : ezlopi_null_str);

        int item_count = 0;
        l_ezlopi_item_t* tm_itme_l_list = tm_device_l_list->items;
        while (tm_itme_l_list)
        {
            TRACE_D("|~~~|--------------- Item - %d ---------------|", item_count + 1);
            TRACE_D("|~~~|- Id: %08X", tm_itme_l_list->cloud_properties.item_id);
            TRACE_D("|~~~|- Category: %s", tm_itme_l_list->cloud_properties.item_name ? tm_itme_l_list->cloud_properties.item_name : ezlopi_null_str);
            // TRACE_D("|~~~|- Value: %s", tm_itme_l_list->cloud_properties.value_type ? tm_itme_l_list->cloud_properties.value_type : "null");
            // TRACE_D("|~~~|- Device-type: %.*s", 32, tm_device_l_list->cloud_properties.device_type ? tm_device_l_list->cloud_properties.device_type : "null");

            TRACE_D("|~~~|- Interface-type: %d", tm_itme_l_list->interface_type);
            ezlopi_device_print_interface_type(tm_itme_l_list);

            tm_itme_l_list = tm_itme_l_list->next;
            item_count++;
        }

        TRACE_D("|~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|");

        tm_device_l_list = tm_device_l_list->next;
        device_count++;
    }
#endif

#if 0
    l_ezlopi_configured_devices_t* current_head = ezlopi_devices_list_get_configured_items();
    while (NULL != current_head)
    {
        ezlopi_device_print_properties(current_head->properties);
        TRACE_I("Device name: %.*s", sizeof(current_head->properties->ezlopi_cloud.device_name), current_head->properties->ezlopi_cloud.device_name);
        current_head = current_head->next;
    }
#endif
}

static void ezlopi_device_free_item(l_ezlopi_item_t* items)
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
//         ezlopi_device_free_setting(settings->next);//recursive
//     }
//
//     free(settings);
// }

static void ezlopi_device_free_single(l_ezlopi_device_t* device)
{
    if (device->items)
    {
        ezlopi_device_free_item(device->items);
        device->items = NULL;
    }

    free(device);
}

l_ezlopi_device_settings_v3_t* ezlopi_device_add_settings_to_device_v3(l_ezlopi_device_t* device, int (*setting_func)(e_ezlopi_settings_action_t action, struct l_ezlopi_device_settings_v3* setting, void* arg, void* user_arg))
{
    l_ezlopi_device_settings_v3_t* new_setting = NULL;
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
                l_ezlopi_device_settings_v3_t* curr_setting = device->settings;
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

cJSON* ezlopi_device_create_device_table_from_prop(l_ezlopi_device_t* device_prop)
{
    cJSON* cj_device = NULL;

    if (device_prop)
    {
        cj_device = cJSON_CreateObject();
        if (cj_device)
        {
            char tmp_string[64];
            snprintf(tmp_string, sizeof(tmp_string), "%08x", device_prop->cloud_properties.device_id);
            cJSON_AddStringToObject(cj_device, ezlopi__id_str, tmp_string);
            if (device_prop->cloud_properties.device_type_id)
            {
                cJSON_AddStringToObject(cj_device, ezlopi_deviceTypeId_str, device_prop->cloud_properties.device_type_id);
            }
            else
            {
                cJSON_AddStringToObject(cj_device, ezlopi_deviceTypeId_str, ezlopi_ezlopi_str);
            }

            if (device_prop->cloud_properties.parent_device_id >= DEVICE_ID_START)
            {
                snprintf(tmp_string, sizeof(tmp_string), "%08x", device_prop->cloud_properties.parent_device_id);
            }
            else
            {
                tmp_string[0] = '\0';
            }
            cJSON_AddStringToObject(cj_device, ezlopi_parentDeviceId_str, tmp_string);
            cJSON_AddStringToObject(cj_device, ezlopi_category_str, device_prop->cloud_properties.category);
            cJSON_AddStringToObject(cj_device, ezlopi_subcategory_str, device_prop->cloud_properties.subcategory);
            snprintf(tmp_string, sizeof(tmp_string), "%08x", ezlopi_cloud_generate_gateway_id());
            cJSON_AddStringToObject(cj_device, ezlopi_gatewayId_str, tmp_string);
            cJSON_AddBoolToObject(cj_device, ezlopi_batteryPowered_str, false);
            cJSON_AddStringToObject(cj_device, ezlopi_name_str, device_prop->cloud_properties.device_name);
            cJSON_AddStringToObject(cj_device, ezlopi_type_str, device_prop->cloud_properties.device_type);
            cJSON_AddBoolToObject(cj_device, ezlopi_reachable_str, true);
            cJSON_AddBoolToObject(cj_device, ezlopi_persistent_str, true);
            cJSON_AddBoolToObject(cj_device, ezlopi_serviceNotification_str, false);
            // cJSON_AddBoolToObject(cj_device, "armed", false);
            cJSON_AddStringToObject(cj_device, ezlopi_roomId_str, ezlopi__str);
            cJSON_AddStringToObject(cj_device, ezlopi_security_str, ezlopi_no_str);
            cJSON_AddBoolToObject(cj_device, ezlopi_ready_str, true);
            cJSON_AddStringToObject(cj_device, ezlopi_status_str, ezlopi_idle_str);
            if (NULL != device_prop->cloud_properties.info)
            {
                cJSON_AddItemReferenceToObject(cj_device, ezlopi_info_str, device_prop->cloud_properties.info);
            }
            // TRACE_E(" Cj_device [%s]", cJSON_Print(cj_device));
        }
    }

    return cj_device;
}
