#include "ezlopi_actions.h"
#include "ezlopi_devices_list.h"
#include "digital_io.h"
#include "sensor_pir.h"
// #include "door_hall_sensor.h"
#include "dimmable_bulb.h"
#include "joystick_2_axis.h"
#include "ultrasonic_MB1013.h"
#include "servo_motor_MG_996R.h"
#include "ultrasonic_HC_SR04.h"
#include "0023_sensor_touch_switch_tpp_223b.h"
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
#include "034_proximity_sensor.h"
#include "030_sens_ds18b20_sensor.h"
#include "032_sens_soil_moisture.h"
#include "016_sens_dht22_sensor.h"
#include "0012_bme280_sensor.h"
#include "0035_sensor_touch_sensor_tpp_223b.h"
#include "0038_digitalOut_RGB.h"
#include "0044_sensor_I2C_TSL256_luminosity.h"
#include "0008_sensor_I2C_LTR303ALS.h"
#include "0009_other_RMT_SK6812.h"
#include "0010_I2C_BME680.h"
#include "0056_sensor_ADC_Force_Sensitive_Resistor.h"

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

#ifdef EZLOPI_SENSOR_0008_ALS_LTR303_I2C_SENSOR
    {
        .id = EZLOPI_SENSOR_0008_ALS_LTR303_I2C_SENSOR,
        .func = sensor_0008_I2C_LTR303ALS_v3,
    },
#endif

#ifdef EZLOPI_DEVICE_0009_SK6812_LED_STRIP
    {
        .id = EZLOPI_DEVICE_0009_SK6812_LED_STRIP,
        .func = device_0009_other_RMT_SK6812_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_0010_BME680_I2C
    {
        .id = EZLOPI_SENSOR_0010_BME680_I2C,
        .func = sensor_0010_I2C_BME680_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_0012_BME280_I2C
    {
        .id = EZLOPI_SENSOR_0012_BME280_I2C,
        .func = sensor_0012_bme280_sensor_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_0019_PIR
    {
        .id = EZLOPI_SENSOR_0019_PIR,
        .func = sensor_pir_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_0022_DIMMABLE_BULB
    {
        .id = EZLOPI_SENSOR_0022_DIMMABLE_BULB,
        .func = ezlopi_dimmable_bulb_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_0023_TTP_223B_TOUCH_SWITCH
    {
        .id = EZLOPI_SENSOR_0023_TTP_223B_TOUCH_SWITCH,
        .func = sensor_touch_ttp_223b_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_034_PROXIMITY_SENSOR
    {
        .id = EZLOPI_SENSOR_034_PROXIMITY_SENSOR,
        .func = proximity_sensor_v3,
    },
#endif

#ifdef EZLOPI_DEVICE_0038_RGB_LED
    {
        .id = EZLOPI_DEVICE_0038_RGB_LED,
        .func = device_0038_digitalOut_RGB_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_0035_TPP_32B_TOUCH_SENSOR
    {
        .id = EZLOPI_SENSOR_0035_TPP_32B_TOUCH_SENSOR,
        .func = sensor_touch_sensor_ttp_223b_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_0016_DHT22_SENSOR
    {
        .id = EZLOPI_SENSOR_0016_DHT22_SENSOR,
        .func = dht22_sensor_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_0024_ULTRASONIC_HC_SR04_SENSOR
    {
        .id = EZLOPI_SENSOR_0024_ULTRASONIC_HC_SR04_SENSOR,
        .func = sensor_0024_other_HCSR04_v3,
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

#ifdef EZLOPI_SENSOR_0027_WATER_LEAK
    {
        .id = EZLOPI_SENSOR_0027_WATER_LEAK,
        .func = water_leak_sensor_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_029_IR_BLASTER
    {
        .id = EZLOPI_SENSOR_029_IR_BLASTER,
        .func = IR_blaster_remote_v3,
    },
#endif
#ifdef EZLOPI_SENSOR_030_DS18B20
    {
        .id = EZLOPI_SENSOR_030_DS18B20,
        .func = ds18b20_sensor_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_032_SOIL_MOISTURE
    {
        .id = EZLOPI_SENSOR_032_SOIL_MOISTURE,
        .func = soil_moisture_sensor_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_033_TURBIDITY_SENSOR
    {
        .id = EZLOPI_SENSOR_033_TURBIDITY_SENSOR,
        .func = turbidity_sensor_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_0044_I2C_TSL2561_LUMINOSITY
    {
        .id = EZLOPI_SENSOR_0044_I2C_TSL2561_LUMINOSITY,
        .func = sensor_TSL256_luminosity_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_0056_ADC_FSR
    {
        .id = EZLOPI_SENSOR_0056_ADC_FSR,
        .func = sensor_0056_fsr_v3,
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
