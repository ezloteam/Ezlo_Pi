
idf_component_register(SRCS "sensor_0050_other_MQ3_Alcohol_detector.c" 
                        INCLUDE_DIRS "."          
                        REQUIRES trace 
                                updated
                                ezlopi_actions 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_devices 
                                ezlopi_cloud_constants
                                )