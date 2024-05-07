file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "TCS230.c" "sensor_0040_other_TCS230.c" "${config_src}"
                    INCLUDE_DIRS "."
                    REQUIRES   
                                ezlopi-util-trace 
                                freertos
                                ezlopi-core-timer 
                                ezlopi-core-cloud 
                                ezlopi-core-actions 
                                ezlopi-core-devices 
                                ezlopi-core-valueformatter
                                ezlopi-core-processes
                                 
                                ezlopi-hal-adc

                                ezlopi-cloud-items
                                ezlopi-cloud-constants
                                )