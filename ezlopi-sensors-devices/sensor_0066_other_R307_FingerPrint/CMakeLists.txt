
idf_component_register(SRCS "sensor_0066_other_R307_FingerPrint.c" "R307_AS606.c"
    INCLUDE_DIRS "."
    REQUIRES    trace 
                driver
                esp_timer
                freertos
                ezlopi-core-actions 
                ezlopi-hal-uart
                ezlopi-core-timer 
                ezlopi-core-devices 
                ezlopi-cloud-constants
                ezlopi-core-valueformatter )

