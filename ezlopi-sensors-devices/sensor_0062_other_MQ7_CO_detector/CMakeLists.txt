
idf_component_register(SRCS "sensor_0062_other_MQ7_CO_detector.c" 
    INCLUDE_DIRS "."
    REQUIRES trace 
            ezlopi-core-timer 
            ezlopi-core-cloud 
            ezlopi-core-actions 
            ezlopi-core-devices 
            ezlopi-core-valueformatter 
            
            ezlopi-hal-adc

            ezlopi-cloud-items
            ezlopi-cloud-constants 
            )

