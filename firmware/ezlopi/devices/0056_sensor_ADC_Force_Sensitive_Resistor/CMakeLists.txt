
idf_component_register(SRCS "0056_sensor_ADC_Force_Sensitive_Resistor.c" 
                        INCLUDE_DIRS "."          
                        REQUIRES trace 
                                ezlopi_actions 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_devices 
                                ezlopi_cloud_constants
                                )