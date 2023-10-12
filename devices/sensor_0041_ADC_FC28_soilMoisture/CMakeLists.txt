
idf_component_register(SRCS "sensor_0041_ADC_FC28_soilMoisture.c" 
                        INCLUDE_DIRS "."          
                        REQUIRES trace 
                                ezlopi_actions 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_devices 
                                ezlopi_cloud_constants
                                ezlopi_valueformatter
                                )