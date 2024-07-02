
file(GLOB_RECURSE provisioning_src "*.c*")

set(EZLOPI_UTIL_COMPONENTS
        ezlopi-util-trace
)
set(EZLOPI_CORE_COMPONENTS  
        ezlopi-core-api
        ezlopi-core-wifi
        ezlopi-core-http
        ezlopi-core-buffer
        ezlopi-core-ezlopi
        ezlopi-core-event-group
        ezlopi-core-factory-info 
        ezlopi-core-websocket_client 
        ezlopi-core-processes
)

set(EZLOPI_CLOUD_COMPONENTS 
)



idf_component_register(SRCS "ezlopi_test_prov.c" "ezlopi_service_webprov.c" "${provisioning_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${EZLOPI_UTIL_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS} 
                ${EZLOPI_CLOUD_COMPONENTS} 
    )
