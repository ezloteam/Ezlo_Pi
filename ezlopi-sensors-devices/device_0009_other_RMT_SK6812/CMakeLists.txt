

idf_component_register(SRCS "device_0009_other_RMT_SK6812.c"
                        INCLUDE_DIRS "."
                        REQUIRES trace 
                        led_strip
                        ezlopi-core-actions 
                        ezlopi-hal-spi
                        ezlopi-core-timer 
                        ezlopi-core-devices 
                        ezlopi-cloud-constants 
                        ezlopi-service-gpioisr
                        )

