# file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "TCS230.c" "sensor_0040_other_TCS230.c"
                    INCLUDE_DIRS "."
                    REQUIRES   trace 
                                freertos
                                ezlopi_actions 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_devices 
                                ezlopi_cloud_constants
                                ezlopi_valueformatter
                                )