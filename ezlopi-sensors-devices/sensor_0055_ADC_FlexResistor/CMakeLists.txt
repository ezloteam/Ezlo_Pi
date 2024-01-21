
idf_component_register(SRCS "sensor_0055_ADC_FlexResistor.c" 
    INCLUDE_DIRS "."
    REQUIRES    trace 
                ezlopi-core-actions 
                 
                ezlopi-core-timer 
                ezlopi-core-devices 
                ezlopi-cloud-constants 
                ezlopi-service-gpioisr)

