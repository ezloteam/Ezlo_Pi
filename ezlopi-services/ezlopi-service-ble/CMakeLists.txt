# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "*.c*")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-devices 
        ezlopi-core-ble 
)


idf_component_register(SRCS "ezlopi_service_ble_device_info.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_CORE_COMPONENTS} 
    ) 
