file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "sensor_0023_digitalIn_touch_switch_TTP223B.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace ezlopi-core-actions  ezlopi-core-timer ezlopi-core-devices ezlopi-cloud-constants ezlopi-service-gpioisr)