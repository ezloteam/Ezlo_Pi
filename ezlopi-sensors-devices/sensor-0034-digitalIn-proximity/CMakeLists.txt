file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0034_digitalIn_proximity.c"
                        INCLUDE_DIRS "."
                        REQUIRES    ezlopi-util-trace 
                                    driver
                                    # ezlopi-core-timer 
                                    ezlopi-core-cloud 
                                    ezlopi-core-actions 
                                    ezlopi-core-devices 
                                    ezlopi-core-valueformatter 
                                    ezlopi-core-errors
                                    
                                    ezlopi-cloud-items
                                    ezlopi-cloud-constants 

                                    ezlopi-service-gpioisr
                                    )