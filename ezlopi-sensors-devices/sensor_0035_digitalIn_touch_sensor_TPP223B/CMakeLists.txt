

idf_component_register(SRCS "sensor_0035_digitalIn_touch_sensor_TPP223B.c"
                            INCLUDE_DIRS .
                            REQUIRES 
                                    trace  
                                    driver
                                    ezlopi-core-timer 
                                    ezlopi-core-cloud 
                                    ezlopi-core-actions 
                                    ezlopi-core-devices 
                                    ezlopi-core-valueformatter 

                                    ezlopi-cloud-items
                                    ezlopi-cloud-constants 

                                    ezlopi-service-gpioisr
                                    )
                                 

