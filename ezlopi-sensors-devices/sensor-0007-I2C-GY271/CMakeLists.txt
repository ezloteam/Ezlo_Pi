file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "gy271.c" "sensor_0007_I2C_GY271.c" "${config_src}"
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