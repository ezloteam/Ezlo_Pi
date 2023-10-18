#include <string.h>
#include "sdkconfig.h"

#include "cJSON.h"
#include "trace.h"

#include "items.h"
#include "ezlopi_timer.h"
#include "ezlopi_actions.h"
#include "1024_device_health.h"

#include "gpio_isr_service.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_cloud_constants.h"
#include "ezlopi_device_value_updated.h"

#if 0 // v2.x
static int device_health_prepare(void *arg);
static int device_health_init(s_ezlopi_device_properties_t *properties);
static int device_health_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg);

int device_health(e_ezlopi_actions_t action, s_ezlopi_device_properties_t *properties, void *arg, void *user_arg)
{
    int ret = 0;
    switch (action)
    {
    case EZLOPI_ACTION_PREPARE:
    {
        ret = device_health_prepare(arg);
        break;
    }
    case EZLOPI_ACTION_INITIALIZE:
    {
        ret = device_health_init(properties);
        break;
    }
    case EZLOPI_ACTION_GET_EZLOPI_VALUE:
    {
        ret = device_health_get_value_cjson(properties, arg);
        break;
    }
    case EZLOPI_ACTION_NOTIFY_200_MS:
    {
        ret = ezlopi_device_value_updated_from_device(properties);
        break;
    }
    default:
    {
        break;
    }
    }

    return ret;
}

static s_ezlopi_device_properties_t *__prepare_start_time_property(void)
{
    s_ezlopi_device_properties_t *properties = malloc(sizeof(s_ezlopi_device_properties_t));
    if (properties)
    {
        const char *device_name = "start_time";
        ASSIGN_DEVICE_NAME(properties, device_name);
        properties->ezlopi_cloud.battery_powered = false;
        properties->ezlopi_cloud.category = category_interface;
        properties->ezlopi_cloud.subcategory = subcategory_not_defined;
        properties->ezlopi_cloud.item_name = "";
    }

    return properties;
}

static int device_health_prepare(void *arg)
{
    int ret = 0;

    return ret;
}

static int device_health_init(s_ezlopi_device_properties_t *properties)
{
    int ret = 0;
    return ret;
}

static int device_health_get_value_cjson(s_ezlopi_device_properties_t *properties, void *arg)
{
    int ret = 0;

    return ret;
}
#endif
