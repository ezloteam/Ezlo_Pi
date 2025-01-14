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
 * @file    ezlopi_core_devices_list_modules.h
 * @brief   Macros to list device modeles
 * @author  ezlopi_team_np
 * @version 0.1
 * @date    12th DEC 2024
 */

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

#if (1 == DEVICE_0001_DIGITALOUT_GENERIC)
    EZLOPI_DEVICE_LIST("digital-out generic                 ", 1, DEVICE_0001_digitalOut_generic)
#endif // DEVICE_0001_DIGITALOUT_GENERIC

#if (1 == DEVICE_0001_DIGITALOUT_GENERIC)
    EZLOPI_DEVICE_LIST("digital-out relay                   ", 2, DEVICE_0002_digitalOut_relay)
#endif // DEVICE_0001_DIGITALOUT_GENERIC

#if (1 == DEVICE_0001_DIGITALOUT_GENERIC)
    EZLOPI_DEVICE_LIST("digital-out plug                    ", 3, DEVICE_0003_digitalOut_plug)
#endif // DEVICE_0001_DIGITALOUT_GENERIC

#if (1 == DEVICE_0001_DIGITALOUT_GENERIC)
    EZLOPI_DEVICE_LIST("digital-in generic                  ", 4, DEVICE_0004_digitalIn_generic)
#endif // DEVICE_0001_DIGITALOUT_GENERIC

#if (1 == SENSOR_0005_I2C_MPU6050)
    EZLOPI_DEVICE_LIST("sensor-MPU6050-I2C                  ", 5, SENSOR_0005_i2c_mpu6050)
#endif // SENSOR_0005_I2C_MPU6050

#if (1 == SENSOR_0006_I2C_ADXL345)
    EZLOPI_DEVICE_LIST("sensor-ADXL345-I2C                  ", 6, SENSOR_0006_i2c_adxl345)
#endif // SENSOR_0006_I2C_ADXL345

#if (1 == SENSOR_0007_I2C_GY271)
    EZLOPI_DEVICE_LIST("sensor-GY271-I2C                    ", 7, SENSOR_0007_i2c_gy271)
#endif // SENSOR_0007_I2C_GY271

#if (1 == SENSOR_0008_I2C_LTR303ALS)
    EZLOPI_DEVICE_LIST("sensor-LTR303ALS-I2C                ", 8, SENSOR_0008_i2c_ltr303als)
#endif // SENSOR_0008_I2C_LTR303ALS

#if (1 == DEVICE_0009_OTHER_RMT_SK6812)
    EZLOPI_DEVICE_LIST("device-rgp_strip-RMT_SK6812         ", 9, DEVICE_0009_other_rmt_sk6812)
#endif // DEVICE_0009_OTHER_RMT_SK6812

#if (1 == SENSOR_0010_I2C_BME680)
#if (CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)
    EZLOPI_DEVICE_LIST("sensor-bme680-i2c                   ", 10, SENSOR_0010_i2c_bme680)
#endif // CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2
#endif // SENSOR_0010_I2C_BME680

#if (1 == SENSOR_0012_I2C_BME280)
    EZLOPI_DEVICE_LIST("sensor-bme280-i2c-temp-humid        ", 12, SENSOR_0012_i2c_bme280)
#endif // SENSOR_0012_I2C_BME280

#if (CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)

#if (1 == SENSOR_0015_ONEWIRE_DHT11)
    EZLOPI_DEVICE_LIST("sensor-DHT11-1-wire-temp-humid      ", 15, SENSOR_0015_oneWire_dht11)
#endif // SENSOR_0015_ONEWIRE_DHT11

#if (1 == SENSOR_0016_ONEWIRE_DHT22)
    EZLOPI_DEVICE_LIST("sensor-DHT22-1-wire-temp-humid      ", 16, SENSOR_0016_oneWire_dht22)
#endif // SENSOR_0016_ONEWIRE_DHT22

#endif // CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2

