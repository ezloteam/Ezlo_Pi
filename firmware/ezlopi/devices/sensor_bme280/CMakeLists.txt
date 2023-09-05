# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")
# set(config_src "sensor_bme280.c")

idf_component_register(SRCS "0012_bme280_sensor.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace ezlopi_actions ezlopi_timer library items)
