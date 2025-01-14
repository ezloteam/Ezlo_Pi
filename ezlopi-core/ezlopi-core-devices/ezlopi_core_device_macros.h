/* ===========================================================================
** Copyright (C) 2025 Ezlo Innovation Inc
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
 * @file    ezlopi_core_device_macros.h
 * @brief   Macros for device-sensors-operations
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */

#ifndef __EZLOPI_CORE_DEVICE_MACROS_H__
#define __EZLOPI_CORE_DEVICE_MACROS_H__

/*******************************************************************************
 *                          Include Files
 *******************************************************************************/

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

#define DEVICE_0001_DIGITALOUT_GENERIC 1
#define DEVICE_0002_DIGITALOUT_RELAY 1
#define DEVICE_0003_DIGITALOUT_PLUG 1
#define DEVICE_0004_DIGITALIN_GENERIC 1
#define DEVICE_0009_OTHER_RMT_SK6812 1
#define DEVICE_0022_PWM_DIMMABLE_LAMP 1
#define DEVICE_0036_PWM_SERVO_MG996R 1
#define DEVICE_0038_OTHER_RGB 1

#define SENSOR_0005_I2C_MPU6050 1
#define SENSOR_0006_I2C_ADXL345 1
#define SENSOR_0007_I2C_GY271 1
#define SENSOR_0008_I2C_LTR303ALS 1
#define SENSOR_0010_I2C_BME680 1
#define SENSOR_0012_I2C_BME280 1
#define SENSOR_0015_ONEWIRE_DHT11 1
#define SENSOR_0016_ONEWIRE_DHT22 1
#define SENSOR_0017_ADC_POTENTIOMETER 1
#define SENSOR_0018_OTHER_INTERNAL_HALL_EFFECT 1
#define SENSOR_0019_DIGITALIN_PIR 1
#define SENSOR_0020_OTHER_2AXIS_JOYSTICK 1
#define SENSOR_0021_UART_MB1013 1
#define SENSOR_0023_DIGITALIN_TOUCH_SWITCH_TTP223B 1
#define SENSOR_0024_OTHER_HCSR04 1
#define SENSOR_0025_DIGITALIN_LDR 1
#define SENSOR_0026_ADC_LDR 1
#define SENSOR_0027_ADC_WATERLEAK 1
#define SENSOR_0028_OTHER_GY61 1
#define SENSOR_0029_I2C_GXHTC3 1
#define SENSOR_0030_ONEWIRE_DS18B20 1
#define SENSOR_0031_OTHER_JSNSR04T 1
#define SENSOR_0032_ADC_SOILMOISTURE 1
#define SENSOR_0033_ADC_TURBIDITY 1
#define SENSOR_0034_DIGITALIN_PROXIMITY 1
#define SENSOR_0035_DIGITALIN_TOUCH_SENSOR_TPP223B 1
#define SENSOR_0037_PMS5003_SENSOR 1
#define SENSOR_0040_OTHER_TCS230 1
#define SENSOR_0041_ADC_FC28_SOILMOISTURE 1
#define SENSOR_0042_ADC_SHUNT_VOLTMETER 1
#define SENSOR_0043_ADC_GYML8511_UV_INTENSITY 1
#define SENSOR_0044_I2C_TSL256_LUMINOSITY 1
#define SENSOR_0046_ADC_ACS712_05B_CURRENTMETER 1
#define SENSOR_0047_OTHER_HX711_LOADCELL 1
#define SENSOR_0048_OTHER_MQ4_CH4_DETECTOR 1
#define SENSOR_0049_OTHER_MQ2_LPG_DETECTOR 1
#define SENSOR_0050_OTHER_MQ3_ALCOHOL_DETECTOR 1
#define SENSOR_0051_OTHER_MQ8_H2_DETECTOR 1
#define SENSOR_0052_OTHER_MQ135_NH3_DETECTOR 1
#define SENSOR_0053_UART_GYGPS6MV2 1
#define SENSOR_0054_PWM_YFS201_FLOWMETER 1
#define SENSOR_0055_ADC_FLEXRESISTOR 1
#define SENSOR_0056_ADC_FORCE_SENSITIVE_RESISTOR 1
#define SENSOR_0057_OTHER_KY026_FLAMEDETECTOR 1
#define SENSOR_0059_OTHER_MQ6_LPG_DETECTOR 1
#define SENSOR_0060_DIGITALIN_VIBRATION_DETECTOR 1
#define SENSOR_0061_DIGITALIN_REED_SWITCH 1
#define SENSOR_0062_OTHER_MQ7_CO_DETECTOR 1
#define SENSOR_0063_OTHER_MQ9_LPG_FLAMEABLE_DETECTOR 1
#define SENSOR_0065_DIGITALIN_FLOAT_SWITCH 1
#define SENSOR_0066_OTHER_R307_FINGERPRINT 1
#define SENSOR_0067_UART_HILINK_PRESENCE_SENSOR 1
#define SENSOR_0068_I2C_ENS160_GAS_SENSOR 1
#define SENSOR_0069_UART_ZE08_CH02_GAS_SENSOR 1
#define SENSOR_0070_ADC_DUMMY_POTENTIOMETER 1

    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif //  __EZLOPI_CORE_DEVICE_MACROS_H__

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
