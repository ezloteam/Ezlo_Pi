# set(config_src "config.cpp")

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
                                        
                                                        
                                0044_sensor_I2C_TSL256_luminosity
                                0057_sensor_other_KY026_FlameDetector
                                0053_sensor_UART_GYGPS6MV2
                                0054_sensor_PWM_YFS201_flowmeter
                                0056_sensor_ADC_Force_Sensitive_Resistor                                
                                0048_sensor_other_MQ4_CH4_detector
                                0049_sensor_other_MQ2_LPG_detector
                                0050_sensor_other_MQ3_alcohol_detector
                                0051_sensor_other_MQ8_H2_detector
                                0052_sensor_other_MQ135_NH3_detector
                                0059_sensor_other_MQ6_LPG_detector
                                0062_sensor_other_MQ7_CO_detector
                                0063_sensor_other_MQ9_LPG_flameable_detector
                                0055_sensor_ADC_FlexResistor
                                0060_sensor_digitalIn_vibration_detector
                                0061_sensor_digitalIn_reed_switch
                                0065_sensor_digitalIn_float_switch
                                )


