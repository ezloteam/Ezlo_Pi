
idf_component_register(SRCS "sensor_0052_other_MQ135_NH3_detector.c" 
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-util-trace 
                ezlopi-core-timer 
                ezlopi-core-cloud 
                ezlopi-core-actions 
                ezlopi-core-devices 
                ezlopi-core-valueformatter 

                ezlopi-hal-adc

                ezlopi-cloud-items
                ezlopi-cloud-constants 
                )

