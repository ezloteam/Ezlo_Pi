
idf_component_register(SRCS "sensor_0063_other_MQ9_LPG_flameable_detector.c" 
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

