file(GLOB_RECURSE config_src "*.c")

set(EZLOPI_CORE_COMPONENTS
        nvs_flash 
        micro_tz_db  
        ezlopi-util-trace
        ezlopi-core-nvs
        ezlopi-core-wifi
)
set(EZLOPI_HAL_COMPONENTS 

)
set(EZLOPI_CLOUD_COMPONENTS 
        
)
set(EZLOPI_SENSOR_DEVICE_COMPONENTS

)


idf_component_register(SRCS "ezlopi_core_sntp.c" "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES        ${EZLOPI_CORE_COMPONENTS}
                                        ${EZLOPI_HAL_COMPONENTS}
                                        ${EZLOPI_CLOUD_COMPONENTS}
                                        ${EZLOPI_SENSOR_DEVICE_COMPONENTS}                
                        )