#if (1 == SENSOR_0017_ADC_POTENTIOMETER)
    EZLOPI_DEVICE_LIST("sensor-potentiometer-ADC            ", 17, SENSOR_0017_adc_potentiometer)
#endif // SENSOR_0017_ADC_POTENTIOMETER

#ifdef CONFIG_IDF_TARGET_ESP32
#if (1 == SENSOR_0018_OTHER_INTERNAL_HALL_EFFECT)
    EZLOPI_DEVICE_LIST("sensor-Hall-effect                  ", 18, SENSOR_0018_other_internal_hall_effect)
#endif // SENSOR_0018_OTHER_INTERNAL_HALL_EFFECT
#endif // CONFIG_IDF_TARGET_ESP32

#if (1 == SENSOR_0019_DIGITALIN_PIR)
    EZLOPI_DEVICE_LIST("sensor-PIR-digital-proximity        ", 19, SENSOR_0019_digitalIn_PIR)
#endif // SENSOR_0019_DIGITALIN_PIR

#if (1 == SENSOR_0020_OTHER_2AXIS_JOYSTICK)
    EZLOPI_DEVICE_LIST("sensor-2AXIS-JOYSTICK               ", 20, SENSOR_0020_other_2axis_joystick)
#endif // SENSOR_0020_OTHER_2AXIS_JOYSTICK

#if (1 == SENSOR_0021_UART_MB1013)
    EZLOPI_DEVICE_LIST("sensor-UART-MB1013                  ", 21, SENSOR_0021_uart_mb1013)
#endif // SENSOR_0021_UART_MB1013

#if (1 == DEVICE_0022_PWM_DIMMABLE_LAMP)
    EZLOPI_DEVICE_LIST("device-PWM-dimmable-lamp            ", 22, DEVICE_0022_pwm_dimmable_lamp)
#endif // DEVICE_0022_PWM_DIMMABLE_LAMP

#if (1 == SENSOR_0023_DIGITALIN_TOUCH_SWITCH_TTP223B)
    EZLOPI_DEVICE_LIST("sensor-TTP223B-digital-touch        ", 23, SENSOR_0023_digitalIn_touch_switch_TTP223B)
#endif // SENSOR_0023_DIGITALIN_TOUCH_SWITCH_TTP223B

#if (1 == SENSOR_0024_OTHER_HCSR04)
    EZLOPI_DEVICE_LIST("sensor-HCSR04-pulse-width           ", 24, SENSOR_0024_other_HCSR04_v3)
#endif // SENSOR_0024_OTHER_HCSR04

#if (1 == SENSOR_0025_DIGITALIN_LDR)
    EZLOPI_DEVICE_LIST("sensor-LDR-digital-in               ", 25, SENSOR_0025_digitalIn_ldr)
#endif // SENSOR_0025_DIGITALIN_LDR

#if (1 == SENSOR_0026_ADC_LDR)
    EZLOPI_DEVICE_LIST("sensor-LDR-ADC                      ", 26, SENSOR_0026_adc_ldr)
#endif // SENSOR_0026_ADC_LDR

#if (1 == SENSOR_0027_ADC_WATERLEAK)
    EZLOPI_DEVICE_LIST("sensor-WATERLEAK-ADC                ", 27, SENSOR_0027_adc_waterLeak)
#endif // SENSOR_0027_ADC_WATERLEAK

#if (1 == SENSOR_0028_OTHER_GY61)
    EZLOPI_DEVICE_LIST("sensor-GY61-other-accelerometer     ", 28, SENSOR_0028_other_gy61)
#endif // SENSOR_0028_OTHER_GY61

#if (1 == SENSOR_0029_I2C_GXHTC3)
    EZLOPI_DEVICE_LIST("sensor-GXHTC3-I2C-temp-humidity     ", 29, SENSOR_0029_i2c_gxhtc3)
#endif // SENSOR_0029_I2C_GXHTC3

#if (1 == SENSOR_0030_ONEWIRE_DS18B20)
    EZLOPI_DEVICE_LIST("sensor-DS18B20-1-wire-temp          ", 30, SENSOR_0030_oneWire_DS18B20)
