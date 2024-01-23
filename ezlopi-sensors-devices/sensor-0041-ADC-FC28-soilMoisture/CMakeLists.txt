file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0041_ADC_FC28_soilMoisture.c" "${config_src}"
                    INCLUDE_DIRS "."
                    REQUIRES    trace 
                                ezlopi-core-timer 
                                ezlopi-core-cloud 
                                ezlopi-core-actions 
                                ezlopi-core-devices 
                                ezlopi-core-valueformatter 

                                ezlopi-hal-adc

                                ezlopi-cloud-items 
                                ezlopi-cloud-constants 
                                )
