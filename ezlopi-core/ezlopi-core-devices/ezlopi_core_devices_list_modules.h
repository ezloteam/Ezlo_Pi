
#if (1 == DEVICE_0001_DIGITALOUT_GENERIC)
EZLOPI_DEVICE_LIST("digital-out generic                 ", 1, device_0001_digitalOut_generic)
#endif // DEVICE_0001_DIGITALOUT_GENERIC

#if (1 == DEVICE_0001_DIGITALOUT_GENERIC)
EZLOPI_DEVICE_LIST("digital-out relay                   ", 2, device_0002_digitalOut_relay)
#endif // DEVICE_0001_DIGITALOUT_GENERIC

#if (1 == DEVICE_0001_DIGITALOUT_GENERIC)
EZLOPI_DEVICE_LIST("digital-out plug                    ", 3, device_0003_digitalOut_plug)
#endif // DEVICE_0001_DIGITALOUT_GENERIC

#if (1 == DEVICE_0001_DIGITALOUT_GENERIC)
EZLOPI_DEVICE_LIST("digital-in generic                  ", 4, device_0004_digitalIn_generic)
#endif // DEVICE_0001_DIGITALOUT_GENERIC


#if (1 == SENSOR_0005_I2C_MPU6050)
EZLOPI_DEVICE_LIST("sensor-MPU6050-I2C                  ", 5, sensor_0005_I2C_MPU6050)
#endif // SENSOR_0005_I2C_MPU6050

#if (1 == SENSOR_0006_I2C_ADXL345)
EZLOPI_DEVICE_LIST("sensor-ADXL345-I2C                  ", 6, sensor_0006_I2C_ADXL345)
#endif // SENSOR_0006_I2C_ADXL345

#if (1 == SENSOR_0007_I2C_GY271)
EZLOPI_DEVICE_LIST("sensor-GY271-I2C                    ", 7, sensor_0007_I2C_GY271)
#endif // SENSOR_0007_I2C_GY271

#if (1 == SENSOR_0008_I2C_LTR303ALS)
EZLOPI_DEVICE_LIST("sensor-LTR303ALS-I2C                ", 8, sensor_0008_I2C_LTR303ALS)
#endif // SENSOR_0008_I2C_LTR303ALS

#if (1 == DEVICE_0009_OTHER_RMT_SK6812)
EZLOPI_DEVICE_LIST("device-rgp_strip-RMT_SK6812         ", 9, device_0009_other_RMT_SK6812)
#endif // DEVICE_0009_OTHER_RMT_SK6812


#if (1 == SENSOR_0010_I2C_BME680)
#if (CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32)
EZLOPI_DEVICE_LIST("sensor-bme680-i2c                   ", 10, sensor_0010_I2C_BME680)
#endif // CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32
#endif // SENSOR_0010_I2C_BME680

#if (1 == SENSOR_0012_I2C_BME280)
EZLOPI_DEVICE_LIST("sensor-bme280-i2c-temp-humid        ", 12, sensor_0012_I2C_BME280)
#endif // SENSOR_0012_I2C_BME280

#if (CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32 || CONFIG_IDF_TARGET_ESP32S2)

#if (1 == SENSOR_0015_ONEWIRE_DHT11)
EZLOPI_DEVICE_LIST("sensor-DHT11-1-wire-temp-humid      ", 15, sensor_0015_oneWire_DHT11)
#endif // SENSOR_0015_ONEWIRE_DHT11

#endif // CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32

#if (1 == SENSOR_0016_ONEWIRE_DHT22)
EZLOPI_DEVICE_LIST("sensor-DHT22-1-wire-temp-humid      ", 16, sensor_0016_oneWire_DHT22)
#endif // SENSOR_0016_ONEWIRE_DHT22


#if (1 == SENSOR_0017_ADC_POTENTIOMETER)
EZLOPI_DEVICE_LIST("sensor-potentiometer-ADC            ", 17, sensor_0017_ADC_potentiometer)
#endif // SENSOR_0017_ADC_POTENTIOMETER

#ifdef CONFIG_IDF_TARGET_ESP32
#if (1 == SENSOR_0018_OTHER_INTERNAL_HALL_EFFECT)
EZLOPI_DEVICE_LIST("sensor-Hall-effect                  ", 18, sensor_0018_other_internal_hall_effect)
#endif // SENSOR_0018_OTHER_INTERNAL_HALL_EFFECT
#endif // CONFIG_IDF_TARGET_ESP32

#if (1 == SENSOR_0019_DIGITALIN_PIR)
EZLOPI_DEVICE_LIST("sensor-PIR-digital-proximity        ", 19, sensor_0019_digitalIn_PIR)
#endif // SENSOR_0019_DIGITALIN_PIR

