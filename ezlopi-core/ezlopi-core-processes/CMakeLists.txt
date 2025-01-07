file(GLOB_RECURSE config_src "*.c")

set(ESP_IDF_COMPONENTS
        cjext
)
set(EZLOPI_UTIL_COMPONENTS
        ezlopi-util-trace
)

idf_component_register(SRCS  "${config_src}"
                        INCLUDE_DIRS "."
                        REQUIRES ${ESP_IDF_COMPONENTS}
                        ${EZLOPI_UTIL_COMPONENTS})

