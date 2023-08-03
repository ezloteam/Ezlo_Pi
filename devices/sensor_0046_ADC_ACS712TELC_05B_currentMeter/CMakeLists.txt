
idf_component_register(SRCS "sensor_0046_ADC_ACS712TELC_05B_currentMeter.c" 
                        INCLUDE_DIRS "."          
                        REQUIRES trace 
                                updated
                                ezlopi_actions 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_devices 
                                ezlopi_cloud_constants
                                )