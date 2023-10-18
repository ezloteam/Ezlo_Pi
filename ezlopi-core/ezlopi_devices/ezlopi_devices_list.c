#include "ezlopi_actions.h"
#include "ezlopi_devices_list.h"

#include "device_0001_digitalOut_generic.h"
#include "device_0002_digitalOut_relay.h"
#include "device_0003_digitalOut_plug.h"
#include "device_0004_digitalIn_generic.h"

#include "sensor_0008_I2C_LTR303ALS.h"
#include "device_0009_other_RMT_SK6812.h"

#include "sensor_0012_I2C_BME280.h"

#include "sensor_0016_oneWire_DHT22.h"

#include "sensor_0019_digitalIn_PIR.h"
#include "sensor_0021_UART_MB1013.h"
#include "device_0022_PWM_dimmable_lamp.h"
#include "sensor_0023_digitalIn_touch_switch_TTP223B.h"
#include "sensor_0024_other_HCSR04.h"
#include "sensor_0025_digitalIn_LDR.h"
#include "sensor_0026_ADC_LDR.h"
#include "sensor_0027_ADC_waterLeak.h"
#include "sensor_0029_I2C_GXHTC3.h"
#include "sensor_0030_oneWire_DS18B20.h"
#include "sensor_0031_other_JSNSR04T.h"
#include "sensor_0032_ADC_soilMoisture.h"
#include "sensor_0033_ADC_turbidity.h"
#include "sensor_0034_digitalIn_proximity.h"
#include "sensor_0035_digitalIn_touch_sensor_TPP223B.h"
#include "device_0036_PWM_servo_MG996R.h"
#include "device_0038_other_RGB.h"

#include "sensor_0044_I2C_TSL256_luminosity.h"
#include "sensor_0065_digitalIn_float_switch.h"
#include "sensor_0061_digitalIn_reed_switch.h"
#include "sensor_0060_digitalIn_vibration_detector.h"
#include "sensor_0055_ADC_FlexResistor.h"
#include "sensor_0048_other_MQ4_CH4_detector.h"
#include "sensor_0049_other_MQ2_LPG_detector.h"
#include "sensor_0050_other_MQ3_alcohol_detector.h"
#include "sensor_0051_other_MQ8_H2_detector.h"
#include "sensor_0052_other_MQ135_NH3_detector.h"
#include "sensor_0059_other_MQ6_LPG_detector.h"
#include "sensor_0062_other_MQ7_CO_detector.h"
#include "sensor_0063_other_MQ9_LPG_flameable_detector.h"
#include "sensor_0057_other_KY026_FlameDetector.h"
#include "sensor_0053_UART_GYGPS6MV2.h"
#include "sensor_0054_PWM_YFS201_flowmeter.h"
#include "sensor_0056_ADC_Force_Sensitive_Resistor.h"

