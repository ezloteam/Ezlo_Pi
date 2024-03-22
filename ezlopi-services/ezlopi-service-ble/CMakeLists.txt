# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "*.c*")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-devices 
        ezlopi-core-ble
        ezlopi-core-reset
        # ezlopi-user-config
)


idf_component_register(SRCS "ezlopi_service_ble_provisioning.c" "ezlopi_service_ble.c" "ezlopi_service_ble_dynamic_config.c" "ezlopi_service_ble_wifi.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_CORE_COMPONENTS} 
    ) 

 
