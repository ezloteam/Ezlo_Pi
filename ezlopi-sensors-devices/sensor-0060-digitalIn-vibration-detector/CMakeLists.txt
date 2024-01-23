idf_component_register(SRCS "sensor_0060_digitalIn_vibration_detector.c" 
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-util-trace 
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