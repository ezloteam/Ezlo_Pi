file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0015_oneWire_DHT11.c" "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES trace 
                                driver 
                                ezlopi-core-actions 
                                 
                                ezlopi-core-timer 
                                ezlopi-core-devices 
                                ezlopi-cloud-constants 
                                ezlopi-service-gpioisr
                                ezlopi-core-valueformatter)