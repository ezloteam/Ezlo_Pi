
idf_component_register(SRCS "0052_sensor_other_MQ135_NH3_detector.c" 
    INCLUDE_DIRS "."
    REQUIRES trace 
            ezlopi_actions 
            ezlopi_hal 
            ezlopi_timer 
            ezlopi_devices 
            ezlopi_cloud_constants )

