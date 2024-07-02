# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "*.c")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-actions
        ezlopi-core-event-queue
        ezlopi-core-devices 
        ezlopi-core-processes
)


idf_component_register(SRCS "ezlopi_service_loop.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_CORE_COMPONENTS} 
)