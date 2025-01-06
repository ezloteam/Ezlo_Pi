# Include all source files
file(GLOB_RECURSE config_src "*.c")

#Define SDK components
set(ESP_IDF_COMPONENTS
        esp_common 
        esp_http_client 
        esp-tls 
        esp_hw_support

)

# Define core components
set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-factory-info 
        ezlopi-core-wifi 
        ezlopi-core-websocket_client 
        ezlopi-core-http 
)

# Define HAL components
set(EZLOPI_HAL_COMPONENTS 
)

# Define cloud components
set(EZLOPI_CLOUD_COMPONENTS 
        ezlopi-cloud-data 
        ezlopi-cloud-devices 
        ezlopi-cloud-scenes 
        ezlopi-cloud-registration 
        ezlopi-cloud-favorite 
        ezlopi-cloud-gateways 
        ezlopi-cloud-info 
        ezlopi-cloud-modes 
        ezlopi-cloud-items 
        ezlopi-cloud-room 
        ezlopi-cloud-settings 
        ezlopi-cloud-network 
        ezlopi-cloud-ota 
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
                                        ${EZLOPI_SENSOR_DEVICE_COMPONENTS}  
                                        ${EZLOPI_UTIL_COMPONENTS}              
)
