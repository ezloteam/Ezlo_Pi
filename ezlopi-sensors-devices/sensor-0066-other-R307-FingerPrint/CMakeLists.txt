
idf_component_register(SRCS "sensor_0066_other_R307_FingerPrint.c" "R307_AS606.c"
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-util-trace 
                driver
                freertos
                esp_timer
                
                ezlopi-core-timer 
                ezlopi-core-cloud 
                ezlopi-core-actions 
                ezlopi-core-devices 
                ezlopi-core-valueformatter 

                ezlopi-hal-uart

                ezlopi-cloud-items
                ezlopi-cloud-constants
                ezlopi-service-gpioisr
                )

