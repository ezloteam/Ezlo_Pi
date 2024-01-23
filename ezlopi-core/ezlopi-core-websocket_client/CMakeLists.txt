file(GLOB_RECURSE websocket_src "*.c")

set(ESP_IDF_COMPONENTS
        esp_websocket_client    
)
set(EZLOPI_CORE_COMPONENTS
        ezlopi-core-factory-info 
)

idf_component_register(SRCS "${websocket_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${ESP_IDF_COMPONENTS}
                ${EZLOPI_CORE_COMPONENTS}
                )
