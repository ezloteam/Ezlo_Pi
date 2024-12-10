
idf_component_register(SRCS "sensor_0053_UART_GYGPS6MV2.c" "gyGPS6MV2.c"
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-util-trace
                # ezlopi-core-timer
                ezlopi-core-cloud
                ezlopi-core-actions
                ezlopi-core-devices
                ezlopi-core-valueformatter
                ezlopi-core-errors

                ezlopi-hal-uart

                ezlopi-cloud-items
                ezlopi-cloud-constants
                )
