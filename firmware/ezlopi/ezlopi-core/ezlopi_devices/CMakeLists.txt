# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi_devices.c" "ezlopi_device_value_updated.c" "ezlopi_devices_list.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    trace 
                json 
                ezlopi_nvs 
                ezlopi_hal 
                ezlopi_cloud 
                ezlopi_actions
                digital_io 
                sensor_pir 
                sensor_bme280 
                dimmable_bulb 
                joystick_2_axis 
                ultrasonic_HC_SR04 
                0008_sensor_I2C_LTR303ALS 
                0009_other_RMT_SK6812 
                0010_I2C_BME680
                0023_sensor_touch_switch_tpp_223b 
                ultrasonic_MB1013 
                servo_motor_MG_996R 
                025_sens_ldr_digital_module 
                026_sens_ldr_analog_sensor 
                027_sens_water_sensor
                sensor_sound 
                1024_device_health 
                0029_gxhtc3_rh_t_sensor 
                029_IR_blaster_remote 
                030_sens_ds18b20_sensor 
                032_sens_soil_moisture 
                0035_sensor_touch_sensor_tpp_223b 
                016_sens_dht22_sensor
                031_JSN_SR04T_WaterLevelSensor
                033_turbidity_sensor 
                034_proximity_sensor 
                0038_digitalOut_RGB 
                0044_sensor_I2C_TSL256_luminosity
                0054_sensor_PWM_YFS201_flowmeter
                0056_sensor_ADC_Force_Sensitive_Resistor
                )


