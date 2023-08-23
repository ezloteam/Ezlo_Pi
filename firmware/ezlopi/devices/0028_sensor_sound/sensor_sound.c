#include "sensor_sound.h"
#include "ADS131M08.h"

#include "cJSON.h"
#include "trace.h"
#include "frozen.h"
#include "ezlopi_actions.h"
#include "ezlopi_timer.h"
#include "items.h"

#include "ezlopi_cloud.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_device_value_updated.h"
#include "ezlopi_cloud_constants.h"
#include "stdlib.h"

static const char *audible = "audible";
static const char *silent = "silent";

static int __preapare(void *arg);
static int __init(l_ezlopi_item_t *item);
static int __notify(l_ezlopi_item_t *item);
static int __get_cjson_value(l_ezlopi_item_t *item, void *arg);

int sound_sensor_v3(e_ezlopi_actions_t action, l_ezlopi_item_t *item, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = __preapare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = __init(item);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        __get_cjson_value(item, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_1000_MS:
    {
        __notify(item);
        break;
    }
    default:
    {
        break;
    }
    }
    return ret;
}

static int __notify(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        bool prev_audio_status = (bool)item->user_arg;
        bool curr_audio_status = ADS131_value();
        if ((prev_audio_status != curr_audio_status))
        {
            ret = ezlopi_device_value_updated_from_device_v3(item);
            item->user_arg = (void *)curr_audio_status;
        }
    }
    return ret;
}

static int __get_cjson_value(l_ezlopi_item_t *item, void *arg)
{
    int ret = 0;
    if (item && arg)
    {
        cJSON *result = (cJSON *)arg;
        cJSON_AddBoolToObject(result, "value", (bool)item->user_arg);
    }
    return ret;
}

static int __init(l_ezlopi_item_t *item)
{
    int ret = 0;
    if (item)
    {
        ADS131_init(item->interface.spi_master.cs, item->interface.spi_master.miso, item->interface.spi_master.mosi, item->interface.spi_master.sck);
    }
    return ret;
}

static int __preapare(void *arg)
{
    int ret = 0;
    s_ezlopi_prep_arg_t *prep_arg = (s_ezlopi_prep_arg_t *)arg;
    if (prep_arg)
    {
        if (prep_arg->cjson_device)
        {
            l_ezlopi_device_t *device = ezlopi_device_add_device();
            if (device)
            {
                char *device_name = NULL;
                CJSON_GET_VALUE_STRING(prep_arg->cjson_device, "dev_name", device_name);

                ASSIGN_DEVICE_NAME_V2(device, device_name);
                device->cloud_properties.category = category_level_sensor;
                device->cloud_properties.subcategory = subcategory_sound;
                device->cloud_properties.device_type = dev_type_sensor;
                device->cloud_properties.device_id = ezlopi_cloud_generate_device_id();

                l_ezlopi_item_t *item = ezlopi_device_add_item_to_device(device, sound_sensor_v3);
                if (item)
                {
                    item->cloud_properties.show = true;
                    item->cloud_properties.has_getter = true;
                    item->cloud_properties.has_setter = false;
                    item->cloud_properties.item_name = ezlopi_item_name_sounding_mode;
                    item->cloud_properties.value_type = value_type_bool;
                    item->cloud_properties.scale = NULL;
                    item->cloud_properties.item_id = ezlopi_cloud_generate_item_id();

                    item->interface_type = EZLOPI_DEVICE_INTERFACE_SPI_MASTER;
                    CJSON_GET_VALUE_INT(prep_arg->cjson_device, "gpio_miso", item->interface.spi_master.miso);
                    CJSON_GET_VALUE_INT(prep_arg->cjson_device, "gpio_mosi", item->interface.spi_master.mosi);
                    CJSON_GET_VALUE_INT(prep_arg->cjson_device, "gpio_sck", item->interface.spi_master.sck);
                    CJSON_GET_VALUE_INT(prep_arg->cjson_device, "gpio_cs", item->interface.spi_master.cs);
                }
            }
        }
    }
    return ret;
}
