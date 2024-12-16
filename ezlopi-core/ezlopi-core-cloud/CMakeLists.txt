file(GLOB_RECURSE config_src "*.c")

set(ESP_IDF_COMPONENTS 
)

set(EZPI_COMPONENTS
        cjext
)


idf_component_register(SRCS "ezlopi_core_cloud.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES    ${ESP_IDF_COMPONENTS}
                ${EZPI_COMPONENTS}
)
