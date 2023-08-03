

idf_component_register(SRCS "device_0009_other_RMT_SK6812.c"
                        INCLUDE_DIRS "."
                        REQUIRES trace 
                                 updated
                                 led_strip
                                 ezlopi_actions 
                                 ezlopi_hal 
                                 ezlopi_timer 
                                 ezlopi_devices 
                                 ezlopi_cloud_constants 
                                 gpio_isr_service)

