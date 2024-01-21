file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0047_other_HX711_loadcell.c" "${config_src}"
                        INCLUDE_DIRS "."    
                        REQUIRES    trace
                                    ezlopi-core-actions
                                    ezlopi-cloud-constants
                                    ezlopi-core-devices
                                    
                                    ezlopi-core-timer  
                                    ezlopi-core-valueformatter )