#endif // SENSOR_0030_ONEWIRE_DS18B20

#if (1 == SENSOR_0031_OTHER_JSNSR04T)
    EZLOPI_DEVICE_LIST("sensor-JNSR04T-digital-distance     ", 31, SENSOR_0031_other_jsnsr04t)
#endif // SENSOR_0031_OTHER_JSNSR04T

#if (1 == SENSOR_0032_ADC_SOILMOISTURE)
    EZLOPI_DEVICE_LIST("sensor-soilMoisture-ADC             ", 32, SENSOR_0032_adc_soilMoisture)
#endif // SENSOR_0032_ADC_SOILMOISTURE

#if (1 == SENSOR_0033_ADC_TURBIDITY)
    EZLOPI_DEVICE_LIST("sensor-turbidity-ADC                ", 33, SENSOR_0033_adc_turbidity)
#endif // SENSOR_0033_ADC_TURBIDITY

#if (1 == SENSOR_0034_DIGITALIN_PROXIMITY)
    EZLOPI_DEVICE_LIST("sensor-proximity-digital            ", 34, SENSOR_0034_digitalIn_proximity)
#endif // SENSOR_0034_DIGITALIN_PROXIMITY

#if (1 == SENSOR_0035_DIGITALIN_TOUCH_SENSOR_TPP223B)
    EZLOPI_DEVICE_LIST("sensor-TPP223B-digital-touch        ", 35, SENSOR_0035_digitalIn_touch_sensor_tpp223b)
#endif // SENSOR_0035_DIGITALIN_TOUCH_SENSOR_TPP223B

#if (1 == DEVICE_0036_PWM_SERVO_MG996R)
    EZLOPI_DEVICE_LIST("device-PWM-servo-MG996R             ", 36, DEVICE_0036_pwm_servo_mg996r)
#endif // DEVICE_0036_PWM_SERVO_MG996R

#if (1 == SENSOR_0037_PMS5003_SENSOR)
    EZLOPI_DEVICE_LIST("sensor-PMS5003                      ", 37, SENSOR_pms5003_v3)
#endif // SENSOR_0037_PMS5003_SENSOR

#if (1 == DEVICE_0038_OTHER_RGB)
    EZLOPI_DEVICE_LIST("device-other-RGB-LED                ", 38, DEVICE_0038_other_rgb)
#endif // DEVICE_0038_OTHER_RGB

#if (1 == SENSOR_0040_OTHER_TCS230)
    EZLOPI_DEVICE_LIST("sensor-TCS230-other-color           ", 40, SENSOR_0040_other_tcs230)
#endif // SENSOR_0040_OTHER_TCS230

#if (1 == SENSOR_0041_ADC_FC28_SOILMOISTURE)
    EZLOPI_DEVICE_LIST("sensor-FC28-soilMoisture-ADC        ", 41, SENSOR_0041_adc_fc28_soilMoisture)
#endif // SENSOR_0041_ADC_FC28_SOILMOISTURE

#if (1 == SENSOR_0042_ADC_SHUNT_VOLTMETER)
    EZLOPI_DEVICE_LIST("sensor-shunt-voltmeter-ADC          ", 42, SENSOR_0042_adc_shunt_voltmeter)
#endif // SENSOR_0042_ADC_SHUNT_VOLTMETER

#if (1 == SENSOR_0043_ADC_GYML8511_UV_INTENSITY)
    EZLOPI_DEVICE_LIST("sensor-GYML8511-ADC-intensity       ", 43, SENSOR_0043_adc_gyml8511_uv_intensity)
#endif // SENSOR_0043_ADC_GYML8511_UV_INTENSITY

#if (1 == SENSOR_0044_I2C_TSL256_LUMINOSITY)
    EZLOPI_DEVICE_LIST("sensor-TSL256-I2C-luminosity        ", 44, SENSOR_0044_i2c_tsl256_luminosity)
#endif // SENSOR_0044_I2C_TSL256_LUMINOSITY

