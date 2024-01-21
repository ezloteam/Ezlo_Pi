idf_component_register(SRCS "sensor_0060_digitalIn_vibration_detector.c" 
    INCLUDE_DIRS "."
    REQUIRES    trace 
                ezlopi-core-actions 
                 
                ezlopi-core-timer 
                ezlopi-core-devices 
                ezlopi-cloud-constants 
                ezlopi-service-gpioisr)