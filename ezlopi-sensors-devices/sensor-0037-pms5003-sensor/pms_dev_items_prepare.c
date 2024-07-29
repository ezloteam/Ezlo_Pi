#include "esp_err.h"

#include "ezlopi_util_trace.h"

#include "ezlopi_core_devices_list.h"
#include "ezlopi_core_devices.h"
#include "ezlopi_core_cjson_macros.h"
#include "ezlopi_core_errors.h"

#include "ezlopi_cloud_constants.h"

#include "pms5003.h"
#include "sensor_0037_pms5003_sensor.h"
#include "EZLOPI_USER_CONFIG.h"

// Device 0
static int __prepare_particulate_matter_particles_0_dot_3_um_device_and_items(cJSON *cj_properties, uint32_t *parent_id, void *user_arg)
{
    int ret = 0;

    l_ezlopi_device_t *particles_0_dot_3_um_device = ezlopi_device_add_device(cj_properties, "Dust_Particles_0.3um");
    if (particles_0_dot_3_um_device)
    {
        ret = 1;
        // char *dev_name = "Dust Particles 0.3 um";
        // ASSIGN_DEVICE_NAME_V2(particles_0_dot_3_um_device, dev_name);
        particles_0_dot_3_um_device->cloud_properties.category = category_level_sensor;
        particles_0_dot_3_um_device->cloud_properties.subcategory = subcategory_particulate_matter;
        particles_0_dot_3_um_device->cloud_properties.device_type = dev_type_sensor;
        particles_0_dot_3_um_device->cloud_properties.parent_device_id = particles_0_dot_3_um_device->cloud_properties.device_id;
        *parent_id = particles_0_dot_3_um_device->cloud_properties.parent_device_id;

        l_ezlopi_item_t *particles_0_dot_3_um_item = ezlopi_device_add_item_to_device(particles_0_dot_3_um_device, sensor_0037_pms5003_v3);
        if (particles_0_dot_3_um_item)
        {
            particles_0_dot_3_um_item->cloud_properties.has_getter = true;
            particles_0_dot_3_um_item->cloud_properties.has_setter = false;
            particles_0_dot_3_um_item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
            particles_0_dot_3_um_item->cloud_properties.item_name = ezlopi_item_name_particles_0_dot_3_um;
            particles_0_dot_3_um_item->cloud_properties.show = true;
            particles_0_dot_3_um_item->cloud_properties.value_type = value_type_substance_amount;
            particles_0_dot_3_um_item->cloud_properties.scale = scales_particles_per_deciliter;

            particles_0_dot_3_um_item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
            particles_0_dot_3_um_item->interface.uart.enable = true;
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio1_str, particles_0_dot_3_um_item->interface.uart.tx);
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio2_str, particles_0_dot_3_um_item->interface.uart.rx);
            particles_0_dot_3_um_item->interface.uart.baudrate = 9600;

            particles_0_dot_3_um_item->user_arg = user_arg;
            particles_0_dot_3_um_item->is_user_arg_unique = true;
        }
        else
        {
            ezlopi_device_free_device(particles_0_dot_3_um_device);
            ret = -1;
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

// Device 1
static int __prepare_particulate_matter_particles_0_dot_5_um_device_and_items(cJSON *cj_properties, uint32_t parent_id, void *user_arg)
{
    int ret = 0;

    l_ezlopi_device_t *particles_0_dot_5_um_device = ezlopi_device_add_device(cj_properties, "Dust_Particles_0.5_um");
    if (particles_0_dot_5_um_device)
    {
        ret = 1;
        // char *dev_name = "Dust Particles 0.5 um";
        // ASSIGN_DEVICE_NAME_V2(particles_0_dot_5_um_device, dev_name);
        particles_0_dot_5_um_device->cloud_properties.category = category_level_sensor;
        particles_0_dot_5_um_device->cloud_properties.subcategory = subcategory_particulate_matter;
        particles_0_dot_5_um_device->cloud_properties.device_type = dev_type_sensor;
        particles_0_dot_5_um_device->cloud_properties.parent_device_id = parent_id;

        l_ezlopi_item_t *particles_0_dot_5_um_item = ezlopi_device_add_item_to_device(particles_0_dot_5_um_device, sensor_0037_pms5003_v3);
        if (particles_0_dot_5_um_item)
        {
            particles_0_dot_5_um_item->cloud_properties.has_getter = true;
            particles_0_dot_5_um_item->cloud_properties.has_setter = false;
            particles_0_dot_5_um_item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
            particles_0_dot_5_um_item->cloud_properties.item_name = ezlopi_item_name_particles_0_dot_5_um;
            particles_0_dot_5_um_item->cloud_properties.show = true;
            particles_0_dot_5_um_item->cloud_properties.value_type = value_type_substance_amount;
            particles_0_dot_5_um_item->cloud_properties.scale = scales_particles_per_deciliter;

            particles_0_dot_5_um_item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
            particles_0_dot_5_um_item->interface.uart.enable = false;
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio1_str, particles_0_dot_5_um_item->interface.uart.tx);
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio2_str, particles_0_dot_5_um_item->interface.uart.rx);
            particles_0_dot_5_um_item->interface.uart.baudrate = 9600;

            particles_0_dot_5_um_item->user_arg = user_arg;
        }
        else
        {
            ret = -1;
            ezlopi_device_free_device(particles_0_dot_5_um_device);
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

// Device 2
static int __prepare_particulate_matter_particles_1_um_device_and_items(cJSON *cj_properties, uint32_t parent_id, void *user_arg)
{
    int ret = 0;

    l_ezlopi_device_t *particles_1_um_device = ezlopi_device_add_device(cj_properties, "Dust_Particles_1_um");
    if (particles_1_um_device)
    {
        ret = 1;
        // char *dev_name = "Dust Particles 1 um";
        // ASSIGN_DEVICE_NAME_V2(particles_1_um_device, dev_name);
        particles_1_um_device->cloud_properties.category = category_level_sensor;
        particles_1_um_device->cloud_properties.subcategory = subcategory_particulate_matter;
        particles_1_um_device->cloud_properties.device_type = dev_type_sensor;
        particles_1_um_device->cloud_properties.parent_device_id = parent_id;

        l_ezlopi_item_t *particles_1_um_item = ezlopi_device_add_item_to_device(particles_1_um_device, sensor_0037_pms5003_v3);
        if (particles_1_um_item)
        {
            particles_1_um_item->cloud_properties.has_getter = true;
            particles_1_um_item->cloud_properties.has_setter = false;
            particles_1_um_item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
            particles_1_um_item->cloud_properties.item_name = ezlopi_item_name_particles_1_um;
            particles_1_um_item->cloud_properties.show = true;
            particles_1_um_item->cloud_properties.value_type = value_type_substance_amount;
            particles_1_um_item->cloud_properties.scale = scales_particles_per_deciliter;

            particles_1_um_item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
            particles_1_um_item->interface.uart.enable = false;
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio1_str, particles_1_um_item->interface.uart.tx);
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio2_str, particles_1_um_item->interface.uart.rx);
            particles_1_um_item->interface.uart.baudrate = 9600;

            particles_1_um_item->user_arg = user_arg;
        }
        else
        {
            ret = -1;
            ezlopi_device_free_device(particles_1_um_device);
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

// Device 3
static int __prepare_particulate_matter_particles_2_dot_5_um_device_and_items(cJSON *cj_properties, uint32_t parent_id, void *user_arg)
{
    int ret = 0;

    l_ezlopi_device_t *particles_2_dot_5_um_device = ezlopi_device_add_device(cj_properties, "Dust_Particles_2.5_um");
    if (particles_2_dot_5_um_device)
    {
        ret = 1;
        // char *dev_name = "Dust Particles 2.5 um";
        // ASSIGN_DEVICE_NAME_V2(particles_2_dot_5_um_device, dev_name);
        particles_2_dot_5_um_device->cloud_properties.category = category_level_sensor;
        particles_2_dot_5_um_device->cloud_properties.subcategory = subcategory_particulate_matter;
        particles_2_dot_5_um_device->cloud_properties.device_type = dev_type_sensor;
        particles_2_dot_5_um_device->cloud_properties.parent_device_id = parent_id;

        l_ezlopi_item_t *particles_2_dot_5_um_item = ezlopi_device_add_item_to_device(particles_2_dot_5_um_device, sensor_0037_pms5003_v3);
        if (particles_2_dot_5_um_item)
        {
            particles_2_dot_5_um_item->cloud_properties.has_getter = true;
            particles_2_dot_5_um_item->cloud_properties.has_setter = false;
            particles_2_dot_5_um_item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
            particles_2_dot_5_um_item->cloud_properties.item_name = ezlopi_item_name_particles_2_dot_5_um;
            particles_2_dot_5_um_item->cloud_properties.show = true;
            particles_2_dot_5_um_item->cloud_properties.value_type = value_type_substance_amount;
            particles_2_dot_5_um_item->cloud_properties.scale = scales_particles_per_deciliter;

            particles_2_dot_5_um_item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
            particles_2_dot_5_um_item->interface.uart.enable = false;
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio1_str, particles_2_dot_5_um_item->interface.uart.tx);
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio2_str, particles_2_dot_5_um_item->interface.uart.rx);
            particles_2_dot_5_um_item->interface.uart.baudrate = 9600;

            particles_2_dot_5_um_item->user_arg = user_arg;
        }
        else
        {
            ret = -1;
            ezlopi_device_free_device(particles_2_dot_5_um_device);
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

// Device 4
static int __prepare_particulate_matter_particles_5_um_device_and_items(cJSON *cj_properties, uint32_t parent_id, void *user_arg)
{
    int ret = 0;

    l_ezlopi_device_t *particles_5_um_device = ezlopi_device_add_device(cj_properties, "Dust_Particles_5_um");
    if (particles_5_um_device)
    {
        ret = 1;
        // char *dev_name = "Dust Particles 5 um";
        // ASSIGN_DEVICE_NAME_V2(particles_5_um_device, dev_name);
        particles_5_um_device->cloud_properties.category = category_level_sensor;
        particles_5_um_device->cloud_properties.subcategory = subcategory_particulate_matter;
        particles_5_um_device->cloud_properties.device_type = dev_type_sensor;
        particles_5_um_device->cloud_properties.parent_device_id = parent_id;

        l_ezlopi_item_t *particles_5_um_item = ezlopi_device_add_item_to_device(particles_5_um_device, sensor_0037_pms5003_v3);
        if (particles_5_um_item)
        {
            particles_5_um_item->cloud_properties.has_getter = true;
            particles_5_um_item->cloud_properties.has_setter = false;
            particles_5_um_item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
            particles_5_um_item->cloud_properties.item_name = ezlopi_item_name_particles_5_um;
            particles_5_um_item->cloud_properties.show = true;
            particles_5_um_item->cloud_properties.value_type = value_type_substance_amount;
            particles_5_um_item->cloud_properties.scale = scales_particles_per_deciliter;

            particles_5_um_item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
            particles_5_um_item->interface.uart.enable = false;
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio1_str, particles_5_um_item->interface.uart.tx);
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio2_str, particles_5_um_item->interface.uart.rx);
            particles_5_um_item->interface.uart.baudrate = 9600;

            particles_5_um_item->user_arg = user_arg;
        }
        else
        {
            ret = -1;
            ezlopi_device_free_device(particles_5_um_device);
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

// Device 5
static int __prepare_particulate_matter_particles_10_um_device_and_items(cJSON *cj_properties, uint32_t parent_id, void *user_arg)
{
    int ret = 0;

    l_ezlopi_device_t *particles_10_um_device = ezlopi_device_add_device(cj_properties, "Dust_Particles_10_um");
    if (particles_10_um_device)
    {
        ret = 1;
        // char *dev_name = "Dust Particles 10 um";
        // ASSIGN_DEVICE_NAME_V2(particles_10_um_device, dev_name);
        particles_10_um_device->cloud_properties.category = category_level_sensor;
        particles_10_um_device->cloud_properties.subcategory = subcategory_particulate_matter;
        particles_10_um_device->cloud_properties.device_type = dev_type_sensor;
        particles_10_um_device->cloud_properties.parent_device_id = parent_id;

        l_ezlopi_item_t *particles_10_um_item = ezlopi_device_add_item_to_device(particles_10_um_device, sensor_0037_pms5003_v3);
        if (particles_10_um_item)
        {
            particles_10_um_item->cloud_properties.has_getter = true;
            particles_10_um_item->cloud_properties.has_setter = false;
            particles_10_um_item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
            particles_10_um_item->cloud_properties.item_name = ezlopi_item_name_particles_10_um;
            particles_10_um_item->cloud_properties.show = true;
            particles_10_um_item->cloud_properties.value_type = value_type_substance_amount;
            particles_10_um_item->cloud_properties.scale = scales_particles_per_deciliter;

            particles_10_um_item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
            particles_10_um_item->interface.uart.enable = false;
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio1_str, particles_10_um_item->interface.uart.tx);
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio2_str, particles_10_um_item->interface.uart.rx);
            particles_10_um_item->interface.uart.baudrate = 9600;

            particles_10_um_item->user_arg = user_arg;
        }
        else
        {
            ret = -1;
            ezlopi_device_free_device(particles_10_um_device);
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

// Device 6
static int __prepare_particulate_matter_standard_particles_1_um_device_and_items(cJSON *cj_properties, uint32_t parent_id, void *user_arg)
{
    int ret = 0;

    l_ezlopi_device_t *standard_particles_1_um_device = ezlopi_device_add_device(cj_properties, "Dust_Particles_PM_1_um");
    if (standard_particles_1_um_device)
    {
        ret = 1;
        // char *dev_name = "Dust Particles PM 1 um";
        // ASSIGN_DEVICE_NAME_V2(standard_particles_1_um_device, dev_name);
        standard_particles_1_um_device->cloud_properties.category = category_level_sensor;
        standard_particles_1_um_device->cloud_properties.subcategory = subcategory_particulate_matter;
        standard_particles_1_um_device->cloud_properties.device_type = dev_type_sensor;
        standard_particles_1_um_device->cloud_properties.parent_device_id = parent_id;

        l_ezlopi_item_t *standard_particles_1_um_item = ezlopi_device_add_item_to_device(standard_particles_1_um_device, sensor_0037_pms5003_v3);
        if (standard_particles_1_um_item)
        {
            standard_particles_1_um_item->cloud_properties.has_getter = true;
            standard_particles_1_um_item->cloud_properties.has_setter = false;
            standard_particles_1_um_item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
            standard_particles_1_um_item->cloud_properties.item_name = ezlopi_item_name_particulate_matter_1;
            standard_particles_1_um_item->cloud_properties.show = true;
            standard_particles_1_um_item->cloud_properties.value_type = value_type_substance_amount;
            standard_particles_1_um_item->cloud_properties.scale = scales_micro_gram_per_cubic_meter;

            standard_particles_1_um_item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
            standard_particles_1_um_item->interface.uart.enable = false;
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio1_str, standard_particles_1_um_item->interface.uart.tx);
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio2_str, standard_particles_1_um_item->interface.uart.rx);
            standard_particles_1_um_item->interface.uart.baudrate = 9600;

            standard_particles_1_um_item->user_arg = user_arg;
        }
        else
        {
            ret = -1;
            ezlopi_device_free_device(standard_particles_1_um_device);
        }
    }
    else
    {
        ret = -1;
    }

    return ret;
}

// Device 7
static int __prepare_particulate_matter_standard_particles_2_dot_5_um_device_and_items(cJSON *cj_properties, uint32_t parent_id, void *user_arg)
{
    int ret = 0;

    l_ezlopi_device_t *standard_particles_2_dot_5_um_device = ezlopi_device_add_device(cj_properties, "Dust_Particles_PM_2.5_um");
    if (standard_particles_2_dot_5_um_device)
    {
        ret = 1;
        // char *dev_name = "Dust Particles PM 2.5 um";
        // ASSIGN_DEVICE_NAME_V2(standard_particles_2_dot_5_um_device, dev_name);
        standard_particles_2_dot_5_um_device->cloud_properties.category = category_level_sensor;
        standard_particles_2_dot_5_um_device->cloud_properties.subcategory = subcategory_particulate_matter;
        standard_particles_2_dot_5_um_device->cloud_properties.device_type = dev_type_sensor;
        standard_particles_2_dot_5_um_device->cloud_properties.parent_device_id = parent_id;

        l_ezlopi_item_t *standard_particles_2_dot_5_um_item = ezlopi_device_add_item_to_device(standard_particles_2_dot_5_um_device, sensor_0037_pms5003_v3);
        if (standard_particles_2_dot_5_um_item)
        {
            standard_particles_2_dot_5_um_item->cloud_properties.has_getter = true;
            standard_particles_2_dot_5_um_item->cloud_properties.has_setter = false;
            standard_particles_2_dot_5_um_item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
            standard_particles_2_dot_5_um_item->cloud_properties.item_name = ezlopi_item_name_particulate_matter_2_dot_5;
            standard_particles_2_dot_5_um_item->cloud_properties.show = true;
            standard_particles_2_dot_5_um_item->cloud_properties.value_type = value_type_substance_amount;
            standard_particles_2_dot_5_um_item->cloud_properties.scale = scales_micro_gram_per_cubic_meter;

            standard_particles_2_dot_5_um_item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
            standard_particles_2_dot_5_um_item->interface.uart.enable = false;
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio1_str, standard_particles_2_dot_5_um_item->interface.uart.tx);
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio2_str, standard_particles_2_dot_5_um_item->interface.uart.rx);
            standard_particles_2_dot_5_um_item->interface.uart.baudrate = 9600;

            standard_particles_2_dot_5_um_item->user_arg = user_arg;
        }
        else
        {
            ret = -1;
            ezlopi_device_free_device(standard_particles_2_dot_5_um_device);
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

// Device 8
static int __prepare_particulate_matter_standard_particles_10_um_device_and_items(cJSON *cj_properties, uint32_t parent_id, void *user_arg)
{
    int ret = 0;

    l_ezlopi_device_t *standard_particles_10_um_device = ezlopi_device_add_device(cj_properties, "Dust_Particles_PM_10_um");
    if (standard_particles_10_um_device)
    {
        ret = 1;
        // char *dev_name = "Dust Particles PM 10 um";
        // ASSIGN_DEVICE_NAME_V2(standard_particles_10_um_device, dev_name);
        standard_particles_10_um_device->cloud_properties.category = category_level_sensor;
        standard_particles_10_um_device->cloud_properties.subcategory = subcategory_particulate_matter;
        standard_particles_10_um_device->cloud_properties.device_type = dev_type_sensor;
        standard_particles_10_um_device->cloud_properties.parent_device_id = parent_id;

        l_ezlopi_item_t *standard_particles_10_um_item = ezlopi_device_add_item_to_device(standard_particles_10_um_device, sensor_0037_pms5003_v3);
        if (standard_particles_10_um_item)
        {
            standard_particles_10_um_item->cloud_properties.has_getter = true;
            standard_particles_10_um_item->cloud_properties.has_setter = false;
            standard_particles_10_um_item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();
            standard_particles_10_um_item->cloud_properties.item_name = ezlopi_item_name_particulate_matter_10;
            standard_particles_10_um_item->cloud_properties.show = true;
            standard_particles_10_um_item->cloud_properties.value_type = value_type_substance_amount;
            standard_particles_10_um_item->cloud_properties.scale = scales_micro_gram_per_cubic_meter;

            standard_particles_10_um_item->interface_type = EZLOPI_DEVICE_INTERFACE_UART;
            standard_particles_10_um_item->interface.uart.enable = false;
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio1_str, standard_particles_10_um_item->interface.uart.tx);
            CJSON_GET_VALUE_GPIO(cj_properties, ezlopi_gpio2_str, standard_particles_10_um_item->interface.uart.rx);
            standard_particles_10_um_item->interface.uart.baudrate = 9600;

            standard_particles_10_um_item->user_arg = user_arg;
        }
        else
        {
            ret = -1;
            ezlopi_device_free_device(standard_particles_10_um_device);
        }
    }
    else
    {
        ret = 1;
    }

    return ret;
}

static int pms5003_set_pms_object_details(cJSON *cj_properties, s_pms5003_sensor_object *pms_object)
{
    int ret = 0;

    assert(pms_object != NULL);

    CJSON_GET_VALUE_DOUBLE(cj_properties, ezlopi_gpio1_str, pms_object->pms_tx_pin);
    CJSON_GET_VALUE_DOUBLE(cj_properties, ezlopi_gpio2_str, pms_object->pms_rx_pin);
    CJSON_GET_VALUE_DOUBLE(cj_properties, ezlopi_gpio3_str, pms_object->pms_set_pin);
    CJSON_GET_VALUE_DOUBLE(cj_properties, "gpio4", pms_object->pms_reset_pin);
    pms_object->pms_baud_rate = 9600;

    pms_object->pms_active_time = 30;
    pms_object->pms_sleep_time = 120;
    pms_object->pms_stability_time = 15;
    pms_object->pmsStatusReg = 0x00;

    memset(&pms_object->pms_data, 0, sizeof(PM25_AQI_Data));

    pms_object->counter = 0;

    return ret;
}

ezlopi_error_t pms5003_sensor_preapre_devices_and_items(cJSON *cj_properties, uint32_t *parent_id)
{
    ezlopi_error_t ret = EZPI_FAILED;

    s_pms5003_sensor_object *pms_object = (s_pms5003_sensor_object *)ezlopi_malloc(__FUNCTION__, sizeof(s_pms5003_sensor_object));
    if (pms_object)
    {
        pms5003_set_pms_object_details(cj_properties, pms_object);
        ESP_ERROR_CHECK(__prepare_particulate_matter_particles_0_dot_3_um_device_and_items(cj_properties, parent_id, (void *)pms_object));
        ESP_ERROR_CHECK(__prepare_particulate_matter_particles_0_dot_5_um_device_and_items(cj_properties, *parent_id, (void *)pms_object));
        ESP_ERROR_CHECK(__prepare_particulate_matter_particles_1_um_device_and_items(cj_properties, *parent_id, (void *)pms_object));
        ESP_ERROR_CHECK(__prepare_particulate_matter_particles_2_dot_5_um_device_and_items(cj_properties, *parent_id, (void *)pms_object));
        ESP_ERROR_CHECK(__prepare_particulate_matter_particles_5_um_device_and_items(cj_properties, *parent_id, (void *)pms_object));
        ESP_ERROR_CHECK(__prepare_particulate_matter_particles_10_um_device_and_items(cj_properties, *parent_id, (void *)pms_object));
        ESP_ERROR_CHECK(__prepare_particulate_matter_standard_particles_1_um_device_and_items(cj_properties, *parent_id, (void *)pms_object));
        ESP_ERROR_CHECK(__prepare_particulate_matter_standard_particles_2_dot_5_um_device_and_items(cj_properties, *parent_id, (void *)pms_object));
        ESP_ERROR_CHECK(__prepare_particulate_matter_standard_particles_10_um_device_and_items(cj_properties, *parent_id, (void *)pms_object));
        ret = EZPI_SUCCESS;
    }

    return ret;
}
