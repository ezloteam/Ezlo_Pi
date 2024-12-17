
file(GLOB_RECURSE src "*.c*")

set(EZLOPI_UTIL_COMPONENTS
        ezlopi-util-trace
)
set(EZLOPI_CORE_COMPONENTS  
        ezlopi-core-api
        ezlopi-core-wifi
        ezlopi-core-buffer
        ezlopi-core-ezlopi
        ezlopi-core-processes
        ezlopi-core-event-group
        ezlopi-core-factory-info 
        ezlopi-core-websocket_client 
)

set(EZLOPI_CLOUD_COMPONENTS 
)



idf_component_register(SRCS "${src}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_UTIL_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS} 
                ${EZLOPI_CLOUD_COMPONENTS} 
    )
