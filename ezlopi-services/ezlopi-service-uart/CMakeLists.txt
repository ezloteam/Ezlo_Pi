# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "./*.c*")


set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-wifi 
        ezlopi-core-nvs
        ezlopi-core-factory-info
        ezlopi-core-net
        ezlopi-core-reset
        ezlopi-core-ezlopi
        ezlopi-core-sntp
        ezlopi-core-info
        ezlopi-core-processes
        ezlopi-core-ping
)
set(EZLOPI_HAL_COMPONENTS 
        ezlopi-hal-sysinfo
)
set(EZLOPI_CLOUD_COMPONENTS 
        ezlopi-cloud-info
)

set(EZLOPI_SERVICE_COMPONENTS 
        ezlopi-service-ble
)

idf_component_register(SRCS "ezlopi_service_uart.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_CORE_COMPONENTS} 
                ${EZLOPI_HAL_COMPONENTS}
                ${EZLOPI_CLOUD_COMPONENTS} 
                ${EZLOPI_SERVICE_COMPONENTS}
)