/* ===========================================================================
** Copyright (C) 2024 Ezlo Innovation Inc
**
** Under EZLO AVAILABLE SOURCE LICENSE (EASL) AGREEMENT
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are met:
**
** 1. Redistributions of source code must retain the above copyright notice,
**    this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
** 3. Neither the name of the copyright holder nor the names of its
**    contributors may be used to endorse or promote products derived from
**    this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
** AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
** LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
** INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
** CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
** ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
** POSSIBILITY OF SUCH DAMAGE.
** ===========================================================================
*/

/**
 * @file    main.c
 * @brief   perform some function on data
 * @author  John Doe
 * @version 0.1
 * @date    1st January 2024
 */

#ifndef _EZLOPI_CORE_DEVICES_LIST_INCLUDES_H_
#define _EZLOPI_CORE_DEVICES_LIST_INCLUDES_H_

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/
#include "device_0001_digitalOut_generic.h"
#include "device_0002_digitalOut_relay.h"
#include "device_0003_digitalOut_plug.h"
#include "device_0004_digitalIn_generic.h"

#include "sensor_0005_I2C_MPU6050.h"
#include "sensor_0006_I2C_ADXL345.h"
#include "sensor_0007_I2C_GY271.h"
#include "sensor_0008_I2C_LTR303ALS.h"
#include "device_0009_other_RMT_SK6812.h"
#include "sensor_0010_I2C_BME680.h"
#include "sensor_0012_I2C_BME280.h"
#include "sensor_0015_oneWire_DHT11.h"
#include "sensor_0016_oneWire_DHT22.h"
#include "sensor_0017_ADC_potentiometer.h"

#ifdef CONFIG_IDF_TARGET_ESP32
#include "sensor_0018_other_internal_hall_effect.h"
#endif // CONFIG_IDF_TARGET_ESP32

#include "sensor_0019_digitalIn_PIR.h"
#include "sensor_0020_other_2axis_joystick.h"
#include "sensor_0021_UART_MB1013.h"
#include "device_0022_PWM_dimmable_lamp.h"
#include "sensor_0023_digitalIn_touch_switch_TTP223B.h"
#include "sensor_0024_other_HCSR04.h"
#include "sensor_0025_digitalIn_LDR.h"
#include "sensor_0026_ADC_LDR.h"
#include "sensor_0027_ADC_waterLeak.h"
#include "sensor_0028_other_GY61.h"
#include "sensor_0029_I2C_GXHTC3.h"
#include "sensor_0030_oneWire_DS18B20.h"
#include "sensor_0031_other_JSNSR04T.h"
#include "sensor_0032_ADC_soilMoisture.h"
#include "sensor_0033_ADC_turbidity.h"
#include "sensor_0034_digitalIn_proximity.h"
#include "sensor_0035_digitalIn_touch_sensor_TPP223B.h"
#include "device_0036_PWM_servo_MG996R.h"
#include "sensor_0037_pms5003_sensor.h"
#include "device_0038_other_RGB.h"

#include "sensor_0040_other_TCS230.h"
#include "sensor_0041_ADC_FC28_soilMoisture.h"
#include "sensor_0042_ADC_shunt_voltmeter.h"
#include "sensor_0043_ADC_GYML8511_UV_intensity.h"
#include "sensor_0044_I2C_TSL256_luminosity.h"
#include "sensor_0046_ADC_ACS712_05B_currentmeter.h"
#include "sensor_0047_other_HX711_loadcell.h"
#include "sensor_0048_other_MQ4_CH4_detector.h"
#include "sensor_0049_other_MQ2_LPG_detector.h"
#include "sensor_0050_other_MQ3_alcohol_detector.h"
#include "sensor_0051_other_MQ8_H2_detector.h"
#include "sensor_0052_other_MQ135_NH3_detector.h"
#include "sensor_0053_UART_GYGPS6MV2.h"
#include "sensor_0054_PWM_YFS201_flowmeter.h"
#include "sensor_0055_ADC_FlexResistor.h"
#include "sensor_0056_ADC_Force_Sensitive_Resistor.h"
#include "sensor_0057_other_KY026_FlameDetector.h"

#include "sensor_0059_other_MQ6_LPG_detector.h"
#include "sensor_0060_digitalIn_vibration_detector.h"
#include "sensor_0061_digitalIn_reed_switch.h"
#include "sensor_0062_other_MQ7_CO_detector.h"
#include "sensor_0063_other_MQ9_LPG_flameable_detector.h"
#include "sensor_0065_digitalIn_float_switch.h"
#include "sensor_0066_other_R307_FingerPrint.h"
#include "sensor_0067_hilink_presence_sensor.h"
#include "sensor_0068_ENS160_gas_sensor.h"
#include "sensor_0069_ze08_ch02_gas_sensor.h"

#include "sensor_0070_ADC_dummy_potentiometer.h"

/*******************************************************************************
 *                          C++ Declaration Wrapper
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif

    /*******************************************************************************
     *                          Type & Macro Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif // _EZLOPI_CORE_DEVICES_LIST_INCLUDES_H_

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
