file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "sensor_0069_ze08_ch02_gas_sensor.c" "${config_src}" "ze08_ch2o.c"
                    INCLUDE_DIRS "."
                    REQUIRES    ezlopi-util-trace 
                                ezlopi-core-devices
                                ezlopi-core-actions
                                ezlopi-core-valueformatter
                                ezlopi-core-errors
                                
                                ezlopi-hal-uart
                    )