file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0005_I2C_MPU6050.c" "mpu6050.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES trace 
            ezlopi_hal 
            ezlopi_timer 
            ezlopi_actions 
            ezlopi_devices 
            ezlopi-cloud-constants 
            ezlopi_valueformatter)