#if (1 == SENSOR_0020_OTHER_2AXIS_JOYSTICK)
EZLOPI_DEVICE_LIST("sensor-2AXIS-JOYSTICK               ", 20, sensor_0020_other_2axis_joystick)
#endif // SENSOR_0020_OTHER_2AXIS_JOYSTICK

#if (1 == SENSOR_0021_UART_MB1013)
EZLOPI_DEVICE_LIST("sensor-UART-MB1013                  ", 21, sensor_0021_UART_MB1013)
#endif // SENSOR_0021_UART_MB1013


#if (1 == DEVICE_0022_PWM_DIMMABLE_LAMP)
EZLOPI_DEVICE_LIST("device-PWM-dimmable-lamp            ", 22, device_0022_PWM_dimmable_lamp)
#endif // DEVICE_0022_PWM_DIMMABLE_LAMP

#if (1 == SENSOR_0023_DIGITALIN_TOUCH_SWITCH_TTP223B)
EZLOPI_DEVICE_LIST("sensor-TTP223B-digital-touch        ", 23, sensor_0023_digitalIn_touch_switch_TTP223B)
#endif // SENSOR_0023_DIGITALIN_TOUCH_SWITCH_TTP223B

#if (1 == SENSOR_0024_OTHER_HCSR04)
EZLOPI_DEVICE_LIST("sensor-HCSR04-pulse-width           ", 24, sensor_0024_other_HCSR04_v3)
#endif // SENSOR_0024_OTHER_HCSR04

#if (1 == SENSOR_0025_DIGITALIN_LDR)
EZLOPI_DEVICE_LIST("sensor-LDR-digital-in               ", 25, sensor_0025_digitalIn_LDR)
#endif // SENSOR_0025_DIGITALIN_LDR

#if (1 == SENSOR_0026_ADC_LDR)
EZLOPI_DEVICE_LIST("sensor-LDR-ADC                      ", 26, sensor_0026_ADC_LDR)
#endif // SENSOR_0026_ADC_LDR

#if (1 == SENSOR_0027_ADC_WATERLEAK)
EZLOPI_DEVICE_LIST("sensor-WATERLEAK-ADC                ", 27, sensor_0027_ADC_waterLeak)
#endif // SENSOR_0027_ADC_WATERLEAK

#if (1 == SENSOR_0028_OTHER_GY61)
EZLOPI_DEVICE_LIST("sensor-GY61-other-accelerometer     ", 28, sensor_0028_other_GY61)
#endif // SENSOR_0028_OTHER_GY61


#if (1 == SENSOR_0029_I2C_GXHTC3)
EZLOPI_DEVICE_LIST("sensor-GXHTC3-I2C-temp-humidity     ", 29, sensor_0029_I2C_GXHTC3)
#endif // SENSOR_0029_I2C_GXHTC3

#if (1 == SENSOR_0030_ONEWIRE_DS18B20)
EZLOPI_DEVICE_LIST("sensor-DS18B20-1-wire-temp          ", 30, sensor_0030_oneWire_DS18B20)
#endif // SENSOR_0030_ONEWIRE_DS18B20

#if (1 == SENSOR_0031_OTHER_JSNSR04T)
EZLOPI_DEVICE_LIST("sensor-JNSR04T-digital-distance     ", 31, sensor_0031_other_JSNSR04T)
#endif // SENSOR_0031_OTHER_JSNSR04T

#if (1 == SENSOR_0032_ADC_SOILMOISTURE)
EZLOPI_DEVICE_LIST("sensor-soilMoisture-ADC             ", 32, sensor_0032_ADC_soilMoisture)
#endif // SENSOR_0032_ADC_SOILMOISTURE

#if (1 == SENSOR_0033_ADC_TURBIDITY)
EZLOPI_DEVICE_LIST("sensor-turbidity-ADC                ", 33, sensor_0033_ADC_turbidity)
#endif // SENSOR_0033_ADC_TURBIDITY

#if (1 == SENSOR_0034_DIGITALIN_PROXIMITY)
EZLOPI_DEVICE_LIST("sensor-proximity-digital            ", 34, sensor_0034_digitalIn_proximity)
#endif // SENSOR_0034_DIGITALIN_PROXIMITY

#if (1 == SENSOR_0035_DIGITALIN_TOUCH_SENSOR_TPP223B)
EZLOPI_DEVICE_LIST("sensor-TPP223B-digital-touch        ", 35, sensor_0035_digitalIn_touch_sensor_TPP223B)
#endif // SENSOR_0035_DIGITALIN_TOUCH_SENSOR_TPP223B

#if (1 == DEVICE_0036_PWM_SERVO_MG996R)
EZLOPI_DEVICE_LIST("device-PWM-servo-MG996R             ", 36, device_0036_PWM_servo_MG996R)
#endif // DEVICE_0036_PWM_SERVO_MG996R

