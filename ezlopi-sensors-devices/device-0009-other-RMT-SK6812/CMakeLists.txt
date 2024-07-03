

idf_component_register(SRCS "device_0009_other_RMT_SK6812.c"
            INCLUDE_DIRS "."
            REQUIRES    
                        driver
                        led_strip

                        ezlopi-util-trace 

                        ezlopi-core-timer 
                        ezlopi-core-cloud 
                        ezlopi-core-actions 
                        ezlopi-core-devices 
                        ezlopi-core-valueformatter 
                        ezlopi-core-errors

                        #ezlopi-hal-i2c
                        #ezlopi-hal-spi
                        
                        ezlopi-cloud-items 
                        ezlopi-cloud-constants 

                        ezlopi-service-gpioisr
                        )

