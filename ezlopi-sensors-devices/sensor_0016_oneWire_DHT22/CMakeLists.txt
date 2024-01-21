file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "dht22.c" "sensor_0016_oneWire_DHT22.c" "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES trace 
                                driver 
                                ezlopi-core-actions 
                                 
                                ezlopi-core-timer 
                                ezlopi-core-devices 
                                ezlopi-cloud-constants 
                                ezlopi-service-gpioisr
                                ezlopi-core-valueformatter)