static s_ezlopi_device_v3_t device_array_v3[] = {
#ifdef EZLOPI_SENSOR_0001_LED
    {
        .id = EZLOPI_SENSOR_0001_LED,
        .func = device_0001_digitalOut_generic,
    },
#endif

#ifdef EZLOPI_SENSOR_0002_RELAY
    {
        .id = EZLOPI_SENSOR_0002_RELAY,
        .func = device_0002_digitalOut_relay,
    },
#endif

#ifdef EZLOPI_SENSOR_0003_PLUG
    {
        .id = EZLOPI_SENSOR_0003_PLUG,
        .func = device_0003_digitalOut_plug,
    },
#endif

#ifdef EZLOPI_SENSOR_0004_DIGITALIN_GENERIC
    {
        .id = EZLOPI_SENSOR_0004_DIGITALIN_GENERIC,
        .func = device_0004_digitalIn_generic,
    },
#endif

#ifdef EZLOPI_SENSOR_0008_ALS_LTR303_I2C_SENSOR
    {
        .id = EZLOPI_SENSOR_0008_ALS_LTR303_I2C_SENSOR,
        .func = sensor_0008_I2C_LTR303ALS,
    },
#endif

#ifdef EZLOPI_DEVICE_0009_SK6812_LED_STRIP
    {
        .id = EZLOPI_DEVICE_0009_SK6812_LED_STRIP,
        .func = device_0009_other_RMT_SK6812,
    },
#endif

#ifdef EZLOPI_SENSOR_0010_BME680_I2C
    {
        .id = EZLOPI_SENSOR_0010_BME680_I2C,
        // .func = sensor_0010_I2C_BME680_v3,
    },
#endif

#ifdef EZLOPI_SENSOR_0012_BME280_I2C
    {
        .id = EZLOPI_SENSOR_0012_BME280_I2C,
        .func = sensor_0012_I2C_BME280,
    },
#endif

#ifdef EZLOPI_SENSOR_0019_PIR
    {
        .id = EZLOPI_SENSOR_0019_PIR,
        .func = sensor_0019_digitalIn_PIR,
    },
#endif

#ifdef EZLOPI_SENSOR_0022_DIMMABLE_BULB
    {
        .id = EZLOPI_SENSOR_0022_DIMMABLE_BULB,
        .func = device_0022_PWM_dimmable_lamp,
    },
#endif

#ifdef EZLOPI_SENSOR_0023_TTP_223B_TOUCH_SWITCH
    {
        .id = EZLOPI_SENSOR_0023_TTP_223B_TOUCH_SWITCH,
        .func = sensor_0023_digitalIn_touch_switch_TTP223B,
    },
#endif

#ifdef EZLOPI_SENSOR_034_PROXIMITY_SENSOR
    {
        .id = EZLOPI_SENSOR_034_PROXIMITY_SENSOR,
        .func = sensor_0034_digitalIn_proximity,
    },
#endif

#ifdef EZLOPI_DEVICE_0038_RGB_LED
    {
        .id = EZLOPI_DEVICE_0038_RGB_LED,
        .func = device_0038_other_RGB,
    },
#endif

#ifdef EZLOPI_SENSOR_0035_TPP_32B_TOUCH_SENSOR
    {
        .id = EZLOPI_SENSOR_0035_TPP_32B_TOUCH_SENSOR,
        .func = sensor_0035_digitalIn_touch_sensor_TPP223B,
    },
#endif

#ifdef EZLOPI_SENSOR_0016_DHT22_SENSOR
    {
        .id = EZLOPI_SENSOR_0016_DHT22_SENSOR,
        .func = sensor_0016_oneWire_DHT22,
    },
#endif

#ifdef EZLOPI_SENSOR_0024_ULTRASONIC_HC_SR04_SENSOR
    {
        .id = EZLOPI_SENSOR_0024_ULTRASONIC_HC_SR04_SENSOR,
        .func = sensor_0024_other_HCSR04,
    },
#endif

#ifdef EZLOPI_SENSOR_0025_LDR_DIGITAL_MODULE_SENSOR
    {
        .id = EZLOPI_SENSOR_0025_LDR_DIGITAL_MODULE_SENSOR,
        .func = sensor_0025_digitalIn_LDR,
    },
#endif
#ifdef EZLOPI_SENSOR_0026_LDR_ANALOGUE_SENSOR
    {
        .id = EZLOPI_SENSOR_0026_LDR_ANALOGUE_SENSOR,
        .func = sensor_0026_ADC_LDR,
    },
#endif

#ifdef EZLOPI_SENSOR_0027_WATER_LEAK
    {
        .id = EZLOPI_SENSOR_0027_WATER_LEAK,
        .func = sensor_0027_ADC_waterLeak,
    },
#endif

#ifdef EZLOPI_SENSOR_029_IR_BLASTER
    {
        .id = EZLOPI_SENSOR_029_IR_BLASTER,
        .func = sensor_0029_I2C_GXHTC3,
    },
#endif
#ifdef EZLOPI_SENSOR_030_DS18B20
    {
        .id = EZLOPI_SENSOR_030_DS18B20,
        .func = sensor_0030_oneWire_DS18B20,
    },
#endif

#ifdef EZLOPI_SENSOR_032_SOIL_MOISTURE
    {
        .id = EZLOPI_SENSOR_032_SOIL_MOISTURE,
        .func = sensor_0032_ADC_soilMoisture,
    },
#endif

#ifdef EZLOPI_SENSOR_033_TURBIDITY_SENSOR
    {
        .id = EZLOPI_SENSOR_033_TURBIDITY_SENSOR,
        .func = sensor_0033_ADC_turbidity,
    },
#endif

#ifdef EZLOPI_SENSOR_0044_I2C_TSL2561_LUMINOSITY
    {
        .id = EZLOPI_SENSOR_0044_I2C_TSL2561_LUMINOSITY,
        .func = sensor_0044_I2C_TSL256_luminosity,
    },
#endif

#ifdef EZLOPI_SENSOR_0061_DIGITALIN_REED_SWITCH
    {
        .id = EZLOPI_SENSOR_0061_DIGITALIN_REED_SWITCH,
        .func = sensor_0061_digitalIn_reed_switch,
    },
#endif

#ifdef EZLOPI_SENSOR_0060_DIGITALIN_VIBRATION_DETECTOR
    {
        .id = EZLOPI_SENSOR_0060_DIGITALIN_VIBRATION_DETECTOR,
        .func = sensor_0060_digitalIn_vibration_detector,
    },
#endif

#ifdef EZLOPI_SENSOR_0055_ADC_FLEXRESISTOR
    {
        .id = EZLOPI_SENSOR_0055_ADC_FLEXRESISTOR,
        .func = sensor_0055_ADC_FlexResistor,
    },
#endif

#ifdef EZLOPI_SENSOR_0048_OTHER_MQ4_CH4_DETECTOR
    {
        .id = EZLOPI_SENSOR_0048_OTHER_MQ4_CH4_DETECTOR,
        .func = sensor_0048_other_MQ4_CH4_detector,
    },
#endif

#ifdef EZLOPI_SENSOR_0049_OTHER_MQ2_LPG_DETECTOR
    {
        .id = EZLOPI_SENSOR_0049_OTHER_MQ2_LPG_DETECTOR,
        .func = sensor_0049_other_MQ2_LPG_detector,
    },
#endif

#ifdef EZLOPI_SENSOR_0050_OTHER_MQ3_ALCOHOL_DETECTOR
    {
        .id = EZLOPI_SENSOR_0050_OTHER_MQ3_ALCOHOL_DETECTOR,
        .func = sensor_0050_other_MQ3_alcohol_detector,
    },
#endif

#ifdef EZLOPI_SENSOR_0051_OTHER_MQ8_H2_DETECTOR
    {
        .id = EZLOPI_SENSOR_0051_OTHER_MQ8_H2_DETECTOR,
        .func = sensor_0051_other_MQ8_H2_detector,
    },
#endif

#ifdef EZLOPI_SENSOR_0052_OTHER_MQ135_NH3_DETECTOR
    {
        .id = EZLOPI_SENSOR_0052_OTHER_MQ135_NH3_DETECTOR,
        .func = sensor_0052_other_MQ135_NH3_detector,
    },
#endif

#ifdef EZLOPI_SENSOR_0059_OTHER_MQ6_LPG_DETECTOR
    {
        .id = EZLOPI_SENSOR_0059_OTHER_MQ6_LPG_DETECTOR,
        .func = sensor_0059_other_MQ6_LPG_detector,
    },
#endif

#ifdef EZLOPI_SENSOR_0062_OTHER_MQ7_CO_DETECTOR
    {
        .id = EZLOPI_SENSOR_0062_OTHER_MQ7_CO_DETECTOR,
        .func = sensor_0062_other_MQ7_CO_detector,
    },
#endif

#ifdef EZLOPI_SENSOR_0063_OTHER_MQ9_LPG_FLAMEABLE_DETECTOR
    {
        .id = EZLOPI_SENSOR_0063_OTHER_MQ9_LPG_FLAMEABLE_DETECTOR,
        .func = sensor_0063_other_MQ9_LPG_flameable_detector,
    },
#endif

#ifdef EZLOPI_SENSOR_0057_OTHER_FLAME_DETECTOR
    {
        .id = EZLOPI_SENSOR_0057_OTHER_FLAME_DETECTOR,
        .func = sensor_0057_other_KY026_FlameDetector,
    },
#endif

#ifdef EZLOPI_SENSOR_0053_UART_GYGPS6MV2
    {
        .id = EZLOPI_SENSOR_0053_UART_GYGPS6MV2,
        .func = sensor_0053_UART_GYGPS6MV2,
    },
#endif

#ifdef EZLOPI_SENSOR_0054_PWM_YFS201_FLOWMETER
    {
        .id = EZLOPI_SENSOR_0054_PWM_YFS201_FLOWMETER,
        .func = sensor_0054_PWM_YFS201_flowmeter,
    },
#endif

#ifdef EZLOPI_SENSOR_0056_ADC_FSR
    {
        .id = EZLOPI_SENSOR_0056_ADC_FSR,
        .func = sensor_0056_ADC_Force_Sensitive_Resistor,
    },
#endif

#ifdef EZLOPI_SENSOR_0065_DIGITALIN_FLOAT_SWITCH
    {
        .id = EZLOPI_SENSOR_0065_DIGITALIN_FLOAT_SWITCH,
        .func = sensor_0065_digitalIn_float_switch,
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

#if 0 // version 1.x.x to 2.x.x
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
