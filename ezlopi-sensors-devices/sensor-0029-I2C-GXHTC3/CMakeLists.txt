file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "sensor_0029_I2C_GXHTC3.c" "gxhtc3.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-util-trace 
                # ezlopi-core-timer 
                ezlopi-core-cloud
                ezlopi-core-actions  
                ezlopi-core-devices 
                ezlopi-core-valueformatter 
                
                ezlopi-hal-i2c

                ezlopi-cloud-items
                ezlopi-cloud-constants 

                ezlopi-service-gpioisr 
                )