idf_component_register(SRCS "device_0038_other_RGB.c"
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-util-trace 
                ezlopi-core-timer 
                ezlopi-core-cloud 
                ezlopi-core-actions 
                ezlopi-core-devices 
                ezlopi-core-valueformatter 
                ezlopi-core-errors

                ezlopi-cloud-items 
                ezlopi-cloud-constants   
                
                ezlopi-service-gpioisr
                )