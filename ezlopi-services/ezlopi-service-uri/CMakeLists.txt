

file(GLOB_RECURSE src_files "*.c")

set(ESP_IDF_COMPONENTS
        esp_http_server
        nvs_flash
        cjext
        spiffs
)
set(EZLOPI_COMPONENTS
        ezlopi-util-trace
        dns_hijacking
)


idf_component_register(SRCS "ezlopi_service_uri.c" "${src_files}"
                        INCLUDE_DIRS "."
                        REQUIRES    ${ESP_IDF_COMPONENTS}
                                    ${EZLOPI_COMPONENTS}

                    )
