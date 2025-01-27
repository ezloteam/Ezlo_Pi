
file(GLOB_RECURSE config_src "./*.c*")

set(EZLOPI_CLOUD_COMPONENTS
        ezlopi-cloud-constants
        ezlopi-core-devices
)

idf_component_register(SRCS "ezlopi_core_valueformatter.c" "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES ${EZLOPI_CLOUD_COMPONENTS})
