# Include all source files
file(GLOB_RECURSE config_src "*.c")

#Define SDK components
set(ESP_IDF_COMPONENTS
    cjext 
)

# Define core components
set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-factory-info 
        ezlopi-core-devices 
)

# Define HAL components
set(EZLOPI_HAL_COMPONENTS 
)

# Define cloud components
set(EZLOPI_CLOUD_COMPONENTS 
        ezlopi-cloud-constants
)

# Define service components
set(EZLOPI_SERV_COMPONENTS
        ezlopi-service-webprov
)

# Define sensor device components
set(EZLOPI_SENSOR_DEVICE_COMPONENTS 
)

# Define ezlopi-utils components
set(EZLOPI_UTIL_COMPONENTS
        ezlopi-util-trace
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

