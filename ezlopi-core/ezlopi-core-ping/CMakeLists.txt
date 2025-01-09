
file(GLOB_RECURSE config_src "./*.c*" "./ezlopi-ping/*.c*")

set(EZLOPI_COMPONENTS
        ezlopi-util-trace
        ezlopi-service-loop

        ezlopi-core-setting-commands
)

idf_component_register(SRCS  "${config_src}"
    INCLUDE_DIRS "."
    REQUIRES ${EZLOPI_COMPONENTS})
