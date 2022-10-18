# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")
# set(config_src "sensor_bme280.c")

idf_component_register(SRCS "sensor_bme280.c"
    INCLUDE_DIRS "."
    REQUIRES debug ezlopi_actions sensor_service ezlopi_timer library)
