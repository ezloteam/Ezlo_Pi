file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0046_ADC_ACS712_05B_currentmeter.c" "${config_src}"
                        INCLUDE_DIRS "."          
                        REQUIRES ezlopi-util-trace 
                                # ezlopi-core-timer 
                                ezlopi-core-cloud 
                                ezlopi-core-actions 
                                ezlopi-core-devices 
                                ezlopi-core-valueformatter 

                                ezlopi-hal-adc

                                ezlopi-cloud-items 
                                ezlopi-cloud-constants 
                                )