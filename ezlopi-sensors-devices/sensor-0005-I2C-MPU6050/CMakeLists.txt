file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0005_I2C_MPU6050.c" "mpu6050.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-util-trace
                # ezlopi-core-timer
                ezlopi-core-cloud
                ezlopi-core-actions
                ezlopi-core-devices
                ezlopi-core-valueformatter
                ezlopi-core-processes
                ezlopi-core-errors

                ezlopi-hal-i2c

                ezlopi-cloud-items
                ezlopi-cloud-constants
                )
