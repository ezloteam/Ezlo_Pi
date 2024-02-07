# Include all source files
file(GLOB_RECURSE config_src "*.c")

#Define SDK components
set(ESP_IDF_COMPONENTS
        json
)

# Define core components
set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-factory-info
        ezlopi-cloud-constants
        ezlopi-core-modes 
        ezlopi-core-sntp
)

# Define HAL components
set(EZLOPI_HAL_COMPONENTS 
)

# Define cloud components
set(EZLOPI_CLOUD_COMPONENTS 
)

# Define sensor device components
set(EZLOPI_SENSOR_DEVICE_COMPONENTS 
)

# Define ezlopi-utils components
set(EZLOPI_UTIL_COMPONENTS
        ezlopi-util-trace
)

# Register the components
idf_component_register(SRCS "ezlopi_cloud_modes_updaters.c" "ezlopi_cloud_modes.c" "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES        ${ESP_IDF_COMPONENTS}
                                        ${EZLOPI_CORE_COMPONENTS}
                                        ${EZLOPI_HAL_COMPONENTS}
                                        ${EZLOPI_CLOUD_COMPONENTS}
                                        ${EZLOPI_SENSOR_DEVICE_COMPONENTS}  
                                        ${EZLOPI_UTIL_COMPONENTS}              
)
