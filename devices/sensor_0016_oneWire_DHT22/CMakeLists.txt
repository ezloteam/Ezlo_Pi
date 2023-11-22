file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES trace 
                                driver 
                                ezlopi_actions 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_devices 
                                ezlopi_cloud_constants 
                                gpio_isr_service
                                ezlopi_valueformatter)