#include "ezlopi_actions.h"
#include "ezlopi_devices_list.h"
#include "sensor_bme280.h"
#include "digital_io.h"
#include "sensor_pir.h"
#include "sensor_door.h"
#include "dimmable_bulb.h"
#include "joystick_2_axis.h"
#include "ultrasonic_MB1013.h"
#include "servo_motor_MG_996R.h"
#include "ultrasonic_HC_SR04.h"
#include "sensor_touch_tpp_223b.h"
#include "025_sens_ldr_digital_module.h"
#include "026_sens_ldr_analog_sensor.h"
#include "027_sens_water_leak_sensor.h"
// #include "028_sens_i2c_accelerometer.h"
#include "029_IR_blaster_remote.h"
#include "sensor_sound.h"
#include "1024_device_health.h"
#include "0029_gxhtc3_rh_t_sensor.h"
#include "jsn_sr04t.h"
#include "turbidity.h"
#include "proximity.h"
#include "030_sens_ds18b20_sensor.h"
#include "032_sens_soil_moisture.h"
#include "016_sens_dht22_sensor.h"

static s_ezlopi_device_v3_t device_array_v3[] = {
#ifdef EZLOPI_SENSOR_0001_LED
    {
        .id = EZLOPI_SENSOR_0001_LED,
        .func = digital_io_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_0002_RELAY
    {
        .id = EZLOPI_SENSOR_0002_RELAY,
        .func = digital_io_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_0019_PIR
    {
        .id = EZLOPI_SENSOR_0019_PIR,
        .func = sensor_pir_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_034_Proximity_Sensor
    {
        .id = EZLOPI_SENSOR_034_Proximity_Sensor,
        .func = proximity_sensor,
    },
#endif

#ifdef EZLOPI_SENSOR_0016_DHT22_SENSOR
    {
        .id = EZLOPI_SENSOR_0016_DHT22_SENSOR,
        .func = dht22_sensor_v3,
    },
#endif
#ifdef EZLOPI_SENSOR_0025_LDR_DIGITAL_MODULE_SENSOR
    {
        .id = EZLOPI_SENSOR_0025_LDR_DIGITAL_MODULE_SENSOR,
        .func = sensor_ldr_digital_module_v3,
    },
#endif
#ifdef EZLOPI_SENSOR_0026_LDR_ANALOGUE_SENSOR
    {
        .id = EZLOPI_SENSOR_0026_LDR_ANALOGUE_SENSOR,
        .func = sensor_ldr_analog_v3,
    },
#endif
    /**
     * @brief 'EZLOPI_SENSOR_NONE' must not be removed from this array.
     * This is essential for terminating the loop termination of loop.
     */
    {
        .id = EZLOPI_SENSOR_NONE,
        .func = NULL,
    },
};

#if 1 // version 3.x.x
s_ezlopi_device_v3_t *ezlopi_devices_list_get_list_v3(void)
{
    return device_array_v3;
}
#endif

#if 1 // version 1.x.x to 2.x.x
s_ezlopi_device_t *ezlopi_devices_list_get_list(void)
{
    return NULL;
    // device_array;
}

static l_ezlopi_configured_devices_t *configured_device = NULL;
static l_ezlopi_configured_devices_t *ezlopi_device_list_create(s_ezlopi_device_t *device, s_ezlopi_device_properties_t *properties, void *user_arg);

l_ezlopi_configured_devices_t *ezlopi_devices_list_get_configured_items(void)
{
    return configured_device;
}

int ezlopi_devices_list_add(s_ezlopi_device_t *device, s_ezlopi_device_properties_t *properties, void *user_arg)
{
    int ret = 0;
    if (configured_device)
    {
        l_ezlopi_configured_devices_t *current_dev = configured_device;

        while (NULL != current_dev->next)
        {
            current_dev = current_dev->next;
        }

        current_dev->next = ezlopi_device_list_create(device, properties, user_arg);
        if (current_dev->next)
        {
            ret = 1;
        }
    }
    else
    {
        configured_device = ezlopi_device_list_create(device, properties, user_arg);
        if (configured_device)
        {
            ret = 1;
        }
    }

    return ret;
}

static l_ezlopi_configured_devices_t *ezlopi_device_list_create(s_ezlopi_device_t *device, s_ezlopi_device_properties_t *properties, void *user_arg)
{
    l_ezlopi_configured_devices_t *device_list_element = (l_ezlopi_configured_devices_t *)malloc(sizeof(l_ezlopi_configured_devices_t));
    if (device_list_element)
    {
        memset(device_list_element, 0, sizeof(l_ezlopi_configured_devices_t));
        device_list_element->device = device;
        device_list_element->properties = properties;
        device_list_element->next = NULL;
        if (user_arg)
        {
            device_list_element->user_arg = user_arg;
        }
    }
    return device_list_element;
}
#endif
