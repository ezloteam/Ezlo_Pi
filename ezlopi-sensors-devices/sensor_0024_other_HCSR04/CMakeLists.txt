file(GLOB_RECURSE config_src "*.c")
# set(config_src)

idf_component_register(SRCS "sensor_0024_other_HCSR04.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    trace 
                driver
                ezlopi-core-timer 
                ezlopi-core-cloud 
                ezlopi-core-actions 
                ezlopi-core-devices 
                ezlopi-core-valueformatter 
                
                ezlopi-hal-gpio

                ezlopi-cloud-items 
                ezlopi-cloud-constants 

                ezlopi-service-gpioisr
                )