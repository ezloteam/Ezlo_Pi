#include "ezlopi_actions.h"
#include "ezlopi_devices_list.h"
#include "sensor_bme280.h"
#include "digital_io.h"

static s_ezlopi_device_t device_array[] = {
#ifdef EZLOPI_SENSOR_0018_BME280
    {
        .id = EZLOPI_SENSOR_0018_BME280,
        .func = sensor_bme280,
        // .is_configured = false,
        .properties = NULL,
    },
#endif

#ifdef EZLOPI_SENSOR_0001_LED
    {
        .id = EZLOPI_SENSOR_0001_LED,
        .func = digital_io,
        // .is_configured = false,
        .properties = NULL,
    },
#endif

#ifdef EZLOPI_SENSOR_0002_RELAY
    {
        .id = EZLOPI_SENSOR_0002_RELAY,
        .func = digital_io,
        // .is_configured = false,
        .properties = NULL,
    },
#endif

#ifdef EZLOPI_SENSOR_0003_PLUG
    {
        .id = EZLOPI_SENSOR_0003_PLUG,
        .func = digital_io,
        // .is_configured = false,
        .properties = NULL,
    },
#endif

    /**
     * @brief 'EZLOPI_SENSOR_NONE' must not be removed from this array.
     * This is essential for terminating the loop termination of loop.
     */
    {
        .id = EZLOPI_SENSOR_NONE,
        .func = NULL,
        // .is_configured = false,
        .properties = NULL,
    },
};

s_ezlopi_device_t *ezlopi_devices_list_get_list(void)
{
    return device_array;
}

static l_ezlopi_configured_devices_t *device_list = NULL;
static l_ezlopi_configured_devices_t *ezlopi_device_list_create(s_ezlopi_device_t *device);

l_ezlopi_configured_devices_t *ezlopi_devices_get_configured_items(void)
{
    return device_list;
}

int ezlopi_devices_list_add(s_ezlopi_device_t *device)
{
    int ret = 0;
    if (device_list)
    {
        l_ezlopi_configured_devices_t *current_dev = device_list;

        while (NULL != current_dev->next)
        {
            current_dev = current_dev->next;
        }
        current_dev->next = ezlopi_device_list_create(device);
        if (current_dev->next)
            ret = 1;
    }
    else
    {
        device_list = ezlopi_device_list_create(device);
        if (device_list)
            ret = 1;
    }

    return ret;
}

static l_ezlopi_configured_devices_t *ezlopi_device_list_create(s_ezlopi_device_t *device)
{
    l_ezlopi_configured_devices_t *device_list_element = (l_ezlopi_configured_devices_t *)malloc(sizeof(l_ezlopi_configured_devices_t));
    if (device_list_element)
    {
        device_list_element->device = device;
        device_list_element->next = NULL;
    }
    return device_list_element;
}
