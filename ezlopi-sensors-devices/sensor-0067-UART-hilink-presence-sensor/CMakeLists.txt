# Include all source files
file(GLOB_RECURSE config_src "*.c")

#Define SDK components
set(ESP_IDF_COMPONENTS
        #cjext
        ld2410
)

# Define core components
set(EZLOPI_CORE_COMPONENTS 
        # ezlopi-core-timer 
        ezlopi-core-cloud 
        ezlopi-core-actions 
        ezlopi-core-devices 
        ezlopi-core-valueformatter
        ezlopi-core-errors
)

# Define HAL components
set(EZLOPI_HAL_COMPONENTS 
        ezlopi-hal-adc
)

# Define cloud components
set(EZLOPI_CLOUD_COMPONENTS 
        ezlopi-cloud-items 
        ezlopi-cloud-constants
)

# Define service components
set(EZLOPI_SERV_COMPONENTS
        ezlopi-service-gpioisr 
)

# Define sensor device components
set(EZLOPI_SENSOR_DEVICE_COMPONENTS 
)

# Define ezlopi-utils components
set(EZLOPI_UTIL_COMPONENTS
        ezlopi-util-trace
)

# Register the components
idf_component_register(SRCS "hilink_presence_sensor_setting.c" "sensor_0067_hilink_presence_sensor.c" "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES        ${ESP_IDF_COMPONENTS}
                                        ${EZLOPI_CORE_COMPONENTS}
                                        ${EZLOPI_HAL_COMPONENTS}
                                        ${EZLOPI_CLOUD_COMPONENTS}
                                        ${EZLOPI_SERV_COMPONENTS}
                                        ${EZLOPI_SENSOR_DEVICE_COMPONENTS}  
                                        ${EZLOPI_UTIL_COMPONENTS}                
)
