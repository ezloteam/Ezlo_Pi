file(GLOB_RECURSE files_src "./*.c*")

set(EZLOPI_SERVICE_COMPONENTS 
        ezlopi-service-webprov
        ezlopi-core-factory-info
        ezlopi-core-nvs
        ezlopi-util-trace
)

idf_component_register(SRCS "ezlopi_core_reset.c" "${files_src}"
    INCLUDE_DIRS "."
    REQUIRES   ${EZLOPI_SERVICE_COMPONENTS})
