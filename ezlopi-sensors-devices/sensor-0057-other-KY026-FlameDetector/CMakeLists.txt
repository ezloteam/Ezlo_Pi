
idf_component_register(SRCS "sensor_0057_other_KY026_FlameDetector.c" 
            INCLUDE_DIRS "."
            REQUIRES    ezlopi-util-trace 
                        # ezlopi-core-timer 
                        ezlopi-core-cloud 
                        ezlopi-core-actions 
                        ezlopi-core-devices 
                        ezlopi-core-valueformatter 
                        ezlopi-core-errors
                        
                        ezlopi-hal-adc

                        ezlopi-cloud-items
                        ezlopi-cloud-constants 
                        )