#if (1 == SENSOR_0037_PMS5003_SENSOR)
EZLOPI_DEVICE_LIST("sensor-PMS5003                      ", 37, sensor_pms5003_v3)
#endif // SENSOR_0037_PMS5003_SENSOR

#if (1 == DEVICE_0038_OTHER_RGB)
EZLOPI_DEVICE_LIST("device-other-RGB-LED                ", 38, device_0038_other_RGB)
#endif // DEVICE_0038_OTHER_RGB

#if (1 == SENSOR_0040_OTHER_TCS230)
EZLOPI_DEVICE_LIST("sensor-TCS230-other-color           ", 40, sensor_0040_other_TCS230)
#endif // SENSOR_0040_OTHER_TCS230

#if (1 == SENSOR_0041_ADC_FC28_SOILMOISTURE)
EZLOPI_DEVICE_LIST("sensor-FC28-soilMoisture-ADC        ", 41, sensor_0041_ADC_FC28_soilMoisture)
#endif // SENSOR_0041_ADC_FC28_SOILMOISTURE

#if (1 == SENSOR_0042_ADC_SHUNT_VOLTMETER)
EZLOPI_DEVICE_LIST("sensor-shunt-voltmeter-ADC          ", 42, sensor_0042_ADC_shunt_voltmeter)
#endif // SENSOR_0042_ADC_SHUNT_VOLTMETER

#if (1 == SENSOR_0043_ADC_GYML8511_UV_INTENSITY)
EZLOPI_DEVICE_LIST("sensor-GYML8511-ADC-intensity       ", 43, sensor_0043_ADC_GYML8511_UV_intensity)
#endif // SENSOR_0043_ADC_GYML8511_UV_INTENSITY

#if (1 == SENSOR_0044_I2C_TSL256_LUMINOSITY)
EZLOPI_DEVICE_LIST("sensor-TSL256-I2C-luminosity        ", 44, sensor_0044_I2C_TSL256_luminosity)
#endif // SENSOR_0044_I2C_TSL256_LUMINOSITY

#if (1 == SENSOR_0046_ADC_ACS712_05B_CURRENTMETER)
EZLOPI_DEVICE_LIST("sensor-ACS712-currentmeter-ADC      ", 46, sensor_0046_ADC_ACS712_05B_currentmeter)
#endif // SENSOR_0046_ADC_ACS712_05B_CURRENTMETER

#if (1 == SENSOR_0047_OTHER_HX711_LOADCELL)
EZLOPI_DEVICE_LIST("sensor-HX711-loadcell               ", 47, sensor_0047_other_HX711_loadcell)
#endif // SENSOR_0047_OTHER_HX711_LOADCELL

#if (1 == SENSOR_0048_OTHER_MQ4_CH4_DETECTOR)
EZLOPI_DEVICE_LIST("sensor-MQ4-CH4-methane              ", 48, sensor_0048_other_MQ4_CH4_detector)
#endif // SENSOR_0048_OTHER_MQ4_CH4_DETECTOR

#if (1 == SENSOR_0049_OTHER_MQ2_LPG_DETECTOR)
EZLOPI_DEVICE_LIST("sensor-MQ2-LPG-detector             ", 49, sensor_0049_other_MQ2_LPG_detector)
#endif // SENSOR_0049_OTHER_MQ2_LPG_DETECTOR

#if (1 == SENSOR_0050_OTHER_MQ3_ALCOHOL_DETECTOR)
EZLOPI_DEVICE_LIST("sensor-MQ3-alcohol-detector         ", 50, sensor_0050_other_MQ3_alcohol_detector)
#endif // SENSOR_0050_OTHER_MQ3_ALCOHOL_DETECTOR

#if (1 == SENSOR_0051_OTHER_MQ8_H2_DETECTOR)
EZLOPI_DEVICE_LIST("sensor-MQ8-H2-detector              ", 51, sensor_0051_other_MQ8_H2_detector)
#endif // SENSOR_0051_OTHER_MQ8_H2_DETECTOR

#if (1 == SENSOR_0052_OTHER_MQ135_NH3_DETECTOR)
EZLOPI_DEVICE_LIST("sensor-MQ135-NH3-detector           ", 52, sensor_0052_other_MQ135_NH3_detector)
#endif // SENSOR_0052_OTHER_MQ135_NH3_DETECTOR

#if (1 == SENSOR_0053_UART_GYGPS6MV2)
EZLOPI_DEVICE_LIST("sensor-GYGPS6MV2-UART-gps           ", 53, sensor_0053_UART_GYGPS6MV2)
#endif // SENSOR_0053_UART_GYGPS6MV2

#if (1 == SENSOR_0054_PWM_YFS201_FLOWMETER)
EZLOPI_DEVICE_LIST("sensor-YFS201-PWM-flowmeter         ", 54, sensor_0054_PWM_YFS201_flowmeter)
#endif // SENSOR_0054_PWM_YFS201_FLOWMETER

