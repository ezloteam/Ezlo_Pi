


idf_component_register(SRCS "sensor_0018_internal_hall_effect.c"
                        INCLUDE_DIRS .
                        REQUIRES trace 
                                 gpio_isr_service
                                 ezlopi_devices 
                                 ezlopi_actions 
                                 ezlopi_timer)