#if (1 == SENSOR_0046_ADC_ACS712_05B_CURRENTMETER)
    EZLOPI_DEVICE_LIST("sensor-ACS712-currentmeter-ADC      ", 46, SENSOR_0046_adc_acs712_05b_currentmeter)
#endif // SENSOR_0046_ADC_ACS712_05B_CURRENTMETER

#if (1 == SENSOR_0047_OTHER_HX711_LOADCELL)
    EZLOPI_DEVICE_LIST("sensor-HX711-loadcell               ", 47, SENSOR_0047_other_hx711_loadcell)
#endif // SENSOR_0047_OTHER_HX711_LOADCELL

#if (1 == SENSOR_0048_OTHER_MQ4_CH4_DETECTOR)
    EZLOPI_DEVICE_LIST("sensor-MQ4-CH4-methane              ", 48, SENSOR_0048_other_mq4_ch4_detector)
#endif // SENSOR_0048_OTHER_MQ4_CH4_DETECTOR

#if (1 == SENSOR_0049_OTHER_MQ2_LPG_DETECTOR)
    EZLOPI_DEVICE_LIST("sensor-MQ2-LPG-detector             ", 49, SENSOR_0049_other_mq2_lpg_detector)
#endif // SENSOR_0049_OTHER_MQ2_LPG_DETECTOR

#if (1 == SENSOR_0050_OTHER_MQ3_ALCOHOL_DETECTOR)
    EZLOPI_DEVICE_LIST("sensor-MQ3-alcohol-detector         ", 50, SENSOR_0050_other_mq3_alcohol_detector)
#endif // SENSOR_0050_OTHER_MQ3_ALCOHOL_DETECTOR

#if (1 == SENSOR_0051_OTHER_MQ8_H2_DETECTOR)
    EZLOPI_DEVICE_LIST("sensor-MQ8-H2-detector              ", 51, SENSOR_0051_other_mq8_h2_detector)
#endif // SENSOR_0051_OTHER_MQ8_H2_DETECTOR

#if (1 == SENSOR_0052_OTHER_MQ135_NH3_DETECTOR)
    EZLOPI_DEVICE_LIST("sensor-MQ135-NH3-detector           ", 52, SENSOR_0052_other_mq135_nh3_detector)
#endif // SENSOR_0052_OTHER_MQ135_NH3_DETECTOR

#if (1 == SENSOR_0053_UART_GYGPS6MV2)
    EZLOPI_DEVICE_LIST("sensor-GYGPS6MV2-UART-gps           ", 53, SENSOR_0053_uart_gygps6mv2)
#endif // SENSOR_0053_UART_GYGPS6MV2

#if (1 == SENSOR_0054_PWM_YFS201_FLOWMETER)
    EZLOPI_DEVICE_LIST("sensor-YFS201-PWM-flowmeter         ", 54, SENSOR_0054_pwm_yfs201_flowmeter)
#endif // SENSOR_0054_PWM_YFS201_FLOWMETER

#if (1 == SENSOR_0055_ADC_FLEXRESISTOR)
    EZLOPI_DEVICE_LIST("sensor-Flex-Resistor-ADC            ", 55, SENSOR_0055_adc_flexresistor)
#endif // SENSOR_0055_ADC_FLEXRESISTOR

#if (1 == SENSOR_0056_ADC_FORCE_SENSITIVE_RESISTOR)
    EZLOPI_DEVICE_LIST("sensor-Force-sensetive-resistor-ADC ", 56, SENSOR_0056_adc_force_sensitive_resistor)
#endif // SENSOR_0056_ADC_FORCE_SENSITIVE_RESISTOR

#if (1 == SENSOR_0057_OTHER_KY026_FLAMEDETECTOR)
    EZLOPI_DEVICE_LIST("sensor-KY026-Flame-detector         ", 57, SENSOR_0057_other_ky026_flamedetector)
#endif // SENSOR_0057_OTHER_KY026_FLAMEDETECTOR

