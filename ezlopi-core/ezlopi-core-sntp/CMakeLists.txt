file(GLOB_RECURSE config_src "*.c")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-util-trace
)
set(EZLOPI_HAL_COMPONENTS 

)
set(EZLOPI_CLOUD_COMPONENTS 
        
)
set(EZLOPI_SENSOR_DEVICE_COMPONENTS

)


idf_component_register(SRCS "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES        ${EZLOPI_CORE_COMPONENTS}
                                        ${EZLOPI_HAL_COMPONENTS}
                                        ${EZLOPI_CLOUD_COMPONENTS}
                                        ${EZLOPI_SENSOR_DEVICE_COMPONENTS}                
                        )


