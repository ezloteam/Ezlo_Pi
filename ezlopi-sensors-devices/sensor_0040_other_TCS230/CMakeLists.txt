file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0040_other_TCS230.c" "${config_src}"
                    INCLUDE_DIRS "."
                    REQUIRES   trace 
                                freertos
                                ezlopi-core-actions 
                                 
                                ezlopi-core-timer 
                                ezlopi-core-devices 
                                ezlopi-cloud-constants
                                ezlopi-core-valueformatter
                                )