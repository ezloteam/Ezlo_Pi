file( GLOB_RECURSE src_files "*.c*" )

set(ESP_IDF_COMPONENTS
)
set(EZLOPI_COMPONENTS
        ezlopi-util-trace
)
set(EZLOPI_CORE_COMPONENTS
        ezlopi-core-ezlopi
)


idf_component_register(SRCS  "${src_files}"
                       INCLUDE_DIRS "."
                       REQUIRES     ${ESP_IDF_COMPONENTS}
                                    ${EZLOPI_COMPONENTS}
                                    ${EZLOPI_CORE_COMPONENTS}
                        )

