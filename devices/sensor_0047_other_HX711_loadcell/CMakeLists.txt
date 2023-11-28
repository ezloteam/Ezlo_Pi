file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "${config_src}"
                        INCLUDE_DIRS "."    
                        REQUIRES    trace
                                    ezlopi_actions
                                    ezlopi_cloud_constants
                                    ezlopi_devices
                                    ezlopi_hal
                                    ezlopi_timer  
                                    ezlopi_valueformatter )