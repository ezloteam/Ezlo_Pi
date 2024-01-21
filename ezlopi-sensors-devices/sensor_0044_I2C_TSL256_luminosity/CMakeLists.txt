
idf_component_register(SRCS "sensor_0044_I2C_TSL256_luminosity.c" "TSL256_interface.c"
    INCLUDE_DIRS "."
    REQUIRES    trace 
                ezlopi-core-actions 
                 
                ezlopi-core-timer 
                ezlopi-core-devices 
                ezlopi-cloud-constants 
                ezlopi-service-gpioisr)

