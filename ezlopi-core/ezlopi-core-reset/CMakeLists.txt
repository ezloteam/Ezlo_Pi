file(GLOB_RECURSE files_src "./*.c*")

set(EZLOPI_SERVICE_COMPONENTS
        ezlopi-core-nvs
        ezlopi-util-trace
        ezlopi-service-webprov
        ezlopi-service-ws-server
        ezlopi-core-factory-info
        ezlopi-core-errors
)

idf_component_register(SRCS  "${files_src}"
    INCLUDE_DIRS "."
    REQUIRES   ${EZLOPI_SERVICE_COMPONENTS})
