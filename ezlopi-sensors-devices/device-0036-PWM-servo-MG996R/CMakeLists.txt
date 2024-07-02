idf_component_register(SRCS "device_0036_PWM_servo_MG996R.c"
                        INCLUDE_DIRS .
                        REQUIRES ezlopi-util-trace 
                                # ezlopi-core-timer 
                                ezlopi-core-cloud 
                                ezlopi-core-actions 
                                ezlopi-core-devices 
                                ezlopi-core-valueformatter 

                                ezlopi-hal-pwm

                                ezlopi-cloud-items 
                                ezlopi-cloud-constants 

                                ezlopi-service-gpioisr
                                )