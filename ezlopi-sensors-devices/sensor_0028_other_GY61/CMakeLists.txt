file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0028_other_GY61.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES  
                trace 
                ezlopi-service-gpioisr
                ezlopi-core-actions 
                 
                ezlopi-core-timer 
                ezlopi-core-devices 
                ezlopi-cloud-constants
                ezlopi-core-valueformatter)

