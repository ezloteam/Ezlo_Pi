idf_component_register(SRCS "device_0036_PWM_servo_MG996R.c"
                        INCLUDE_DIRS .
                        REQUIRES trace 
                                ezlopi_hal 
                                ezlopi_timer 
                                ezlopi_actions 
                                ezlopi_devices 
                                ezlopi-cloud-constants 
                                gpio_isr_service)