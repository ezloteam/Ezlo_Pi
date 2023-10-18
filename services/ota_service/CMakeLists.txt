# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE provisioning_src "*.c*")
idf_component_register(SRCS "ota_service.c" "${provisioning_src}"
    INCLUDE_DIRS "."
    REQUIRES    firmware json
)