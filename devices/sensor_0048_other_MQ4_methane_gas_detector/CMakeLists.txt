
idf_component_register(SRCS "sensor_0048_other_MQ4_methane_gas_detector.c" 
                        INCLUDE_DIRS "."          
                        REQUIRES trace 
                                ezlopi_actions 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_devices 
                                ezlopi_cloud_constants
                                ezlopi_valueformatter
                                )