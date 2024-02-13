
file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "${config_src}"
                    INCLUDE_DIRS "."
                    REQUIRES    ezlopi-util-trace 
                                ezlopi-core-devices
                                ezlopi-core-actions
                                ezlopi-core-valueformatter
                                
                                ezlopi-hal-i2c
                    )
