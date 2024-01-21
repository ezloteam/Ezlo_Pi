# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE provisioning_src "*.c*")
idf_component_register(SRCS  "${provisioning_src}"
    INCLUDE_DIRS "."
    REQUIRES    ezlopi-cloud-ota json ezlopi-core-wifi ezlopi-service-webprov
)