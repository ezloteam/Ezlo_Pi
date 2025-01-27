# Include all source files
file(GLOB_RECURSE config_src "*.c")

#Define SDK components
set(ESP_IDF_COMPONENTS
        #cjext
)

# Define core components
set(EZLOPI_CORE_COMPONENTS
        ezlopi-core-cloud
        ezlopi-core-actions
        ezlopi-core-devices
)

# Define HAL components
set(EZLOPI_HAL_COMPONENTS
        ezlopi-hal-gpio
        ezlopi-hal-adc
        ezlopi-hal-pwm
        ezlopi-hal-onewire
        ezlopi-hal-uart
        ezlopi-hal-i2c
        ezlopi-hal-spi
)

# Define cloud components
set(EZLOPI_CLOUD_COMPONENTS
        ezlopi-cloud-settings
)

# Define service components
set(EZLOPI_SERV_COMPONENTS
        #ezlopi-service-loop
)

# Define sensor device components
set(EZLOPI_SENSOR_DEVICE_COMPONENTS
        device-0001-digitalOut-generic
        # Add other sensor device components here
)

# Define ezlopi-utils components
set(EZLOPI_UTIL_COMPONENTS
        ezlopi-util-trace
)

# Register the components
idf_component_register(SRCS "source.c" "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES        ${ESP_IDF_COMPONENTS}
                                        ${EZLOPI_CORE_COMPONENTS}
                                        ${EZLOPI_HAL_COMPONENTS}
                                        ${EZLOPI_CLOUD_COMPONENTS}
                                        ${EZLOPI_SERV_COMPONENTS}
                                        ${EZLOPI_SENSOR_DEVICE_COMPONENTS}  
                                        ${EZLOPI_UTIL_COMPONENTS}                
)
