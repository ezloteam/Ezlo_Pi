file(GLOB_RECURSE config_src "*.c")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-cloud 
        ezlopi-core-devices 
        ezlopi-core-errors
)
set(EZLOPI_HAL_COMPONENTS 

)
set(EZLOPI_CLOUD_COMPONENTS 
        
)
set(EZLOPI_SENSOR_DEVICE_COMPONENTS

)

set(EZLOPI_SERVICE_COMPONENTS
        ezlopi-service-modes
)


idf_component_register(SRCS "ezlopi_core_default_modes.c" "ezlopi_core_modes_cjson.c" "ezlopi_core_modes.c" "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES        ${EZLOPI_CORE_COMPONENTS}
                                        ${EZLOPI_HAL_COMPONENTS}
                                        ${EZLOPI_CLOUD_COMPONENTS}
                                        ${EZLOPI_SENSOR_DEVICE_COMPONENTS}     
                                        ${EZLOPI_SERVICE_COMPONENTS}           
                        )


