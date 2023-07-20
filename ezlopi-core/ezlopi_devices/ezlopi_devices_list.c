#include "ezlopi_actions.h"
#include "ezlopi_devices_list.h"
#include "ezlopi_factory_info.h"
#include "device_0001_digitalOut_generic.h"
#include "device_0002_digitalOut_relay.h"
#include "device_0003_digitalOut_plug.h"
// TODO add include for 004
#include "sensor_0005_I2C_MPU6050.h"
#include "sensor_0006_I2C_ADXL345.h"
// TODO Include for 0007 - 0011
#include "sensor_0012_I2C_BME280.h"
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
#include "sensor_0029_I2C_GXHTC3.h"
#include "sensor_0030_oneWire_DS18B20.h"
#include "sensor_0031_other_JSNSR04T.h"
#include "sensor_0032_ADC_soilMoisture.h"
#include "sensor_0035_digitalIn_touch_sensor_TPP223B.h"
#include "device_0036_PWM_servo_MG996R.h"

static s_ezlopi_device_t device_array[] = {

#if (EZLOPI_DEVICE_TYPE == EZLOPI_DEVICE_TYPE_GENERIC)

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
            .func = sensor_0005_I2C_MPU6050
        },
    #endif

    #ifdef EZLOLPI_SENSOR_0006_I2C_ADXL345
        {
            .id = EZLOLPI_SENSOR_0006_I2C_ADXL345,
            .func = sensor_0006_I2C_ADXL345
        },
    #endif

    // #ifdef EZLOLPI_SENSOR_0007_I2C_GY271
    //     {
    //         .id = EZLOLPI_SENSOR_0007_I2C_GY271,
    //         .func = 
    //     },
    // #endif
    // #ifdef EZLOLPI_SENSOR_0008_I2C_LTR303AL
    //     {
    //         .id = EZLOLPI_SENSOR_0008_I2C_LTR303AL,
    //         .func = 
    //     },
    // #endif    
    // #ifdef EZLOLPI_DEVICE_0009_OTHER_RMT_SK6812
    //     {
    //         .id = EZLOLPI_DEVICE_0009_OTHER_RMT_SK6812,
    //         .func = 
    //     },
    // #endif      
    // #ifdef EZLOLPI_SENSOR_0010_I2C_BME680
    //     {
    //         .id = EZLOLPI_SENSOR_0010_I2C_BME680,
    //         .func = 
    //     },
    // #endif       
    // #ifdef EZLOLPI_SENSOR_0011_I2C_MAX30100
    //     {
    //         .id = EZLOLPI_SENSOR_0011_I2C_MAX30100,
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
    //         .func = 
    //     },
    // #endif       
    // #ifdef EZLOPI_SENSOR_0014_UART_PMS5003
    //     {
    //         .id = EZLOPI_SENSOR_0014_UART_PMS5003,
    //         .func = 
    //     },
    // #endif   
    // #ifdef EZLOPI_SENSOR_0015_ONE_WIRE_DHT11
    //     {
    //         .id = EZLOPI_SENSOR_0015_ONE_WIRE_DHT11,
    //         .func = 
    //     },
    // #endif            
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
            .func =  device_0022_PWM_dimmable_lamp, // ezlopi_servo_motor_MG_996R,
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

    // #ifdef EZLOPI_SENSOR_0028_ADC_GY61
    //     {
    //         .id = EZLOPI_SENSOR_0028_ADC_GY61,
    //         .func = ,
    //     },
    // #endif

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
    // #ifdef EZLOPI_SENSOR_0033_ADC_TURBIDITY
    //     {
    //         .id = EZLOPI_SENSOR_0033_ADC_TURBIDITY,
    //         .func = ,
    //     },
    // #endif
    // #ifdef EZLOPI_SENSOR_0034_DIGITAL_IN_PROXIMITY
    //     {
    //         .id = EZLOPI_SENSOR_0034_DIGITAL_IN_PROXIMITY,
    //         .func = ,
    //     },
    // #endif    
    #ifdef EZLOPI_SENSOR_0035_DIGITAL_IN_TPP223B_TOUCH_SENSOR
        {
            .id = EZLOPI_SENSOR_0035_DIGITAL_IN_TPP223B_TOUCH_SENSOR,
            .func = sensor_0035_digitalIn_touch_sensor_TPP223B,
        },
    #endif

#elif (EZLOPI_SWITCH_BOX == EZLOPI_DEVICE_TYPE)
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
#elif (EZLOPI_TEST_DEVICE == EZLOPI_DEVICE_TYPE)
#else
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
