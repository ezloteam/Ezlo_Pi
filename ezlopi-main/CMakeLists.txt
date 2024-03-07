# Include all source files
file(GLOB_RECURSE config_src "*.c")

# Define SDK components
set(EZLOPI_SDK_COMPONENTS
        # driver
        # nghttp
        # esp_http_server
        # protocol_examples_common
)

# Define ezlopi-components
set(EZLOPI_COMPONENTS
        ezlopi-util-trace
)

# Define core components
set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-ezlopi
)

# Define HAL components
set(EZLOPI_HAL_COMPONENTS 

)

# Define cloud components
set(EZLOPI_CLOUD_COMPONENTS 

)

# Define sensor device components
set(EZLOPI_SENSOR_DEVICE_COMPONENTS 
        # Add other sensor device components here
)

# Define service components
set(EZLOPI_SERVICES
ezlopi-service-ble
        ezlopi-service-ota
        ezlopi-service-uart
        ezlopi-service-modes
        ezlopi-service-timer
        ezlopi-service-webprov
        ezlopi-service-gpioisr
        ezlopi-service-meshbot 
        ezlopi-service-ws-server
        ezlopi-service-broadcast
)

# Register the components
idf_component_register(SRCS "ezlopi_main.c" "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES        ${EZLOPI_COMPONENTS}
                                        ${EZLOPI_CORE_COMPONENTS}
                                        ${EZLOPI_HAL_COMPONENTS}
                                        ${EZLOPI_CLOUD_COMPONENTS}
                                        ${EZLOPI_SENSOR_DEVICE_COMPONENTS}               
                                        ${EZLOPI_SERVICES} 
)
