file(GLOB_RECURSE src_files "*.c*")


set(EZLOPI_UTIL_COMPONENTS
        ezlopi-util-trace
)

set(SDK_COMPONENTS
        esp_http_server
)

set(EZLOPI_CORE_COMPONENTS
        ezlopi-core-api
        ezlopi-core-wifi
        ezlopi-core-buffer
        ezlopi-core-ezlopi
        ezlopi-core-factory-info
        ezlopi-core-offline-login
)

set(EZLOPI_CLOUD_COMPONENTS

)



idf_component_register(SRCS "ezlopi_service_ws_server_clients.c" "${src_files}"
    INCLUDE_DIRS "."
    REQUIRES    ${SDK_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS}
                ${EZLOPI_UTIL_COMPONENTS}
                # ${EZLOPI_CLOUD_COMPONENTS}
    )
