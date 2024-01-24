# file( GLOB_RECURSE provisioning_src "*.c*" "actions/*.c*" )
file(GLOB_RECURSE src_files "*.c")

set(EZLOPI_CORE_COMPONENTS 
        ezlopi-core-actions
        ezlopi-core-event-queue
        ezlopi-core-devices 
)


idf_component_register(SRCS "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_CORE_COMPONENTS} 
)