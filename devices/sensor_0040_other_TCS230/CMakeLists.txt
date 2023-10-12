idf_component_register(SRCS "sensor_0040_other_TCS230.c" "TCS230.c"
                        INCLUDE_DIRS "."
                        REQUIRES trace 
                                gpio_isr_service
                                ezlopi_actions 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_devices 
                                ezlopi_cloud_constants  
                                ezlopi_valueformatter)