# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c*")

idf_component_register(SRCS "mpu6050.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace i2c_master_interface)