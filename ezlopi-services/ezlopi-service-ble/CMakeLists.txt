# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "*.c*")

set(EZLOPI_CORE_COMPONENTS
        ezlopi-core-devices
        ezlopi-core-ble
        ezlopi-core-reset
        ezlopi-core-ping
        ezlopi-core-sntp
        ezlopi-core-info
        ezlopi-core-errors
        # ezlopi-user-config
)


idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_CORE_COMPONENTS}
    )
