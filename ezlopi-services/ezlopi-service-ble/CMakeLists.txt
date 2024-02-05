# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "*.c*")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-devices 
        ezlopi-core-ble
        ezlopi-core-reset
)


idf_component_register(SRCS "ezlopi_ble_service_security.c" "ezlopi_service_ble_security.c" "ezlopi_service_ble_device_info.c" "ezlopi_service_ble.c" "ezlopi_service_ble_dynamic_config.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_CORE_COMPONENTS} 
    ) 
