file(GLOB_RECURSE config_src "./*.c*")

set(EZLOPI_SERVICE_COMPONENTS 
        ezlopi-service-webprov
)

idf_component_register(SRCS "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES   ${EZLOPI_SERVICE_COMPONENTS})
