


idf_component_register(SRCS "sensor_0019_digitalIn_PIR.c"
                        INCLUDE_DIRS .
                        REQUIRES 
                                trace
                                items
                                gpio_isr_service
                                ezlopi_actions 
                                ezlopi_timer
                                ezlopi_valueformatter 
                                )