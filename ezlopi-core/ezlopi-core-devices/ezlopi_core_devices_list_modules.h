

EZLOPI_DEVICE_LIST("digital-out generic                 ", 1, device_0001_digitalOut_generic)
EZLOPI_DEVICE_LIST("digital-out relay                   ", 2, device_0002_digitalOut_relay)
EZLOPI_DEVICE_LIST("digital-out plug                    ", 3, device_0003_digitalOut_plug)
EZLOPI_DEVICE_LIST("digital-in generic                  ", 4, device_0004_digitalIn_generic)

EZLOPI_DEVICE_LIST("sensor-MPU6050-I2C                  ", 5, sensor_0005_I2C_MPU6050)
EZLOPI_DEVICE_LIST("sensor-ADXL345-I2C                  ", 6, sensor_0006_I2C_ADXL345)
EZLOPI_DEVICE_LIST("sensor-GY271-I2C                    ", 7, sensor_0007_I2C_GY271)
EZLOPI_DEVICE_LIST("sensor-LTR303ALS-I2C                ", 8, sensor_0008_I2C_LTR303ALS)
EZLOPI_DEVICE_LIST("device-rgp_strip-RMT_SK6812         ", 9, device_0009_other_RMT_SK6812)

#if (CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32)
EZLOPI_DEVICE_LIST("sensor-bme680-i2c                   ", 10, sensor_0010_I2C_BME680)
#endif // CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32

EZLOPI_DEVICE_LIST("sensor-bme280-i2c-temp-humid        ", 12, sensor_0012_I2C_BME280)

#if (CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32)
EZLOPI_DEVICE_LIST("sensor-DHT11-1-wire-temp-humid      ", 15, sensor_0015_oneWire_DHT11)

EZLOPI_DEVICE_LIST("sensor-DHT22-1-wire-temp-humid      ", 16, sensor_0016_oneWire_DHT22)
#endif // CONFIG_IDF_TARGET_ESP32S3 || CONFIG_IDF_TARGET_ESP32

EZLOPI_DEVICE_LIST("sensor-potentiometer-ADC            ", 17, sensor_0017_ADC_potentiometer)

#ifdef CONFIG_IDF_TARGET_ESP32
EZLOPI_DEVICE_LIST("sensor-Hall-effect                  ", 18, sensor_0018_other_internal_hall_effect)
#endif // CONFIG_IDF_TARGET_ESP32

// EZLOPI_DEVICE_LIST("sensor-PIR-digital-proximity        ", 19, sensor_0019_digitalIn_PIR)
EZLOPI_DEVICE_LIST("sensor-2AXIS-JOYSTICK               ", 20, sensor_0020_other_2axis_joystick)
// EZLOPI_DEVICE_LIST("sensor-UART-MB1013                  ", 21, sensor_0021_UART_MB1013)

// EZLOPI_DEVICE_LIST("device-PWM-dimmable-lamp            ", 22, device_0022_PWM_dimmable_lamp)
// EZLOPI_DEVICE_LIST("sensor-TTP223B-digital-touch        ", 23, sensor_0023_digitalIn_touch_switch_TTP223B)
// EZLOPI_DEVICE_LIST("sensor-HCSR04-pulse-width           ", 24, sensor_0024_other_HCSR04_v3)
// EZLOPI_DEVICE_LIST("sensor-LDR-digital-in               ", 25, sensor_0025_digitalIn_LDR)
// EZLOPI_DEVICE_LIST("sensor-LDR-ADC                      ", 26, sensor_0026_ADC_LDR)
EZLOPI_DEVICE_LIST("sensor-WATERLEAK-ADC                ", 27, sensor_0027_ADC_waterLeak)

// EZLOPI_DEVICE_LIST("sensor-GY61-other-accelerometer     ", 28, sensor_0028_other_GY61)

// EZLOPI_DEVICE_LIST("sensor-GXHTC3-I2C-temp-humidity     ", 29, sensor_0029_I2C_GXHTC3)
// EZLOPI_DEVICE_LIST("sensor-DS18B20-1-wire-temp          ", 30, sensor_0030_oneWire_DS18B20)
// EZLOPI_DEVICE_LIST("sensor-JNSR04T-digital-distance     ", 31, sensor_0031_other_JSNSR04T)
EZLOPI_DEVICE_LIST("sensor-soilMoisture-ADC             ", 32, sensor_0032_ADC_soilMoisture)
// EZLOPI_DEVICE_LIST("sensor-turbidity-ADC                ", 33, sensor_0033_ADC_turbidity)
// EZLOPI_DEVICE_LIST("sensor-proximity-digital            ", 34, sensor_0034_digitalIn_proximity)
// EZLOPI_DEVICE_LIST("sensor-TPP223B-digital-touch        ", 35, sensor_0035_digitalIn_touch_sensor_TPP223B)
// EZLOPI_DEVICE_LIST("sensor-PMS5003                      ", 37, sensor_pms5003_v3)
// EZLOPI_DEVICE_LIST("device-other-RGB-LED                ", 38, device_0038_other_RGB)

