idf_component_register(SRCS "sensor_0065_digitalIn_float_switch.c" 
                       INCLUDE_DIRS .
                       REQUIRES ezlopi-util-trace 
                                ezlopi-core-actions 
                                # ezlopi-core-timer 
                                ezlopi-core-devices 
                                ezlopi-core-valueformatter
                                ezlopi-core-errors

                                ezlopi-hal-gpio

                                ezlopi-cloud-items
                                ezlopi-cloud-constants 
                                
                                ezlopi-service-gpioisr
                                )

