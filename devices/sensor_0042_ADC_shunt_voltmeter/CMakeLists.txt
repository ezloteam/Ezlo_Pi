idf_component_register(SRCS "sensor_0042_ADC_shunt_voltmeter.c"           
                        INCLUDE_DIRS "."                          
                        REQUIRES trace 
                                updated
                                ezlopi_actions 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_devices 
                                ezlopi_cloud_constants
                                )