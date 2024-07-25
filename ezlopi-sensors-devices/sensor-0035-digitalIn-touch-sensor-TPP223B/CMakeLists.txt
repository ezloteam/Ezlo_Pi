

idf_component_register(SRCS "sensor_0035_digitalIn_touch_sensor_TPP223B.c"
                            INCLUDE_DIRS .
                            REQUIRES 
                                    ezlopi-util-trace  
                                    driver
                                    ezlopi-core-timer 
                                    ezlopi-core-cloud 
                                    ezlopi-core-actions 
                                    ezlopi-core-devices 
                                    ezlopi-core-valueformatter 
                                    ezlopi-core-errors

                                    ezlopi-cloud-items
                                    ezlopi-cloud-constants 

                                    ezlopi-service-gpioisr
                                    )
                                 