// EZLOPI_DEVICE_LIST("device-PWM-servo-MG996R             ", 36, device_0036_PWM_servo_MG996R)

// EZLOPI_DEVICE_LIST("sensor-TCS230-other-color           ", 40, sensor_0040_other_TCS230)
// EZLOPI_DEVICE_LIST("sensor-FC28-soilMoisture-ADC        ", 41, sensor_0041_ADC_FC28_soilMoisture)
// EZLOPI_DEVICE_LIST("sensor-shunt-voltmeter-ADC          ", 42, sensor_0042_ADC_shunt_voltmeter)
// EZLOPI_DEVICE_LIST("sensor-GYML8511-ADC-intensity       ", 43, sensor_0043_ADC_GYML8511_UV_intensity)
// EZLOPI_DEVICE_LIST("sensor-TSL256-I2C-luminosity        ", 44, sensor_0044_I2C_TSL256_luminosity)

// EZLOPI_DEVICE_LIST("sensor-ACS712-currentmeter-ADC      ", 46, sensor_0046_ADC_ACS712_05B_currentmeter)

// EZLOPI_DEVICE_LIST("sensor-HX711-loadcell               ", 47, sensor_0047_other_HX711_loadcell)

EZLOPI_DEVICE_LIST("sensor-MQ4-CH4-methane              ", 48, sensor_0048_other_MQ4_CH4_detector)
// EZLOPI_DEVICE_LIST("sensor-MQ2-LPG-detector             ", 49, sensor_0049_other_MQ2_LPG_detector)
// EZLOPI_DEVICE_LIST("sensor-MQ3-alcohol-detector         ", 50, sensor_0050_other_MQ3_alcohol_detector)
// EZLOPI_DEVICE_LIST("sensor-MQ8-H2-detector              ", 51, sensor_0051_other_MQ8_H2_detector)
// EZLOPI_DEVICE_LIST("sensor-MQ135-NH3-detector           ", 52, sensor_0052_other_MQ135_NH3_detector)
// EZLOPI_DEVICE_LIST("sensor-GYGPS6MV2-UART-gps           ", 53, sensor_0053_UART_GYGPS6MV2)
// EZLOPI_DEVICE_LIST("sensor-YFS201-PWM-flowmeter         ", 54, sensor_0054_PWM_YFS201_flowmeter)
EZLOPI_DEVICE_LIST("sensor-Flex-Resistor-ADC            ", 55, sensor_0055_ADC_FlexResistor)
// EZLOPI_DEVICE_LIST("sensor-Force-sensetive-resistor-ADC ", 56, sensor_0056_ADC_Force_Sensitive_Resistor)
EZLOPI_DEVICE_LIST("sensor-KY026-Flame-detector         ", 57, sensor_0057_other_KY026_FlameDetector)

// EZLOPI_DEVICE_LIST("sensor-MQ6-LPG-detector             ", 59, sensor_0059_other_MQ6_LPG_detector)
// EZLOPI_DEVICE_LIST("sensor-vibration-detector-Digital   ", 60, sensor_0060_digitalIn_vibration_detector)
// EZLOPI_DEVICE_LIST("sensor-reed-switch-digital          ", 61, sensor_0061_digitalIn_reed_switch)
// EZLOPI_DEVICE_LIST("sensor-MQ7-CO-detector              ", 62, sensor_0062_other_MQ7_CO_detector)
EZLOPI_DEVICE_LIST("sensor-MQ9-LPG-flameable-detector   ", 63, sensor_0063_other_MQ9_LPG_flameable_detector)

// EZLOPI_DEVICE_LIST("sensor-float-switch-digital         ", 65, sensor_0065_digitalIn_float_switch)
// EZLOPI_DEVICE_LIST("sensor-fingerprint-sensor           ", 66, sensor_0066_other_R307_FingerPrint)
// EZLOPI_DEVICE_LIST("sensor-hilink-presence-sensor       ", 67, sensor_0067_hilink_presence_sensor_v3)
// EZLOPI_DEVICE_LIST("sensor-ENS160-gas-sensor            ", 68, sensor_0068_ENS160_gas_sensor)
// EZLOPI_DEVICE_LIST("sensor-ze08-ch02-gas-sensor         ", 69, sensor_0069_ze08_ch02_gas_sensor)

#warning "need to uncomment necessary modules";
//////// End of sensor/device list
EZLOPI_DEVICE_LIST(NULL, 0, NULL)