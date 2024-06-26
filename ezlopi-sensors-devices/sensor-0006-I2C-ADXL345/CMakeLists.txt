
file(GLOB_RECURSE config_src "*.c")

idf_component_register(SRCS "adxl345.c" "sensor_0006_I2C_ADXL345.c" "${config_src}"
                    INCLUDE_DIRS "."
                    REQUIRES    ezlopi-util-trace 
                                # ezlopi-core-timer 
                                ezlopi-core-cloud 
                                ezlopi-core-actions 
                                ezlopi-core-devices 
                                ezlopi-core-valueformatter 

                                ezlopi-hal-i2c
                                
                                ezlopi-cloud-items 
                                ezlopi-cloud-constants    
                                )
