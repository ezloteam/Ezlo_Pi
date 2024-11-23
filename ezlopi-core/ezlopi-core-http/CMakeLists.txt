# set(config_src "config.cpp")

file(GLOB_RECURSE config_src "*.c")

set(ESP_IDF_COMPONENTS
        cjext
        mbedtls
        esp-tls
        esp_http_client
)

set(EZLOPI_COMPONENTS
        ezlopi-util-trace
)

set(EZLOPI_CORE_COMPONENTS
        ezlopi-core-event-group
        ezlopi-core-errors
)

idf_component_register(SRCS "ezlopi_core_http.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${ESP_IDF_COMPONENTS}
                ${EZLOPI_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS}
    )
