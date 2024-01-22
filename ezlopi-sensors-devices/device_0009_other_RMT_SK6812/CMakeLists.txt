

idf_component_register(SRCS "device_0009_other_RMT_SK6812.c"
            INCLUDE_DIRS "."
            REQUIRES    trace 
                        led_strip
                        ezlopi-core-timer 
                        ezlopi-core-cloud 
                        ezlopi-core-actions 
                        ezlopi-core-devices 
                        ezlopi-core-valueformatter 
                        ezlopi-hal-spi
                        ezlopi-cloud-constants 
                        ezlopi-service-gpioisr
                        )

