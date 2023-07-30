#include "ezlopi_actions.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_factory_info.h"
#include "device_0001_digitalOut_generic.h"
#include "device_0002_digitalOut_relay.h"
#include "device_0003_digitalOut_plug.h"
// TODO add include for 004
// TODO add include for 004
#include "sensor_0005_I2C_MPU6050.h"
#include "sensor_0006_I2C_ADXL345.h"
#include "sensor_0007_I2C_GY271.h"
#include "sensor_0008_I2C_LTR303ALS.h"
#include "device_0009_other_RMT_SK6812.h"

#if (CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32)
#include "sensor_0010_I2C_BME680.h"
#endif
// TODO Include for 0011
#include "sensor_0012_I2C_BME280.h"
// TODO Include for 0013
#include "sensor_0014_UART_PMS5003.h"
#include "sensor_0015_oneWire_DHT11.h"
#include "sensor_0016_oneWire_DHT22.h"
#include "sensor_0017_ADC_potentiometer.h"
#include "sensor_0018_internal_hall_effect.h"
#include "sensor_0019_digitalIn_PIR.h"
#include "sensor_0020_ADC_2axis_joystick.h"
#include "sensor_0021_UART_MB1013.h"
#include "device_0022_PWM_dimmable_lamp.h"
#include "sensor_0023_digitalIn_touch_switch_TTP223B.h"
#include "sensor_0024_other_HCSR04.h"
#include "sensor_0025_digitalIn_LDR.h"
#include "sensor_0026_ADC_LDR.h"
#include "sensor_0027_ADC_waterLeak.h"
#include "sensor_0028_ADC_GY61.h"
#include "sensor_0029_I2C_GXHTC3.h"
#include "sensor_0030_oneWire_DS18B20.h"
#include "sensor_0031_other_JSNSR04T.h"
#include "sensor_0032_ADC_soilMoisture.h"
#include "sensor_0033_ADC_turbidity.h"
#include "sensor_0034_digitalIn_proximity.h"
#include "sensor_0033_ADC_turbidity.h"
#include "sensor_0034_digitalIn_proximity.h"
#include "sensor_0035_digitalIn_touch_sensor_TPP223B.h"
#include "device_0036_PWM_servo_MG996R.h"
#include "device_0038_digitalOut_RGB.h"

// #include "sensor_0039_I2C_MAX30102.h"
#include "sensor_0040_PWM_TCS230.h"
#include "sensor_0041_ADC_FC28_soilMoisture.h"
#include "sensor_0042_ADC_shunt_voltmeter.h"
#include "sensor_0043_ADC_GYML8511_UV_intensity.h"
#include "sensor_0044_I2C_TSL2561_luminosity.h"
#include "sensor_0046_ADC_ACS712TELC_05B_currentMeter.h"
#include "sensor_0047_other_HX711_loadcell.h"
#include "sensor_0048_other_MQ4_methane_gas_detector.h"
#include "sensor_0049_other_MQ2_LPG_gas_detector.h"
#include "sensor_0050_other_MQ3_Alcohol_detector.h"
#include "sensor_0051_other_MQ8_H2_gas_detector.h"
#include "sensor_0052_other_MQ135_NH3_gas_detector.h"

