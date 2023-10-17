
idf_component_register(SRCS "0050_sensor_other_MQ3_alcohol_detector.c" 
    INCLUDE_DIRS "."
    REQUIRES trace 
            ezlopi_actions 
            ezlopi_hal 
            ezlopi_timer 
            ezlopi_devices 
            ezlopi_cloud_constants )

