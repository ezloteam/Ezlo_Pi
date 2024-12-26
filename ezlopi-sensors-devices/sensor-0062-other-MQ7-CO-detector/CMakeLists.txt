
idf_component_register(SRCS "sensor_0062_other_MQ7_CO_detector.c" 
    INCLUDE_DIRS "."
    REQUIRES ezlopi-util-trace 
            # ezlopi-core-timer 
            ezlopi-core-cloud 
            ezlopi-core-actions 
            ezlopi-core-devices 
            ezlopi-core-valueformatter 
            ezlopi-core-processes
            ezlopi-core-errors
            
            ezlopi-hal-adc

            ezlopi-cloud-items
            ezlopi-cloud-constants 
            
            ezlopi-service-loop
            )

