# Include all source files
file(GLOB_RECURSE config_src "*.c")

#Define SDK components
set(ESP_IDF_COMPONENTS
        freertos 
)

# Define core components
set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-factory-info                     
        ezlopi-core-websocket_client 
        ezlopi-core-processes 
        ezlopi-core-errors
)

# Define HAL components
set(EZLOPI_HAL_COMPONENTS 
)

# Define cloud components
set(EZLOPI_CLOUD_COMPONENTS 
)

# Define service components
set(EZLOPI_SERV_COMPONENTS
        ezlopi-service-webprov 
        ezlopi-service-loop
)

# Define sensor device components
set(EZLOPI_SENSOR_DEVICE_COMPONENTS 
)

# Define ezlopi-utils components
set(EZLOPI_UTIL_COMPONENTS
        ezlopi-util-trace
        ezlopi-util-version
)

# Register the components
idf_component_register(SRCS  "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES        ${ESP_IDF_COMPONENTS}
                                        ${EZLOPI_CORE_COMPONENTS}
                                        ${EZLOPI_HAL_COMPONENTS}
                                        ${EZLOPI_CLOUD_COMPONENTS}
                                        ${EZLOPI_SERV_COMPONENTS}
                                        ${EZLOPI_SENSOR_DEVICE_COMPONENTS}  
                                        ${EZLOPI_UTIL_COMPONENTS}              
)
