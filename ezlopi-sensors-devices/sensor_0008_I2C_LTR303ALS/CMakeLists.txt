file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "sensor_0008_I2C_LTR303ALS.c" "LTR303.c" "ALS_LTR303.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES ezlopi-cloud-items ezlopi-hal-spi trace ezlopi-core-actions  ezlopi-core-timer ezlopi-core-devices ezlopi-cloud-constants ezlopi-service-gpioisr ezlopi-core-cloud)