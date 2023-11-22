file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi_devices.c" "ezlopi_device_value_updated.c" "ezlopi_devices_list.c" "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES 
                                trace 
                                json 
                                ezlopi_nvs 
                                ezlopi_hal 
                                ezlopi_cloud 
                                ezlopi_actions                              
                                
                                device_0001_digitalOut_generic
                                device_0002_digitalOut_relay
                                device_0003_digitalOut_plug
                                device_0004_digitalIn_generic
                                
                                sensor_0008_I2C_LTR303ALS
                                device_0009_other_RMT_SK6812
                                sensor_0012_I2C_BME280
                                sensor_0015_oneWire_DHT11
                                sensor_0016_oneWire_DHT22

                                sensor_0018_other_internal_hall_effect
                                sensor_0019_digitalIn_PIR
                                sensor_0020_ADC_2axis_joystick
                                sensor_0021_UART_MB1013
                                device_0022_PWM_dimmable_lamp
                                sensor_0023_digitalIn_touch_switch_TTP223B
                                sensor_0024_other_HCSR04
                                sensor_0025_digitalIn_LDR
                                sensor_0027_ADC_waterLeak                                
                                sensor_0026_ADC_LDR
                                sensor_0029_I2C_GXHTC3
                                sensor_0030_oneWire_DS18B20
                                sensor_0031_other_JSNSR04T
                                sensor_0032_ADC_soilMoisture   
                                sensor_0033_ADC_turbidity   
                                sensor_0034_digitalIn_proximity 
                                sensor_0035_digitalIn_touch_sensor_TPP223B                                                         
                                device_0036_PWM_servo_MG996R
                                device_0038_other_RGB
                                        
                                                        
                                sensor_0044_I2C_TSL256_luminosity
                                sensor_0053_UART_GYGPS6MV2
                                sensor_0054_PWM_YFS201_flowmeter
                                sensor_0055_ADC_FlexResistor
                                sensor_0056_ADC_Force_Sensitive_Resistor                                
                                sensor_0057_other_KY026_FlameDetector
                                sensor_0048_other_MQ4_CH4_detector
                                sensor_0049_other_MQ2_LPG_detector
                                sensor_0050_other_MQ3_alcohol_detector
                                sensor_0051_other_MQ8_H2_detector
                                sensor_0052_other_MQ135_NH3_detector
                                sensor_0059_other_MQ6_LPG_detector
                                sensor_0060_digitalIn_vibration_detector
                                sensor_0061_digitalIn_reed_switch
                                sensor_0062_other_MQ7_CO_detector
                                sensor_0063_other_MQ9_LPG_flameable_detector
                                sensor_0065_digitalIn_float_switch
                                sensor_0066_other_R307_FingerPrint
                                )


