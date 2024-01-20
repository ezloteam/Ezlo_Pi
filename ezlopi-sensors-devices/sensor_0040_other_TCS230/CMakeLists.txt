file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0040_other_TCS230.c" "${config_src}"
                    INCLUDE_DIRS "."
                    REQUIRES   trace 
                                freertos
                                ezlopi_actions 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_devices 
                                ezlopi-cloud-constants
                                ezlopi_valueformatter
                                )