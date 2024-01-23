
idf_component_register(SRCS "sensor_0051_other_MQ8_H2_detector.c" 
    INCLUDE_DIRS "."
    REQUIRES    trace 
                ezlopi-core-timer 
                ezlopi-core-cloud 
                ezlopi-core-actions 
                ezlopi-core-devices 
                ezlopi-core-valueformatter 

                ezlopi-hal-adc

                ezlopi-cloud-items
                ezlopi-cloud-constants 
                )

