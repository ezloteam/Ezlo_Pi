idf_component_register(SRCS "device_0070_other_RGB_CW.c"
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-util-trace 
                ezlopi-core-timer 
                ezlopi-core-cloud 
                ezlopi-core-actions 
                ezlopi-core-devices 
                ezlopi-core-valueformatter 

                ezlopi-cloud-items 
                ezlopi-cloud-constants   
                
                ezlopi-service-gpioisr
                )