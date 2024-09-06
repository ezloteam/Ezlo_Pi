file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "dht11.c" "sensor_0015_oneWire_DHT11.c" "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES        ezlopi-util-trace 
                                        # ezlopi-core-timer 
                                        ezlopi-core-cloud 
                                        ezlopi-core-actions 
                                        ezlopi-core-devices 
                                        ezlopi-core-valueformatter 
                                        ezlopi-core-setting-commands
                
                                        ezlopi-cloud-items 
                                        ezlopi-cloud-constants 
                                        
                                        ezlopi-service-gpioisr
                                        )