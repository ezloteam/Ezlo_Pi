# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi_devices.c" "ezlopi_device_value_updated.c" "ezlopi_devices_list.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace json ezlopi_nvs ezlopi_hal ezlopi_cloud ezlopi_actions
    digital_io sensor_pir sensor_bme280 door_hall_sensor dimmable_bulb joystick_2_axis ultrasonic_HC_SR04 
    sensor_touch_tpp_223b ultrasonic_MB1013 servo_motor_MG_996R 025_sens_ldr_digital_module 026_sens_ldr_analog_sensor 027_sens_water_sensor
    sensor_sound 1024_device_health 0029_gxhtc3_rh_t_sensor 029_IR_blaster_remote 030_sens_ds18b20_sensor 032_sens_soil_moisture 016_sens_dht22_sensor
    031_JSN_SR04T_WaterLevelSensor 033_turbidity_sensor 034_proximity_sensor)


