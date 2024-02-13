
file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "${config_src}" "sensor_0068_ENS160_gas_sensor_settings.c"
                    INCLUDE_DIRS "."
                    REQUIRES    ezlopi-util-trace 
                                ezlopi-core-devices
                                ezlopi-core-actions
                                ezlopi-core-valueformatter
                                ezlopi-core-nvs

                                ezlopi-cloud-constants
                                
                                ezlopi-hal-i2c
                    )
