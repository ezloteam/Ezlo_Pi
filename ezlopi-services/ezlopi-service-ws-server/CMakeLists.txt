
file(GLOB_RECURSE provisioning_src "*.c*")

set(EZLOPI_UTIL_COMPONENTS
        ezlopi-util-trace
)

set(SDK_COMPONENTS
        esp_http_server
)

set(EZLOPI_CORE_COMPONENTS  
        ezlopi-core-wifi
        ezlopi-core-ezlopi
)

set(EZLOPI_CLOUD_COMPONENTS 
        
)



idf_component_register(SRCS "ezlopi_service_ws_server.c" "ezlopi_service_ws_server_clients.c" "${provisioning_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${SDK_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS} 
                ${EZLOPI_UTIL_COMPONENTS}
                ${EZLOPI_CLOUD_COMPONENTS} 
    )
