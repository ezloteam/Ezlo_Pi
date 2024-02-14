
file(GLOB_RECURSE src_files "*.c*")

idf_component_register(SRCS "hilink_presence_sensor_setting.c" "sensor_0067_hilink_presence_sensor.c" "${src_files}"
                    INCLUDE_DIRS .
                    REQUIRES    ezlopi-util-trace 
                                ld2410
                                ezlopi-core-timer 
                                ezlopi-core-cloud 
                                ezlopi-core-actions  
                                ezlopi-core-devices 
                                ezlopi-core-valueformatter

                                ezlopi-cloud-items
                                ezlopi-cloud-constants 
                                
                                ezlopi-service-gpioisr 
                                )

