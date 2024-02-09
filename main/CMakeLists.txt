# Include all source files
file(GLOB_RECURSE config_src "*.c")

# Define SDK components
set(EZLOPI_SDK_COMPONENTS
        driver
)

# Define ezlopi-components
set(EZLOPI_COMPONENTS
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
        device-0001-digitalOut-generic
        # Add other sensor device components here
)

# Define service components
set(EZLOPI_SERVICES
        ezlopi-service-webprov
        ezlopi-service-timer
        ezlopi-service-gpioisr
        ezlopi-service-ble
        ezlopi-service-ota
        ezlopi-service-meshbot 
        ezlopi-service-uart
        ezlopi-service-modes
)

# Register the components
idf_component_register(SRCS "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES        ${EZLOPI_CORE_COMPONENTS}
                                        ${EZLOPI_HAL_COMPONENTS}
                                        ${EZLOPI_CLOUD_COMPONENTS}
                                        ${EZLOPI_SENSOR_DEVICE_COMPONENTS}               
                                        ${EZLOPI_SERVICES} 
)
