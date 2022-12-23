# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace json ezlopi_nvs ezlopi_hal ezlopi_cloud
    digital_io sensor_pir sensor_bme280 sensor_door dimmable_bulb joystick_2_axis ultrasonic_MB1013)