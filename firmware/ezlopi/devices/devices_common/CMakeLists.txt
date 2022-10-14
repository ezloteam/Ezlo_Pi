# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "src/*.c*")

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "include"
    REQUIRES dht11 mpu6050 led plug switch nvs_storage debug json MPU6050_service sensor_service)