static s_ezlopi_device_t device_array[] = {

#if ((EZLOPI_DEVICE_TYPE == EZLOPI_DEVICE_TYPE_GENERIC) || (EZLOPI_DEVICE_TYPE == EZLOPI_DEVICE_TYPE_TEST_DEVICE))

#ifdef EZLOPI_DEVICE_0001_DIGITAL_OUT_LED
    {
        .id = EZLOPI_DEVICE_0001_DIGITAL_OUT_LED,
        .func = device_0001_digitalOut_generic,
    },
#endif

#ifdef EZLOPI_DEVICE_0002_DIGITAL_OUT_RELAY
    {
        .id = EZLOPI_DEVICE_0002_DIGITAL_OUT_RELAY,
        .func = device_0002_digitalOut_relay,
    },
#endif

#ifdef EZLOPI_DEVICE_0003_DIGITAL_OUT_PLUG
    {
        .id = EZLOPI_DEVICE_0003_DIGITAL_OUT_PLUG,
        .func = device_0003_digitalOut_plug,
    },
#endif

#ifdef EZLOPI_SENSOR_0004_DIGITAL_IN_SWITCH
    {
        .id = EZLOPI_SENSOR_0004_DIGITAL_IN_SWITCH,
        .func = device_0001_digitalOut_generic,
    },
#endif

#ifdef EZLOPI_SENSOR_0005_I2C_MPU6050
    {
        .id = EZLOPI_SENSOR_0005_I2C_MPU6050,
        .func = sensor_0005_I2C_MPU6050,
    },
#endif

#ifdef EZLOPI_SENSOR_0006_I2C_ADXL345
    {.id = EZLOPI_SENSOR_0006_I2C_ADXL345,
     .func = sensor_0006_I2C_ADXL345},
#endif

#ifdef EZLOPI_SENSOR_0007_I2C_GY271
    {
        .id = EZLOPI_SENSOR_0007_I2C_GY271,
        .func = sensor_0007_I2C_GY271,
    },
#endif

#ifdef EZLOPI_SENSOR_0008_I2C_LTR303AL
    {
        .id = EZLOPI_SENSOR_0008_I2C_LTR303AL,
        .func = sensor_0008_I2C_LTR303ALS,
    },
#endif

#ifdef EZLOPI_DEVICE_0009_OTHER_RMT_SK6812
    {
        .id = EZLOPI_DEVICE_0009_OTHER_RMT_SK6812,
        .func = device_0009_other_RMT_SK6812,
    },
#endif

#ifdef EZLOPI_SENSOR_0010_I2C_BME680
    {
        .id = EZLOPI_SENSOR_0010_I2C_BME680,
        .func = sensor_0010_I2C_BME680,
    },

#endif

// #ifdef EZLOPI_SENSOR_0011_I2C_MAX30100
//     {
//         .id = EZLOPI_SENSOR_0011_I2C_MAX30100,
//         .func =
//     },
// #endif

#ifdef EZLOPI_SENSOR_0012_I2C_BME280
    {
        .id = EZLOPI_SENSOR_0012_I2C_BME280,
        .func = sensor_0012_I2C_BME280,
    },
#endif

// #ifdef EZLOPI_SENSOR_0013_SPI_BME280
//     {
//         .id = EZLOPI_SENSOR_0013_SPI_BME280,
//         .func = ,
//     },
// #endif

#ifdef EZLOPI_SENSOR_0014_UART_PMS5003
    {
        .id = EZLOPI_SENSOR_0014_UART_PMS5003,
        .func = sensor_0014_UART_PMS5003,
    },
#endif

#ifdef EZLOPI_SENSOR_0015_ONE_WIRE_DHT11
    {
        .id = EZLOPI_SENSOR_0015_ONE_WIRE_DHT11,
        .func = sensor_0015_oneWire_DHT11,
    },
#endif

#ifdef EZLOPI_SENSOR_0016_ONE_WIRE_DHT22
    {
        .id = EZLOPI_SENSOR_0016_ONE_WIRE_DHT22,
        .func = sensor_0016_oneWire_DHT22,
    },
#endif

#ifdef EZLOPI_SENSOR_0017_ADC_POTENTIOMETER
    {
        .id = EZLOPI_SENSOR_0017_ADC_POTENTIOMETER,
        .func = potentiometer,
    },
#endif

#ifdef EZLOPI_SENSOR_0018_OTHER_INTERNAL_HALL_EFFECT
    {
        .id = EZLOPI_SENSOR_0018_OTHER_INTERNAL_HALL_EFFECT,
        .func = door_hall_sensor,
    },
#endif

#ifdef EZLOPI_SENSOR_0019_DIGITAL_INPUT_PIR
    {
        .id = EZLOPI_SENSOR_0019_DIGITAL_INPUT_PIR,
        .func = sensor_0019_digitalIn_PIR,
    },
#endif

#ifdef EZLOPI_SENSOR_0020_ADC_JOYSTICK_2_AXIS
    {
        .id = EZLOPI_SENSOR_0020_ADC_JOYSTICK_2_AXIS,
        .func = sensor_0020_ADC_2axis_joystick,
    },
#endif

#ifdef EZLOPI_SENSOR_0021_UART_MB1013
    {
        .id = EZLOPI_SENSOR_0021_UART_MB1013,
        .func = sensor_0021_UART_MB1013,
    },
#endif

#ifdef EZLOPI_DEVICE_0022_PWM_DIMMABLE_BULB
    {
        .id = EZLOPI_DEVICE_0022_PWM_DIMMABLE_BULB,
        .func = device_0022_PWM_dimmable_lamp, // ezlopi_servo_motor_MG_996R,
    },
#endif

#ifdef EZLOPI_SENSOR_0023_DIGITAL_IN_TTP223B_TOUCH_SWITCH
    {
        .id = EZLOPI_SENSOR_0023_DIGITAL_IN_TTP223B_TOUCH_SWITCH,
        .func = sensor_0023_digitalIn_touch_switch_TTP223B,
    },
#endif

#ifdef EZLOPI_SENSOR_0024_OTHER_HCSR04
    {
        .id = EZLOPI_SENSOR_0024_OTHER_HCSR04,
        .func = sensor_0024_other_HCSR04,
    },
#endif

#ifdef EZLOPI_SENSOR_0025_DIGITAL_IN_LDR_DIGITAL
    {
        .id = EZLOPI_SENSOR_0025_DIGITAL_IN_LDR_DIGITAL,
        .func = sensor_0025_digitalIn_LDR,
    },
#endif

#ifdef EZLOPI_SENSOR_0026_ADC_LDR
    {
        .id = EZLOPI_SENSOR_0026_ADC_LDR,
        .func = sensor_0026_ADC_LDR,
    },
#endif

#ifdef EZLOPI_SENSOR_0027_ADC_WATER_LEAK
    {
        .id = EZLOPI_SENSOR_0027_ADC_WATER_LEAK,
        .func = sensor_0027_ADC_waterLeak,
    },
#endif

#ifdef EZLOPI_SENSOR_0028_ADC_GY61
    {
        .id = EZLOPI_SENSOR_0028_ADC_GY61,
        .func = sensor_0028_ADC_GY61,
    },
#endif

#ifdef EZLOPI_SENSOR_0029_I2C_GXHTC3
    {
        .id = EZLOPI_SENSOR_0029_I2C_GXHTC3,
        .func = sensor_0029_I2C_GXHTC3,
    },
#endif

#ifdef EZLOPI_SENSOR_0030_ONE_WIRE_DS18B20
    {
        .id = EZLOPI_SENSOR_0030_ONE_WIRE_DS18B20,
        .func = sensor_0030_oneWire_DS18B20,
    },
#endif

#ifdef EZLOPI_SENSOR_0031_OTHER_JSNSR04T
    {
        .id = EZLOPI_SENSOR_0031_OTHER_JSNSR04T,
        .func = sensor_0031_other_JSNSR04T,
    },
#endif

#ifdef EZLOPI_SENSOR_0032_ADC_SOIL_MOISTURE
    {
        .id = EZLOPI_SENSOR_0032_ADC_SOIL_MOISTURE,
        .func = sensor_0032_ADC_soilMoisture,
    },
#endif

#ifdef EZLOPI_SENSOR_0033_ADC_TURBIDITY
    {
        .id = EZLOPI_SENSOR_0033_ADC_TURBIDITY,
        .func = sensor_0033_ADC_turbidity,
    },
#endif

#ifdef EZLOPI_SENSOR_0034_DIGITAL_IN_PROXIMITY
    {
        .id = EZLOPI_SENSOR_0034_DIGITAL_IN_PROXIMITY,
        .func = sensor_0034_digitalIn_proximity,
    },
#endif

#ifdef EZLOPI_SENSOR_0035_DIGITAL_IN_TPP223B_TOUCH_SENSOR
    {
        .id = EZLOPI_SENSOR_0035_DIGITAL_IN_TPP223B_TOUCH_SENSOR,
        .func = sensor_0035_digitalIn_touch_sensor_TPP223B,
    },
#endif

#ifdef EZLOPI_DEVICE_0036_PWM_SERVO_MG996R
    {
        .id = EZLOPI_DEVICE_0036_PWM_SERVO_MG996R,
        .func = device_0036_PWM_servo_MG996R,
    },
#endif

#ifdef EZLOPI_SENSOR_0037_DIGITAL_OUTPUT_PMS5003_GPIO
    {
        .id = EZLOPI_SENSOR_0037_DIGITAL_OUTPUT_PMS5003_GPIO,
        .func = sensor_0014_UART_PMS5003,
    },
#endif

#ifdef EZLOPI_DEVICE_0038_DIGITAL_OUTPUT_RGB_LED
    {
        .id = EZLOPI_DEVICE_0038_DIGITAL_OUTPUT_RGB_LED,
        .func = device_0038_digitalOut_RGB,
    },
#endif

// #ifdef EZLOPI_SENSOR_0039_I2C_MAX30102
//     {
//         .id = EZLOPI_SENSOR_0039_I2C_MAX30102,
//         .func = sensor_0039_I2C_MAX30102,
//     },
// #endif

#ifdef EZLOPI_SENSOR_0040_PWM_TCS230
    {
        .id = EZLOPI_SENSOR_0040_PWM_TCS230,
        .func = sensor_0040_PWM_TCS230,
    },
#endif

#ifdef EZLOPI_SENSOR_0041_ADC_FC28_soilMoisture
    {
        .id = EZLOPI_SENSOR_0041_ADC_FC28_soilMoisture,
        .func = sensor_0041_ADC_FC28_soilMoisture,
    },
#endif

#ifdef EZLOPI_SENSOR_0042_ADC_shunt_voltmeter
    {
        .id = EZLOPI_SENSOR_0042_ADC_shunt_voltmeter,
        .func = sensor_0042_ADC_shunt_voltmeter,
    },
#endif

#ifdef EZLOPI_SENSOR_0043_ADC_UV_INTENSITY
    {
        .id = EZLOPI_SENSOR_0043_ADC_UV_INTENSITY,
        .func = sensor_0043_adc_gyml8511_uv_intensity,
    },
#endif

#ifdef EZLOPI_SENSOR_0044_I2C_TSL2561_LUMINOSITY
    {
        .id = EZLOPI_SENSOR_0044_I2C_TSL2561_LUMINOSITY,
        .func = sensor_0044_I2C_TSL2561_luminosity,
    },
#endif

// need to include 'sensor_0045'

#ifdef EZLOPI_SENSOR_0046_ADC_ACS712TELC_05B_CURRENTMETER
    {
        .id = EZLOPI_SENSOR_0046_ADC_ACS712TELC_05B_CURRENTMETER,
        .func = sensor_0046_ADC_ACS712TELC_05B_currentMeter,
    },
#endif

#ifdef EZLOPI_SENSOR_0047_OTHER_HX711_LOADCELL
    {
        .id = EZLOPI_SENSOR_0047_OTHER_HX711_LOADCELL,
        .func = sensor_0047_other_hx711,
    },
#endif

#ifdef EZLOPI_SENSOR_0048_OTHER_MQ4_METHANE_GAS_DETECTOR
    {
        .id = EZLOPI_SENSOR_0048_OTHER_MQ4_METHANE_GAS_DETECTOR,
        .func = sensor_0048_MQ4_methane,
    },
#endif

#ifdef EZLOPI_SENSOR_0049_OTHER_MQ2_LPG_GAS_DETECTOR
    {
        .id = EZLOPI_SENSOR_0049_OTHER_MQ2_LPG_GAS_DETECTOR,
        .func = sensor_0049_MQ2_LPG,
    },
#endif

#ifdef EZLOPI_SENSOR_0050_OTHER_MQ3_ALCOHOL_GAS_DETECTOR
    {
        .id = EZLOPI_SENSOR_0050_OTHER_MQ3_ALCOHOL_GAS_DETECTOR,
        .func = sensor_0050_MQ3_alcohol,
    },
#endif

#ifdef EZLOPI_SENSOR_0051_OTHER_MQ8_HYDROGEN_GAS_DETECTOR
    {
        .id = EZLOPI_SENSOR_0051_OTHER_MQ8_HYDROGEN_GAS_DETECTOR,
        .func = sensor_0051_MQ8_H2,
    },

#endif

#ifdef EZLOPI_SENSOR_0052_OTHER_MQ135_AMONIA_GAS_DETECTOR
    {
        .id = EZLOPI_SENSOR_0052_OTHER_MQ135_AMONIA_GAS_DETECTOR,
        .func = sensor_0052_MQ135_NH3,
    },
#endif

#elif (EZLOPI_DEVICE_TYPE_SWITCH_BOX == EZLOPI_DEVICE_TYPE)
#ifdef EZLOPI_DEVICE_0001_DIGITAL_OUT_LED
    {
        .id = EZLOPI_DEVICE_0001_DIGITAL_OUT_LED,
        .func = device_0001_digitalOut_generic,
    },
#endif
#ifdef EZLOPI_SENSOR_0029_I2C_GXHTC3
    {
        .id = EZLOPI_SENSOR_0029_I2C_GXHTC3,
        .func = sensor_0029_I2C_GXHTC3,
    },
#endif
#elif (EZLOPI_DEVICE_TYPE_AMBIENT_TRACKER_PRO == EZLOPI_DEVICE_TYPE)
// #ifdef EZLOPI_DEVICE_0009_OTHER_RMT_SK6812
//     {
//         .id = EZLOPI_DEVICE_0009_OTHER_RMT_SK6812,
//         .func =
//     },
// #endif
#ifdef EZLOPI_SENSOR_0010_I2C_BME680
    {.id = EZLOPI_SENSOR_0010_I2C_BME680,
     .func = sensor_0010_I2C_BME680},
#endif
// #ifdef EZLOPI_SENSOR_0014_UART_PMS5003
//     {
//         .id = EZLOPI_SENSOR_0014_UART_PMS5003,
//         .func = sensor_0014_UART_PMS5003
//     },
// #endif
// #ifdef EZOPI_SENSOR_0008_I2C_LTR303AL
//     {
//         .id = EZLOPI_SENSOR_0008_I2C_LTR303AL,
//         .func =
//     },
// #endif
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

s_ezlopi_device_t *ezlopi_devices_list_get_list(void)
{
    return device_array;
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
