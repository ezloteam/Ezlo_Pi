file(GLOB_RECURSE config_src "*.c")

# Define SDK components
set(EZLOPI_SDK_COMPONENTS
        driver
        nghttp
        esp_http_server
        protocol_examples_common
        wpa_supplicant
        esp_https_server
)

# Define ezlopi-components
set(EZLOPI_COMPONENTS
        ezlopi-util-uuid
        ezlopi-util-trace
)

# Define core components
set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-ble
        ezlopi-core-sntp
        ezlopi-core-cloud 
        ezlopi-core-ezlopi
        ezlopi-core-scenes
        ezlopi-core-actions
        ezlopi-core-devices 
        ezlopi-core-event-group
)

# Define HAL components
set(EZLOPI_HAL_COMPONENTS 
        ezlopi-hal-adc
        ezlopi-hal-pwm
        ezlopi-hal-i2c
        ezlopi-hal-spi
        ezlopi-hal-uart                           
        ezlopi-hal-gpio
        ezlopi-hal-onewire     
)

# Define cloud components
set(EZLOPI_CLOUD_COMPONENTS 
        ezlopi-cloud-settings
)

# Define sensor device components
set(EZLOPI_SENSOR_DEVICE_COMPONENTS 

        # Add other sensor device components here
)

# Define service components
set(EZLOPI_SERVICES
        
)

# Register the components
idf_component_register(SRCS "ezlopi_core_ws_server.c" "ezlopi_core_ws_server_conn.c" "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES        ${EZLOPI_SERVICES} 
                                        ${EZLOPI_COMPONENTS}
                                        ${EZLOPI_HAL_COMPONENTS}
                                        ${EZLOPI_CORE_COMPONENTS}
                                        ${EZLOPI_CLOUD_COMPONENTS}
                                        ${EZLOPI_SENSOR_DEVICE_COMPONENTS}               
)
