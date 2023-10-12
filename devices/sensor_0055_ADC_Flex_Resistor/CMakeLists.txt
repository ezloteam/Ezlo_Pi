
idf_component_register(SRCS "sensor_0055_ADC_Flex_Resistor.c" 
                        INCLUDE_DIRS "."          
                        REQUIRES trace 
                                ezlopi_actions 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_devices 
                                ezlopi_cloud_constants
                                ezlopi_valueformatter
                                )