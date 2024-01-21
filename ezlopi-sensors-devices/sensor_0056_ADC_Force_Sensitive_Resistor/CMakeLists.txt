
idf_component_register(SRCS "sensor_0056_ADC_Force_Sensitive_Resistor.c" 
                        INCLUDE_DIRS "."          
                        REQUIRES trace 
                                ezlopi-core-actions 
                                 
                                ezlopi-core-timer 
                                ezlopi-core-devices 
                                ezlopi-cloud-constants
                                )