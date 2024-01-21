file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0005_I2C_MPU6050.c" "mpu6050.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace 
            ezlopi-hal-i2c
            ezlopi-core-timer 
            ezlopi-core-actions 
            ezlopi-core-devices 
            ezlopi-cloud-constants 
            ezlopi-core-valueformatter)