#if (1 == SENSOR_0055_ADC_FLEXRESISTOR)
EZLOPI_DEVICE_LIST("sensor-Flex-Resistor-ADC            ", 55, sensor_0055_ADC_FlexResistor)
#endif // SENSOR_0055_ADC_FLEXRESISTOR

#if (1 == SENSOR_0056_ADC_FORCE_SENSITIVE_RESISTOR)
EZLOPI_DEVICE_LIST("sensor-Force-sensetive-resistor-ADC ", 56, sensor_0056_ADC_Force_Sensitive_Resistor)
#endif // SENSOR_0056_ADC_FORCE_SENSITIVE_RESISTOR

#if (1 == SENSOR_0057_OTHER_KY026_FLAMEDETECTOR)
EZLOPI_DEVICE_LIST("sensor-KY026-Flame-detector         ", 57, sensor_0057_other_KY026_FlameDetector)
#endif // SENSOR_0057_OTHER_KY026_FLAMEDETECTOR


#if (1 == SENSOR_0059_OTHER_MQ6_LPG_DETECTOR)
EZLOPI_DEVICE_LIST("sensor-MQ6-LPG-detector             ", 59, sensor_0059_other_MQ6_LPG_detector)
#endif // SENSOR_0059_OTHER_MQ6_LPG_DETECTOR

#if (1 == SENSOR_0060_DIGITALIN_VIBRATION_DETECTOR)
EZLOPI_DEVICE_LIST("sensor-vibration-detector-Digital   ", 60, sensor_0060_digitalIn_vibration_detector)
#endif // SENSOR_0060_DIGITALIN_VIBRATION_DETECTOR

#if (1 == SENSOR_0061_DIGITALIN_REED_SWITCH)
EZLOPI_DEVICE_LIST("sensor-reed-switch-digital          ", 61, sensor_0061_digitalIn_reed_switch)
#endif // SENSOR_0061_DIGITALIN_REED_SWITCH

#if (1 == SENSOR_0062_OTHER_MQ7_CO_DETECTOR)
EZLOPI_DEVICE_LIST("sensor-MQ7-CO-detector              ", 62, sensor_0062_other_MQ7_CO_detector)
#endif // SENSOR_0062_OTHER_MQ7_CO_DETECTOR

#if (1 == SENSOR_0063_OTHER_MQ9_LPG_FLAMEABLE_DETECTOR)
EZLOPI_DEVICE_LIST("sensor-MQ9-LPG-flameable-detector   ", 63, sensor_0063_other_MQ9_LPG_flameable_detector)
#endif // SENSOR_0063_OTHER_MQ9_LPG_FLAMEABLE_DETECTOR


#if (1 == SENSOR_0065_DIGITALIN_FLOAT_SWITCH)
EZLOPI_DEVICE_LIST("sensor-float-switch-digital         ", 65, sensor_0065_digitalIn_float_switch)
#endif // SENSOR_0065_DIGITALIN_FLOAT_SWITCH

#if (1 == SENSOR_0066_OTHER_R307_FINGERPRINT)
EZLOPI_DEVICE_LIST("sensor-fingerprint-sensor           ", 66, sensor_0066_other_R307_FingerPrint)
#endif // SENSOR_0066_OTHER_R307_FINGERPRINT

#if (1 == SENSOR_0067_UART_HILINK_PRESENCE_SENSOR)
EZLOPI_DEVICE_LIST("sensor-hilink-presence-sensor       ", 67, sensor_0067_hilink_presence_sensor_v3)
#endif // SENSOR_0067_UART_HILINK_PRESENCE_SENSOR

#if (1 == SENSOR_0068_I2C_ENS160_GAS_SENSOR)
EZLOPI_DEVICE_LIST("sensor-ENS160-gas-sensor            ", 68, sensor_0068_ENS160_gas_sensor)
#endif // SENSOR_0068_I2C_ENS160_GAS_SENSOR

#if (1 == SENSOR_0069_UART_ZE08_CH02_GAS_SENSOR)
EZLOPI_DEVICE_LIST("sensor-ze08-ch02-gas-sensor         ", 69, sensor_0069_ze08_ch02_gas_sensor)
#endif // SENSOR_0069_UART_ZE08_CH02_GAS_SENSOR

#if (1 == SENSOR_0070_ADC_DUMMY_POTENTIOMETER)
EZLOPI_DEVICE_LIST("sensor-dummy-potentiometer-ADC      ", 70, sensor_0070_ADC_dummy_potentiometer)
#endif // SENSOR_0070_ADC_DUMMY_POTENTIOMETER

//////// End of sensor/device list
EZLOPI_DEVICE_LIST(NULL, 0, NULL)