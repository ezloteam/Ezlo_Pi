
idf_component_register(SRCS "sensor_0044_I2C_TSL256_luminosity.c" "TSL256_interface.c"
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-util-trace 
                # ezlopi-core-timer    
                ezlopi-core-cloud 
                ezlopi-core-actions 
                ezlopi-core-devices 
                ezlopi-core-valueformatter
                ezlopi-core-errors

                ezlopi-hal-i2c

                ezlopi-cloud-items
                ezlopi-cloud-constants 
                )

