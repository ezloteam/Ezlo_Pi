
idf_component_register(SRCS "sensor_0056_ADC_Force_Sensitive_Resistor.c" 
                        INCLUDE_DIRS "."          
                        REQUIRES trace 
                                ezlopi_actions 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_devices 
                                ezlopi-cloud-constants
                                )