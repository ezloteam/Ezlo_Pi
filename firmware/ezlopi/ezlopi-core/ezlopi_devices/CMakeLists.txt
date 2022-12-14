# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "ezlopi_device_value_updated.c" "ezlopi_devices_list.c" "ezlopi_devices.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace json ezlopi_nvs ezlopi_hal ezlopi_cloud
    digital_io sensor_pir sensor_bme280 sensor_door dimmable_bulb joystick_2_axis ultrasonic_HC_SR04 ultrasonic_MB1013 servo_motor_MG_996R)


