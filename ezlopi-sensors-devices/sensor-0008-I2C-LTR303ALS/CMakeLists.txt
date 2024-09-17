file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "sensor_0008_I2C_LTR303ALS.c" "LTR303.c" "ALS_LTR303.c" "${config_src}"
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
                
                ezlopi-service-gpioisr 
                )