#if (1 == SENSOR_0059_OTHER_MQ6_LPG_DETECTOR)
    EZLOPI_DEVICE_LIST("sensor-MQ6-LPG-detector             ", 59, SENSOR_0059_other_mq6_lpg_detector)
#endif // SENSOR_0059_OTHER_MQ6_LPG_DETECTOR

#if (1 == SENSOR_0060_DIGITALIN_VIBRATION_DETECTOR)
    EZLOPI_DEVICE_LIST("sensor-vibration-detector-Digital   ", 60, SENSOR_0060_digitalIn_vibration_detector)
#endif // SENSOR_0060_DIGITALIN_VIBRATION_DETECTOR

#if (1 == SENSOR_0061_DIGITALIN_REED_SWITCH)
    EZLOPI_DEVICE_LIST("sensor-reed-switch-digital          ", 61, SENSOR_0061_digitalIn_reed_switch)
#endif // SENSOR_0061_DIGITALIN_REED_SWITCH

#if (1 == SENSOR_0062_OTHER_MQ7_CO_DETECTOR)
    EZLOPI_DEVICE_LIST("sensor-MQ7-CO-detector              ", 62, SENSOR_0062_other_mq7_co_detector)
#endif // SENSOR_0062_OTHER_MQ7_CO_DETECTOR

#if (1 == SENSOR_0063_OTHER_MQ9_LPG_FLAMEABLE_DETECTOR)
    EZLOPI_DEVICE_LIST("sensor-MQ9-LPG-flameable-detector   ", 63, SENSOR_0063_other_mq9_lpg_flameable_detector)
#endif // SENSOR_0063_OTHER_MQ9_LPG_FLAMEABLE_DETECTOR

#if (1 == SENSOR_0065_DIGITALIN_FLOAT_SWITCH)
    EZLOPI_DEVICE_LIST("sensor-float-switch-digital         ", 65, SENSOR_0065_digitalIn_float_switch)
#endif // SENSOR_0065_DIGITALIN_FLOAT_SWITCH

#if (1 == SENSOR_0066_OTHER_R307_FINGERPRINT)
    EZLOPI_DEVICE_LIST("sensor-fingerprint-sensor           ", 66, SENSOR_0066_other_r307_fingerprint)
#endif // SENSOR_0066_OTHER_R307_FINGERPRINT

#if (1 == SENSOR_0067_UART_HILINK_PRESENCE_SENSOR)
    EZLOPI_DEVICE_LIST("sensor-hilink-presence-sensor       ", 67, SENSOR_0067_hilink_presence_sensor_v3)
#endif // SENSOR_0067_UART_HILINK_PRESENCE_SENSOR

#if (1 == SENSOR_0068_I2C_ENS160_GAS_SENSOR)
    EZLOPI_DEVICE_LIST("sensor-ENS160-gas-sensor            ", 68, SENSOR_0068_ens160_gas_sensor)
#endif // SENSOR_0068_I2C_ENS160_GAS_SENSOR

#if (1 == SENSOR_0069_UART_ZE08_CH02_GAS_SENSOR)
    EZLOPI_DEVICE_LIST("sensor-ze08-ch02-gas-sensor         ", 69, SENSOR_0069_ze08_ch02_gas_sensor)
#endif // SENSOR_0069_UART_ZE08_CH02_GAS_SENSOR

#if (1 == SENSOR_0070_ADC_DUMMY_POTENTIOMETER)
    EZLOPI_DEVICE_LIST("sensor-dummy-potentiometer-ADC      ", 70, SENSOR_0070_adc_dummy_potentiometer)
#endif // SENSOR_0070_ADC_DUMMY_POTENTIOMETER

    //////// End of sensor/device list
    EZLOPI_DEVICE_LIST(NULL, 0, NULL)
    /*******************************************************************************
     *                          Extern Data Declarations
     *******************************************************************************/

    /*******************************************************************************
     *                          Extern Function Prototypes
     *******************************************************************************/

#ifdef __cplusplus
}
#endif

/*******************************************************************************
 *                          End of File
 *******************************************************************************/
